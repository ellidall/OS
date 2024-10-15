#include <iostream>
#include <string>
#include <cstdint>
#include <map>

const std::string INPUT = "Naq rju 5rj fu gorj qsw!";

char EncodeChar(uint8_t dil)
{
    auto rdi = static_cast<uint64_t>(dil);
    auto eax = static_cast<uint32_t>(rdi - 65);
    auto al = static_cast<uint8_t>(eax);
    if (al <= 25) {
        auto edi = static_cast<uint32_t>(dil);
        eax = 90;
        eax -= edi;
        auto rax = static_cast<uint64_t>(eax);
        auto edx = static_cast<uint32_t>(rax + rax * 2);
        auto rdx = static_cast<uint64_t>(edx);
        rax = rdx;
        rdx *= 1321528399;
        rdx >>= 35;
        edx = static_cast<uint32_t>(rdx);
        edx *= 26;
        eax = static_cast<uint32_t>(rax);
        eax -= edx;
        eax += 65;
        return static_cast<char>(eax);
    }

    eax = static_cast<uint32_t>(rdi - 97);
    al = static_cast<uint8_t>(eax);
    if (al <= 25) {
        auto edi = static_cast<uint32_t>(dil);
        rdi = static_cast<uint64_t>(edi);
        uint32_t edx = rdi - 485 + rdi * 4;
        auto rax = static_cast<uint64_t>(eax);
        auto rdx = static_cast<uint64_t>(edx);
        rax = rdx;
        rdx = rdx * 1321528399;
        rdx >>= 35;
        edx = static_cast<uint32_t>(rdx);
        edx = edx * 26;
        eax = static_cast<uint32_t>(rax);
        eax -= edx;
        eax += 97;
        return static_cast<char>(eax);
    }

    auto edx = static_cast<uint32_t>(rdi - 48);
    auto edi = static_cast<uint32_t>(rdi);
    eax = edi;
    auto dl = static_cast<uint8_t>(edx);
    if (dl <= 9) {
        edi = static_cast<uint32_t>(dil);
        eax = 57;
        uint32_t ecx = 3435973837;
        eax -= edi;
        edx = static_cast<uint32_t>(dl);
        edx = eax * 23;
        auto rax = static_cast<uint64_t>(eax);
        auto rdx = static_cast<uint64_t>(edx);
        rax = rdx;
        auto rcx = static_cast<uint64_t>(ecx);
        rdx *= rcx;
        rdx >>= 35;
        edx = rdx + rdx * 4;
        edx += edx;
        eax = static_cast<uint32_t>(rax);
        eax -= edx;
        eax += 48;
        return static_cast<char>(eax);
    }

    return static_cast<char>(dil);
}

std::string EncodeString(const std::string& str)
{
    std::string encoded;
    encoded.reserve(str.size());

    for (char ch : str)
    {
        encoded += EncodeChar(static_cast<uint8_t>(ch));
    }

    return encoded;
}

std::map<char, char> PrepareDictionary()
{
    std::string alphabet = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::map<char, char> dictionary;

    for (auto ch : alphabet)
    {
        dictionary.insert({ EncodeChar(ch), ch });
    }

    return dictionary;
}

std::string DecodeString(const std::string& str, const std::map<char, char>& dict)
{
    std::string decoded;
    decoded.reserve(str.size());

    for (const char ch : str)
    {
        auto it = dict.find(ch);
        if (it != dict.end())
        {
            decoded += it->second;
        }
        else
        {
            decoded += ch;
        }
    }

    return decoded;
}

int main()
{
    std::map<char, char> dictionary = PrepareDictionary();

    std::string in;
    while (in != "exit")
    {
        std::cout << "Input: ";
        std::getline(std::cin, in);
        std::cout << "Decoded: " << DecodeString(in, dictionary) << std::endl;
    }

    return EXIT_SUCCESS;
}