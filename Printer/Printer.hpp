#pragma once
#include "Common.hpp"
#include <fstream>

enum PrintOutputType_T
{
    HEX,
    DEC,
    CHAR,
    BASE64,
};

class Printer
{
    std::ofstream file;

    public:
        Printer();
        Printer(std::string file_path);
        void WriteToFile(byte_buffer buffer, PrintOutputType_T output_type);
        void WriteIoStream(byte_buffer buffer, PrintOutputType_T output_type, std::string spacing = {});
        void WriteIoStream(std::string string);
};
