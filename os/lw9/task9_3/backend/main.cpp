#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <thread>
#include <memory>
#include <shared_mutex>

using namespace boost::asio;
using namespace boost::beast;
using boost::asio::ip::tcp;
using json = nlohmann::json;

constexpr const unsigned short DEFAULT_PORT = 8080;
const std::string PREFIX_HTTP = "http://";
const std::string DEFAULT_DOMAIN = "localhost";
const std::string PORT_SEPARATOR = ":";
const std::string URL_ADD = "/add";

struct Document
{
    std::string url;
    std::string title;
    std::string content;
};

// использовать структуры данных для быстрого поиска
// проверить так, чтобы
struct Content
{
    std::map<std::string, std::shared_ptr<Document>> documents;
    std::shared_mutex mutex;

    void AddDocument(const std::string& url, const std::string& title, const std::string& content)
    {
        std::unique_lock<std::shared_mutex> lock(mutex);
        auto doc = std::make_shared<Document>(Document{url, title, content});
        documents[url] = doc;
    }

    std::vector<std::shared_ptr<Document>> SearchDocuments(std::string url)
    {
        std::shared_lock<std::shared_mutex> lock(mutex);
        std::vector<std::shared_ptr<Document>> results;

        if (!url.empty() && url[0] == '/')
        {
            url.erase(0, 1);
        }

        for (const auto& pair : documents)
        {
            auto doc = pair.second;
            if (doc->url.find(url) != std::string::npos
                || doc->title.find(url) != std::string::npos
                || doc->content.find(url) != std::string::npos)
            {
                results.push_back(doc);
            }
        }

        std::sort(results.begin(), results.end(),
                [&](const auto& doc1, const auto& doc2) {
                    return GetTfIdfScore(url, doc1) > GetTfIdfScore(url, doc2);
                }
        );
        return results;
    }

private:
    double GetTfIdfScore(const std::string& url, const std::shared_ptr<Document>& doc)
    {
        double tf = GetTermFrequency(url, doc);
        double idf = GetInverseDocumentFrequency(url);
        return tf * idf;
    }

    static double GetTermFrequency(const std::string& url, const std::shared_ptr<Document>& doc)
    {
        size_t termCount = 0;
        size_t totalWords = 0;
        std::string text = doc->title + " " + doc->content;
        size_t pos = 0;
        while ((pos = text.find(url, pos)) != std::string::npos)
        {
            ++termCount;
            pos += url.length();
        }

        totalWords = std::count(text.begin(), text.end(), ' ') + 1;

        return (totalWords > 0) ? static_cast<double>(termCount) / static_cast<double>(totalWords) : 0.0;
    }

    double GetInverseDocumentFrequency(const std::string& url)
    {
        size_t totalDocuments = documents.size();

        size_t docFrequency = 0;
        for (const auto& pair : documents)
        {
            const auto& doc = pair.second;
            if (doc->title.find(url) != std::string::npos ||
                doc->content.find(url) != std::string::npos)
            {
                ++docFrequency;
            }
        }

        return log(static_cast<double>(totalDocuments) / (1 + static_cast<double>(docFrequency)));
    }
};

class DocumentServer
{
public:
    DocumentServer(io_context& ioContext, unsigned short port, Content& index)
            : m_ioContext(ioContext), m_port(port), m_acceptor(ioContext, tcp::endpoint(tcp::v4(), port)),
              m_content(index)
    {}

    [[noreturn]] void Run()
    {
        std::cout << "Server is running on " << PREFIX_HTTP << DEFAULT_DOMAIN << PORT_SEPARATOR << m_port << std::endl;
        while (true)
        {
            tcp::socket socket(m_ioContext);
            m_acceptor.accept(socket);
            std::thread(&DocumentServer::HandleRequest, this, std::move(socket)).detach();
        }
    }

private:
    io_context& m_ioContext;
    tcp::acceptor m_acceptor;
    unsigned short m_port;
    Content& m_content;

    void HandleRequest(tcp::socket socket)
    {
        try
        {
            boost::beast::flat_buffer buffer;
            http::request<http::string_body> req;

            http::read(socket, buffer, req);
            http::response<http::string_body> res;
            res.set(http::field::content_type, "application/json");
            res.set(http::field::access_control_allow_origin, "*");
            res.set(http::field::access_control_allow_methods, "POST, GET, OPTIONS");
            res.set(http::field::access_control_allow_headers, "Content-Type");

            std::string url = req.target().to_string();
            if (req.method() == http::verb::options)
            {
                res.result(http::status::no_content);
            }
            else if (req.method() == http::verb::get)
            {
                HandleCommandSearch(url, res);
            }
            else if (url == URL_ADD && req.method() == http::verb::post)
            {
                HandleCommandAdd(req, res);
            }
            else
            {
                res.result(http::status::bad_request);
            }

            http::write(socket, res);
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

    void HandleCommandSearch(std::string& url, http::response<http::string_body>& res)
    {
        auto results = m_content.SearchDocuments(url);

        res.result(http::status::ok);
        json responseJson;
        for (const auto& doc : results)
        {
            responseJson.push_back({
                    {"url",     doc->url},
                    {"title",   doc->title},
                    {"content", doc->content}
            });
        }
        res.body() = responseJson.dump();
        if (results.empty())
        {
            res.body() = "[]";
        }
    }

    void HandleCommandAdd(http::request<http::string_body>& req, http::response<http::string_body>& res)
    {
        try
        {
            auto body = json::parse(req.body());
            std::string currentUrl = body["url"];
            std::string title = body["title"];
            std::string content = body["content"];
            m_content.AddDocument(currentUrl, title, content);
            res.result(http::status::ok);
        }
        catch (const std::exception& e)
        {
            res.result(http::status::bad_request);
        }
    }
};

int main()
{
    try
    {
        io_context ioContext;
        Content content;
        DocumentServer server(ioContext, DEFAULT_PORT, content);
        server.Run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}