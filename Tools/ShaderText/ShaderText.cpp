#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <string>

// WWShade assembles this text at runtime; the array is deliberately not shader bytecode.
int main(int argc, char** argv)
{
    if (argc != 4) {
        std::cerr << "Usage: ShaderText input output symbol\n";
        return 1;
    }
    const std::string symbol = argv[3];
    if (symbol.empty() || symbol.find_first_not_of(
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_") != std::string::npos ||
        (symbol.front() >= '0' && symbol.front() <= '9')) {
        std::cerr << "Invalid array symbol\n";
        return 1;
    }
    std::ifstream input(argv[1], std::ios::binary);
    if (!input) {
        std::cerr << "Cannot open shader text: " << argv[1] << '\n';
        return 1;
    }
    std::string text((std::istreambuf_iterator<char>(input)), {});
    if (input.bad()) {
        std::cerr << "Cannot read shader text: " << argv[1] << '\n';
        return 1;
    }
    text.push_back('\0');
    while (text.size() % 4) text.push_back('\0');
    std::ofstream output(argv[2], std::ios::binary | std::ios::trunc);
    output << "// Generated shader text.\nDWORD " << symbol << "[] = {\n";
    output << std::hex << std::setfill('0');
    for (std::size_t offset = 0; offset < text.size(); offset += 4) {
        std::uint32_t word = 0;
        for (unsigned byte = 0; byte < 4; ++byte)
            word |= std::uint32_t(static_cast<unsigned char>(text[offset + byte])) << (8 * byte);
        output << "    0x" << std::setw(8) << word << ",\n";
    }
    output << "};\n";
    output.close();
    if (!output) {
        std::cerr << "Cannot write shader header: " << argv[2] << '\n';
        return 1;
    }
    return 0;
}
