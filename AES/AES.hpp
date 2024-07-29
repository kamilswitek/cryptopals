#pragma once
#include "Common.hpp"
#include "CryptoMethods.hpp"
#include <optional>

#define AES_BLOCK_SIZE_B        16u
#define AES_STATE_MATRIX_SIZE   4

#define AES_KEY_SIZE_BIT        128u

#if AES_KEY_SIZE_BIT == 128u
    #define AES_NUM_ROUNDS  10u
#elif AES_KEY_SIZE_BIT == 192u
    #define AES_NUM_ROUNDS  12u
#elif AES_KEY_SIZE_bIT == 256u
    #define AES_NUM_ROUNDS  14u
#endif


#define BYTE_LOWER_NIBBLE   0x0F
#define BYTE_UPPER_NIBBLE   0xF0
#define WORD_MASK_BYTE_0    0xFF
#define WORD_MASK_BYTE_1    0xFF00
#define WORD_MASK_BYTE_2    0xFF0000
#define WORD_MASK_BYTE_3    0xFF000000

enum AES_BlockCipherMode_T
{
    ECB,
    CBC
};

enum AES_Mode_T
{
    ENCRYPT,
    DECRYPT
};

enum AES_S_Box_Type_T
{
    S_BOX,
    INVERSE_S_BOX,
};

enum AES_Type_T
{
    AES_128,
    AES_192,
    AES_256
};

namespace AES
{
    namespace KeySchedule
    {
        byte_buffer RotWord(byte_buffer word);
        byte_buffer SubWord(byte_buffer word, AES_Mode_T s_box_type);
        byte_buffer GenerateKeys(byte_buffer key);
        byte_buffer GetKey(byte_buffer keys, size_t index);
    }
    byte_buffer Encrypt(byte_buffer input, byte_buffer key, AES_BlockCipherMode_T block_cipher_mode, const std::optional<byte_buffer>& iv = std::nullopt);
    byte_buffer Decrypt(byte_buffer input, byte_buffer key, AES_BlockCipherMode_T block_cipher_mode, const std::optional<byte_buffer>& iv = std::nullopt);

    void ByteBuffer2State(std::vector<byte_buffer>& state, byte_buffer chunk);
    byte_buffer State2ByteBuffer(std::vector<byte_buffer>& state);
    void AddRoundKey(std::vector<byte_buffer>& state, byte_buffer key);
    void SubBytes(std::vector<byte_buffer>& state, AES_Mode_T aes_mode);
    void ShiftRows(std::vector<byte_buffer>& state, AES_Mode_T aes_mode);
    void MixColumns(std::vector<byte_buffer>& state, AES_Mode_T aes_mode);
    void PKCS7Padding(byte_buffer& block, size_t block_length);
}
