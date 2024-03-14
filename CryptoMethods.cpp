#include "Common.hpp"
#include "CryptoMethods.hpp"
#include "Metrics.hpp"
#include <iostream>

byte_buffer CryptoMethods::XorBuffers(byte_buffer buffer_1, byte_buffer buffer_2)
{
    byte_buffer output_buffer{};
    if(buffer_1.size() == buffer_2.size())
    {
        for(size_t i=0; i<buffer_1.size(); i++)
        {
            uint8_t result = buffer_1[i] ^ buffer_2[i];
            output_buffer.push_back(result);
        }
    }
    return output_buffer;
}

byte_buffer CryptoMethods::SequentialXor_EncryptBuffer(byte_buffer in_buffer, byte_buffer key)
{
    size_t key_max_idx {key.size()};
    int key_idx {0};
    byte_buffer out_buffer{};

    for(unsigned short x : in_buffer)
    {
        out_buffer.push_back(x ^ key[key_idx % key_max_idx]);
        key_idx++;
    }

    return out_buffer;
}

unsigned int CryptoMethods::SequentialXor_FindPossibleKeySize(byte_buffer input)
{
    unsigned int possible_key_size = 0;
    double lowest_distance = UINT32_MAX;
    byte_buffer possible_keys{};

    for(unsigned int key_size = 2; key_size<40; key_size++)
    {
        unsigned int i = 0;
        unsigned int num_samples = 0;
        double distance = 0;

        while(i + 2*key_size < input.size())
        {
            byte_buffer first_sequence(input.begin() + i, input.begin() + i + key_size);
            byte_buffer second_sequence(input.begin() + i + key_size, input.begin() + i + 2 * key_size);

            distance += Metrics::HammingDistance(first_sequence, second_sequence);
            i += key_size;
            num_samples++;
        }

        if(num_samples)
        {
            distance /= num_samples;
            distance /= key_size;

            if(distance < lowest_distance)
            {
                lowest_distance = distance;
                possible_key_size = key_size;
            }
        }

    }
    return possible_key_size;
}

void CryptoMethods::SequentialXor_BreakBufferToChunks(byte_buffer& input, std::vector<byte_buffer>& output, unsigned int chunk_length)
{
    unsigned int i = 0;
    while(i < input.size())
    {
        byte_buffer chunk(input.begin() + i, input.begin() + std::min(static_cast<size_t>(i) + chunk_length, input.size()));
        output.push_back(chunk);
        i += chunk_length;
    }
}

void CryptoMethods::SequentialXor_TransposeBuffer(std::vector<byte_buffer>& input, std::vector<byte_buffer>& output)
{
    for(int i=0; i<input[0].size(); i++)
    {
        byte_buffer transposed_buffer{};
        for(int j=0; j<input.size(); j++)
        {
            if(i < input[j].size())
            {
                transposed_buffer.push_back(input[j][i]);
            }
        }
        output.push_back(transposed_buffer);
    }
}

byte_buffer CryptoMethods::SingleByteXor_DecryptBuffer(byte_buffer in_buffer, unsigned char& key, double& score)
{
    double best_score = 0;
    unsigned short best_char = 0;
    byte_buffer result_buffer{};
    
    /*Look within printable characters */
    for(unsigned short coding_char = 32; coding_char<INT8_MAX; coding_char++)
    {
        byte_buffer decrypted_buffer{};

        for(size_t i=0; i<in_buffer.size(); i++)
        {
            unsigned short result = in_buffer[i] ^ coding_char;

            /* Non-ASCII character - skip the currently analysed char */
            if(result > INT8_MAX)
            {
                decrypted_buffer.clear();
                break;
            }
            decrypted_buffer.push_back(result);
        }

        double char_score = Metrics::LetterFrequency(decrypted_buffer);

        if(char_score > best_score)
        {
            best_score = char_score;
            best_char = coding_char;
            result_buffer = decrypted_buffer;
        }
    }

    key = best_char;
    score = best_score;

    return result_buffer;
}
