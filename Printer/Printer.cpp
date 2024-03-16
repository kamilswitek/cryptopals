#include "Printer.hpp"
#include "FormatConversions.hpp"
#include <iostream>
#include <iomanip>

Printer::Printer()
{

}

Printer::Printer(std::string file_path)
{
    file.open(file_path);
}

void Printer::WriteIoStream(byte_buffer buffer, PrintOutputType_T output_type, std::string spacing)
{
    switch(output_type)
    {
        case PrintOutputType_T::HEX:
            for(unsigned short byte : buffer)
            {
                std::cout << std::hex << std::setw(2) << std::setfill('0') << byte << spacing;
            }
            std::cout << std::endl;
            break;

        case PrintOutputType_T::DEC:
            for(unsigned short byte : buffer)
            {
                std::cout << std::dec << byte << spacing;
            }
            std::cout << std::endl;
            break;

        case PrintOutputType_T::CHAR:
            for(unsigned short byte : buffer)
            {
                std::cout << static_cast<char>(byte) << spacing;
            }
            std::cout << std::endl;
            break;

        case PrintOutputType_T::BASE64:
            for(char element : FormatConversions::Base64Encoder(buffer))
            {
                std::cout << element;
            }
            std::cout << std::endl;
            break;

        default:
            break;
    }
}

void Printer::WriteIoStream(std::string string)
{
    std::cout << string << std::endl;
}