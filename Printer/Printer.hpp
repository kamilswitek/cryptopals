#pragma once
#include "Common.hpp"

enum PrintOutputType_T
{
    HEX,
    DEC,
    CHAR,
    BASE64,
};

namespace Printer
{
    void WriteIoStream(byte_buffer buffer, PrintOutputType_T output_type, std::string spacing = {});
    void WriteIoStream(std::string string);
}
