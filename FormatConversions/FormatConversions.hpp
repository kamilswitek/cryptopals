#pragma once
#include "Common.hpp"

namespace FormatConversions
{
    byte_buffer CharString2ByteBuffer(std::string str);

    byte_buffer HexString2ByteBuffer(std::string hex);

    std::string Base64Encoder(byte_buffer input);

    byte_buffer Base64Decoder(std::string input);
}