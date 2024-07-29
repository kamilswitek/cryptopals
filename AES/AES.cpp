#include "AES.hpp"
#include "AES_lut.hpp"
#include "Printer.hpp"
#include "Eigen/Dense"

byte_buffer AES::KeySchedule::RotWord(byte_buffer word)
{
    byte_buffer output = {word[1], word[2], word[3], word[0]};
    return output;
}

byte_buffer AES::KeySchedule::SubWord(byte_buffer word, AES_Mode_T aes_mode)
{
    byte_buffer result = {};

    const std::vector<std::vector<unsigned char>>* box;
    if(AES_Mode_T::ENCRYPT == aes_mode)
    {
        box = &AES_S_BOX;
    }
    else if(AES_Mode_T::DECRYPT == aes_mode)
    {
        box = &AES_INVERSE_S_BOX;
    }

    for(size_t i = 0; i < 4; i++)
    {
        result.push_back((*box)[(word[i] & BYTE_UPPER_NIBBLE) >> 4][word[i] & BYTE_LOWER_NIBBLE]);
    }

    return result;
}

byte_buffer AES::KeySchedule::GenerateKeys(byte_buffer key)
{
    Printer printer = Printer();

    const unsigned short r = 11; /* 11 round keys for AES-128 */
    const unsigned short N = 4;

    byte_buffer output(4*4*r);
    std::copy(key.begin(), key.end(), output.begin());

    for(size_t i=16; i<4*4*r; i+=4)
    {
        byte_buffer last_key_oldest_word(output.begin() + i - 4 * N, output.begin() + i - 3 * N);
        byte_buffer previous_word(output.begin() + i - 4, output.begin() + i);
        byte_buffer rcon_word = {RCON[i / (4*N)], 0x00, 0x00, 0x00};

        byte_buffer current_word = {};
        if( (i % (4*N)) == 0 )
        {
            current_word = CryptoMethods::XorBuffers(CryptoMethods::XorBuffers((last_key_oldest_word), SubWord(RotWord(previous_word), AES_Mode_T::ENCRYPT)), rcon_word);
        }
        else
        {
            current_word = CryptoMethods::XorBuffers(last_key_oldest_word, previous_word);
        }
        std::copy(current_word.begin(),current_word.end(), output.begin() + i);
    }

    return output;
}

byte_buffer AES::KeySchedule::GetKey(byte_buffer keys, size_t index)
{
    return byte_buffer(keys.begin() + 16*index, keys.begin() + 16*(index+1));
}

void AES::AddRoundKey(std::vector<byte_buffer>& state, byte_buffer key)
{
    for(size_t i=0; i<AES_STATE_MATRIX_SIZE; i++)
    {
        byte_buffer key_word = {key[i], key[4+i], key[8+i], key[12+i]};
        state[i] = CryptoMethods::XorBuffers(key_word, state[i]);
    }
}

void AES::SubBytes(std::vector<byte_buffer>& state, AES_Mode_T aes_mode)
{
    for(size_t i=0; i<AES_STATE_MATRIX_SIZE; i++)
    {
        state[i] = AES::KeySchedule::SubWord(state[i], aes_mode);
    }
}

void AES::ShiftRows(std::vector<byte_buffer>& state, AES_Mode_T aes_mode)
{
    if(aes_mode == AES_Mode_T::ENCRYPT)
    {
        state[1] = {state[1][1], state[1][2], state[1][3], state[1][0]};
        state[2] = {state[2][2], state[2][3], state[2][0], state[2][1]};
        state[3] = {state[3][3], state[3][0], state[3][1], state[3][2]};
    }
    else
    {
        state[1] = {state[1][3], state[1][0], state[1][1], state[1][2]};
        state[2] = {state[2][2], state[2][3], state[2][0], state[2][1]};
        state[3] = {state[3][1], state[3][2], state[3][3], state[3][0]};
    }
}

void AES::MixColumns(std::vector<byte_buffer>& state, AES_Mode_T aes_mode)
{
    Printer printer = Printer();

    byte_buffer result(AES_STATE_MATRIX_SIZE);

    for(size_t i=0; i<AES_STATE_MATRIX_SIZE; i++)
    {
        byte_buffer result(AES_STATE_MATRIX_SIZE);
        if(aes_mode == AES_Mode_T::ENCRYPT)
        {
            result[0] = GALOIS_LUT_MUL_2[state[0][i]]   ^ GALOIS_LUT_MUL_3[state[1][i]]     ^ state[2][i]                       ^ state[3][i];
            result[1] = state[0][i]                     ^ GALOIS_LUT_MUL_2[state[1][i]]     ^ GALOIS_LUT_MUL_3[state[2][i]]     ^ state[3][i];
            result[2] = state[0][i]                     ^ state[1][i]                       ^ GALOIS_LUT_MUL_2[state[2][i]]     ^ GALOIS_LUT_MUL_3[state[3][i]];
            result[3] = GALOIS_LUT_MUL_3[state[0][i]]   ^ state[1][i]                       ^ state[2][i]                       ^ GALOIS_LUT_MUL_2[state[3][i]];
        }
        else
        {
            result[0] = GALOIS_LUT_MUL_14[state[0][i]]  ^ GALOIS_LUT_MUL_11[state[1][i]]    ^ GALOIS_LUT_MUL_13[state[2][i]]   ^ GALOIS_LUT_MUL_9[state[3][i]];
            result[1] = GALOIS_LUT_MUL_9[state[0][i]]   ^ GALOIS_LUT_MUL_14[state[1][i]]    ^ GALOIS_LUT_MUL_11[state[2][i]]   ^ GALOIS_LUT_MUL_13[state[3][i]];
            result[2] = GALOIS_LUT_MUL_13[state[0][i]]  ^ GALOIS_LUT_MUL_9[state[1][i]]     ^ GALOIS_LUT_MUL_14[state[2][i]]   ^ GALOIS_LUT_MUL_11[state[3][i]];
            result[3] = GALOIS_LUT_MUL_11[state[0][i]]  ^ GALOIS_LUT_MUL_13[state[1][i]]    ^ GALOIS_LUT_MUL_9[state[2][i]]    ^ GALOIS_LUT_MUL_14[state[3][i]];
        }

        state[0][i] = result[0];
        state[1][i] = result[1];
        state[2][i] = result[2];
        state[3][i] = result[3];
    }
}

void AES::ByteBuffer2State(std::vector<byte_buffer>& state, byte_buffer chunk)
{
    for(size_t i=0; i<AES_BLOCK_SIZE_B; i++)
    {
        state[i%AES_STATE_MATRIX_SIZE][i/AES_STATE_MATRIX_SIZE] = chunk[i];
    }
}

byte_buffer AES::State2ByteBuffer(std::vector<byte_buffer>& state)
{
    byte_buffer output{};
    for(size_t i=0; i<AES_BLOCK_SIZE_B; i++)
    {
        output.push_back(state[i%AES_STATE_MATRIX_SIZE][i/AES_STATE_MATRIX_SIZE]);
    }
    return output;
}

void AES::PKCS7Padding(byte_buffer& block, size_t block_length)
{
    unsigned char new_byte = block_length - block.size();

    while(block.size() < block_length)
    {
        block.push_back(new_byte);
    }
}

byte_buffer AES::Encrypt(byte_buffer input, byte_buffer key, AES_BlockCipherMode_T block_cipher_mode, const std::optional<byte_buffer>& iv)
{
    byte_buffer scheduled_keys = AES::KeySchedule::GenerateKeys(key);

    byte_buffer encrypted_buffer{};

    try
    {
        if( (block_cipher_mode == AES_BlockCipherMode_T::CBC) && (iv == std::nullopt) )
        {
            throw(block_cipher_mode);
        }
    }
    catch(AES_BlockCipherMode_T mode)
    {
        
    }
    
    auto current_iv = std::move(*iv);

    for(int i=0; i<input.size(); i+=AES_BLOCK_SIZE_B)
    {
        byte_buffer current_chunk(input.begin() + i, input.begin() + i + AES_BLOCK_SIZE_B);

        current_chunk = CryptoMethods::XorBuffers(current_chunk, current_iv);
        
        std::vector<byte_buffer> state(AES_STATE_MATRIX_SIZE, byte_buffer(AES_STATE_MATRIX_SIZE));
        AES::ByteBuffer2State(state, current_chunk);

        for(int round=0; round<=AES_NUM_ROUNDS; round++)
        {
            byte_buffer round_key = AES::KeySchedule::GetKey(scheduled_keys, round);

            if(round == 0)
            {
                AES::AddRoundKey(state, round_key);
            }
            else if(round>0 && round<AES_NUM_ROUNDS)
            {
                AES::SubBytes(state, AES_Mode_T::ENCRYPT);
                AES::ShiftRows(state, AES_Mode_T::ENCRYPT);
                AES::MixColumns(state, AES_Mode_T::ENCRYPT);
                AES::AddRoundKey(state, round_key);
            }
            else
            {
                //round == final
                AES::SubBytes(state, AES_Mode_T::ENCRYPT);
                AES::ShiftRows(state, AES_Mode_T::ENCRYPT);
                AES::AddRoundKey(state, round_key);
            }
        }
        byte_buffer output_chunk = AES::State2ByteBuffer(state);

        current_iv = output_chunk;

        encrypted_buffer.insert(encrypted_buffer.end(), output_chunk.begin(), output_chunk.end());
    }

    return encrypted_buffer;
}

byte_buffer AES::Decrypt(byte_buffer input, byte_buffer key, AES_BlockCipherMode_T block_cipher_mode, const std::optional<byte_buffer>& iv)
{
    byte_buffer scheduled_keys = AES::KeySchedule::GenerateKeys(key);

    byte_buffer decrypted_buffer{};

    try
    {
        if( (block_cipher_mode == AES_BlockCipherMode_T::CBC) && (iv == std::nullopt) )
        {
            throw(block_cipher_mode);
        }
    }
    catch(AES_BlockCipherMode_T mode)
    {
        return {};
    }
    
    auto current_iv = std::move(*iv);

    for(int i=0; i<input.size(); i+=AES_BLOCK_SIZE_B)
    {
        byte_buffer current_chunk(input.begin() + i, input.begin() + i + AES_BLOCK_SIZE_B);
        
        std::vector<byte_buffer> state(AES_STATE_MATRIX_SIZE, byte_buffer(AES_STATE_MATRIX_SIZE));
        AES::ByteBuffer2State(state, current_chunk);

        for(int round=AES_NUM_ROUNDS; round>=0; round--)
        {
            byte_buffer round_key = AES::KeySchedule::GetKey(scheduled_keys, round);

            if(round == AES_NUM_ROUNDS)
            {
                AES::AddRoundKey(state, round_key);
            }
            else if(round<AES_NUM_ROUNDS && round>0)
            {
                AES::ShiftRows(state, AES_Mode_T::DECRYPT);
                AES::SubBytes(state, AES_Mode_T::DECRYPT);
                AES::AddRoundKey(state, round_key);
                AES::MixColumns(state, AES_Mode_T::DECRYPT);
            }
            else
            {
                //round == 0
                AES::ShiftRows(state, AES_Mode_T::DECRYPT);
                AES::SubBytes(state, AES_Mode_T::DECRYPT);
                AES::AddRoundKey(state, round_key);
            }
        }
        byte_buffer output_chunk = AES::State2ByteBuffer(state);

        output_chunk = CryptoMethods::XorBuffers(output_chunk, current_iv);

        current_iv = current_chunk;

        decrypted_buffer.insert(decrypted_buffer.end(), output_chunk.begin(), output_chunk.end());
    }

    return decrypted_buffer;
}

