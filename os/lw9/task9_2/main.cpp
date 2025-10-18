#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <boost/asio.hpp>
#include <thread>
#include <utility>
#include <vector>
#include <iostream>
#include <sstream>

using boost::asio::ip::tcp;

const std::string DEFAULT_ADDRESS = "127.0.0.1";

class Whiteboard : public QWidget
{
Q_OBJECT

public:
    Whiteboard(bool isServer, std::string  address, unsigned short port, QWidget* parent = nullptr)
            : QWidget(parent), isServer(isServer), address(std::move(address)), port(port), ioContext()
    {
        setFixedSize(800, 600);
        setWindowTitle(isServer ? "Server - Whiteboard" : "Client - Whiteboard");

        if (isServer)
        {
            networkThread = std::thread(&Whiteboard::StartServer, this);
        }
        else
        {
            networkThread = std::thread(&Whiteboard::StartClient, this);
        }

        connect(this, &Whiteboard::UpdateLinesFromServer, this, &Whiteboard::HandleNewLines);
    }

    ~Whiteboard() override
    {
        ioContext.stop();
        if (networkThread.joinable())
        {
            networkThread.join();
        }
    }

protected:
    void paintEvent(QPaintEvent* event) override
    {
        QPainter painter(this);
        painter.setPen(QPen(Qt::black, 4));
        for (const auto& line : lines)
        {
            painter.drawLine(line.first, line.second);
        }
    }

    void mousePressEvent(QMouseEvent* event) override
    {
        lastPoint = event->pos();
    }

    void mouseMoveEvent(QMouseEvent* event) override
    {
        if (isServer && (event->buttons() & Qt::LeftButton))
        {
            QPoint currentPoint = event->pos();
            lines.emplace_back(lastPoint, currentPoint);
            update();

            SendLineToClients(lastPoint, currentPoint);
            lastPoint = currentPoint;
        }
    }

signals:
    void UpdateLinesFromServer(const std::vector<std::pair<QPoint, QPoint>>& newLines);

public slots:
    void HandleNewLines(const std::vector<std::pair<QPoint, QPoint>>& newLines)
    {
    // проверить, что id равен родительскому через assert
        lines.insert(lines.end(), newLines.begin(), newLines.end());
        update();
    }
    // применить архитектурный паттерн, который отделил бы отрисовку от обработки
    // переименовать поля класса
    // перевести на асинхронную работу
    // проверить работу с медленной сетью

private:
    bool isServer;
    std::string address;
    unsigned short port;

    QPoint lastPoint;
    std::vector<std::pair<QPoint, QPoint>> lines;

    boost::asio::io_context ioContext;
    std::thread networkThread;

    std::list<std::shared_ptr<tcp::socket>> clientSockets;
    std::mutex socketMutex;

    void StartServer()
    {
        try
        {
            tcp::acceptor acceptor(ioContext, tcp::endpoint(tcp::v4(), port));
            while (true)
            {
                std::shared_ptr<tcp::socket> socket = std::make_shared<tcp::socket>(ioContext);
                acceptor.accept(*socket);

                {
                    std::lock_guard<std::mutex> lock(socketMutex);
                    clientSockets.push_back(socket);
                }

                std::thread([this, socket = std::move(socket)]() mutable {
                    try
                    {
                        boost::asio::streambuf buffer;
                        while (true)
                        {
                            boost::asio::read_until(*socket, buffer, "\n");
                            std::istream stream(&buffer);
                        }
                    }
                    catch (std::exception& e)
                    {
                        std::cerr << "Client disconnected: " << e.what() << std::endl;
                        std::lock_guard<std::mutex> lock(socketMutex);
                        clientSockets.remove(socket);
                    }
                }).detach();
            }
        }
        catch (std::exception& e)
        {
            std::cerr << "Server error: " << e.what() << std::endl;
        }
    }

    // сервер должен обрабатывать ситуацию, когда клиент отсоединился

    void StartClient()
    {
        try
        {
            tcp::socket socket(ioContext);
            socket.connect(tcp::endpoint(boost::asio::ip::address::from_string(address), port));

            std::vector<std::pair<QPoint, QPoint>> newLines;
            boost::asio::streambuf buffer;

            while (true)
            {
                boost::asio::read_until(socket, buffer, "\n");
                std::istream stream(&buffer);

                int x1, y1, x2, y2;
                if (stream >> x1 >> y1 >> x2 >> y2)
                {
                    QPoint start(x1, y1);
                    QPoint end(x2, y2);

                    newLines.emplace_back(start, end);
                    emit UpdateLinesFromServer(newLines);
                    newLines.clear();
                }
            }
        }
        catch (std::exception& e)
        {
            std::cerr << "Client error: " << e.what() << std::endl;
        }
    }

    void SendLineToClients(const QPoint& start, const QPoint& end)
    {
        std::lock_guard<std::mutex> lock(socketMutex);
        for (const auto& socket : clientSockets)
        {
            try
            {
                if (socket && socket->is_open())
                {
                    std::ostringstream oss;
                    oss << start.x() << " " << start.y() << " " << end.x() << " " << end.y() << "\n";
                    boost::asio::write(*socket, boost::asio::buffer(oss.str()));
                }
            }
            catch (std::exception& e)
            {
                std::cerr << "Send to client failed: " << e.what() << std::endl;
            }
        }
    }
};

#include "main.moc"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    qRegisterMetaType<std::vector<std::pair<QPoint, QPoint>>>("std::vector<std::pair<QPoint, QPoint>>");
    // cкорость орисовки не должна зависеть от продолжтительности рисаования
    if (argc == 2)
    {
        unsigned short port = std::stoi(argv[1]);
        Whiteboard whiteboard(true, DEFAULT_ADDRESS, port);
        whiteboard.show();
        return QApplication::exec();
    }
    else if (argc == 3)
    {
        std::string address = argv[1];
        unsigned short port = std::stoi(argv[2]);
        Whiteboard whiteboard(false, address, port);
        whiteboard.show();
        return QApplication::exec();
    }
    else
    {
        std::cerr << "Usage:\n"
                  << "Server: whiteboard PORT\n"
                  << "Client: whiteboard ADDRESS PORT\n";
        return EXIT_FAILURE;
    }
}
