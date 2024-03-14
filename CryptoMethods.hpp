#include "Common.hpp"

#define BASE64_TOKEN_NUM_CHARS  6u

static const std::string base64_chars =
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";

namespace CryptoMethods
{
    byte_buffer XorBuffers(byte_buffer buffer_1, byte_buffer buffer_2);

    byte_buffer SequentialXor_EncryptBuffer(byte_buffer in_buffer, byte_buffer key);

    unsigned int SequentialXor_FindPossibleKeySize(byte_buffer input);

    void SequentialXor_BreakBufferToChunks(byte_buffer& input, std::vector<byte_buffer>& output, unsigned int chunk_length);

    void SequentialXor_TransposeBuffer(std::vector<byte_buffer>& input, std::vector<byte_buffer>& output);
    
    byte_buffer SingleByteXor_DecryptBuffer(byte_buffer in_buffer, unsigned char& key, double& score);
}