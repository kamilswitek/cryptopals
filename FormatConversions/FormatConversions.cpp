#include "FormatConversions.hpp"

#define BASE64_TOKEN_NUM_CHARS  6u

static const std::string base64_chars =
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";

#define BASE64_FIRST_SEXTET_MASK 0xFC0000
#define BASE64_CHUNK_SIZE        24u
#define BYTE_SIZE                8u

byte_buffer FormatConversions::HexString2ByteBuffer(std::string hex)
{
    byte_buffer output{};

    auto a = hex.begin();
    while(a < hex.end())
    {
        std::string byte(a, a + 2);
        output.push_back(std::stol(byte, 0, 16));
        a += 2;
    }

    return output;
}

byte_buffer FormatConversions::CharString2ByteBuffer(std::string str)
{
    byte_buffer output{};
    for(char c : str)
    {
        output.push_back(static_cast<unsigned short>(c));
    }
    return output;
}

std::string FormatConversions::Base64Encoder(byte_buffer input)
{
    std::string base64_output{};

    unsigned long index = 0;
    while(index < input.size())
    {
        /* Take one, max. 3-byte chunk of data */
        byte_buffer byte_chunk = {input.begin() + index, min(input.end(), input.begin() + index + 3)};
        index += 3;

        /* Form a number from it */
        unsigned long token = 0;
        for(unsigned int i=0; i<byte_chunk.size(); i++)
        {
            token |= (byte_chunk[i] << (16 - 8 * i));
        }

        /* Convert this 24-byte number to 4 base64 characters */
        unsigned long mask = BASE64_FIRST_SEXTET_MASK;
        unsigned short num_added_bytes = 0;
        while( (mask != 0) && (token != 0) )
        {
            /* Take first sextet, then second, then third ... */
            base64_output.push_back( base64_chars[(token & mask) >> (18 - 6 * num_added_bytes)] );
            token &= ~mask;
            mask >>= 6;
            num_added_bytes++;
        }

        /*Add terminating '='s if missing */
        while(num_added_bytes < 4)
        {
            base64_output.push_back('=');
            num_added_bytes++;
        }
    }
    return base64_output;
}

inline unsigned char FindBase64Coding(char input)
{
    unsigned char value = 0;
    for(int i=0; i<base64_chars.length(); i++)
    {
        if(input == base64_chars[i])
        {
            value = i;
            break;
        }
    }
    return value;
}

byte_buffer FormatConversions::Base64Decoder(std::string input)
{
    byte_buffer output{};

    auto it = input.begin();
    while(it != input.end())
    {
        uint32_t bits = 0;
        uint8_t shift = 0;
        uint8_t num_terminators = 0;
        std::string current_string = std::string(it, it + 4);
        it += 4;

        for(char c : current_string)
        {
            bits |= ( FindBase64Coding(c) << 18 - 6 * shift );
            shift++;
            if(c == '=')
            {
                num_terminators++;
            }
        }

        for(int i=0; i<3-num_terminators; i++)
        {
            output.push_back(bits >> (16 - 8 * i) & 0xFF);
        }
    }
    return output;
}