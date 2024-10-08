#include "Solutions.hpp"
#include "Metrics.hpp"
#include <numeric>
#include <fstream>

void Solutions::Challenge1()
{
    std::string input = "49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d";

    byte_buffer data = FormatConversions::HexString2ByteBuffer(input);

    std::string output = FormatConversions::Base64Encoder(data);

    Printer::WriteIoStream(output);
}

void Solutions::Challenge2()
{
    std::string in1 = "1c0111001f010100061a024b53535009181c";
    std::string in2 = "686974207468652062756c6c277320657965";

    byte_buffer input1 = FormatConversions::HexString2ByteBuffer(in1);
    byte_buffer input2 = FormatConversions::HexString2ByteBuffer(in2);

    byte_buffer output = CryptoMethods::XorBuffers(input1, input2);
    
    Printer::WriteIoStream(output, PrintOutputType_T::HEX);
}

void Solutions::Challenge3()
{
    std::string hex_string = "1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736";

    byte_buffer bytes = FormatConversions::HexString2ByteBuffer(hex_string);

    unsigned char key = 0;
    double score = 0;
    byte_buffer result = CryptoMethods::SingleByteXor_DecryptBuffer(bytes, key, score);

    Printer::WriteIoStream(result, PrintOutputType_T::CHAR);
}

void Solutions::Challenge4()
{
    std::ifstream input_file;
    input_file.open("../inputs/4.txt");

    double best_score = 0;
    unsigned char best_key = 0;
    byte_buffer best_buffer = {};

    size_t line_index = 0;
    std::string line = {};
    while(getline(input_file, line))
    {
        double line_result_score = 0;
        unsigned char line_result_key = 0;

        byte_buffer buffer = FormatConversions::HexString2ByteBuffer(line);

        byte_buffer output_buffer = CryptoMethods::SingleByteXor_DecryptBuffer(buffer, line_result_key, line_result_score);

        if(line_result_score > best_score)
        {
            best_score = line_result_score;
            best_key = line_result_key;
            best_buffer = output_buffer;
        }
    }

    std::cout << "Scores: " << static_cast<short>(best_key) << " " << best_score << std::endl;
    
    Printer::WriteIoStream(best_buffer, PrintOutputType_T::CHAR);

    input_file.close();
}

void Solutions::Challenge5()
{
    const byte_buffer input_buffer = FormatConversions::CharString2ByteBuffer("Burning 'em, if you ain't quick and nimble\nI go crazy when I hear a cymbal");
    const byte_buffer xor_key = FormatConversions::CharString2ByteBuffer("ICE");

    byte_buffer result = CryptoMethods::SequentialXor_EncryptBuffer(input_buffer, xor_key);

    Printer::WriteIoStream(result, PrintOutputType_T::HEX);
}

void Solutions::Challenge6()
{
    std::ifstream input_file;
    input_file.open("../inputs/6.txt");

    byte_buffer input_buffer;

    std::string line;
    while(getline(input_file, line))
    {
        byte_buffer encoded_line = FormatConversions::Base64Decoder(line);
        input_buffer.insert(input_buffer.end(), encoded_line.begin(), encoded_line.end());
    }

    short key_size = CryptoMethods::SequentialXor_FindPossibleKeySize(input_buffer);

    std::vector<byte_buffer> chunked_buffer{};
    CryptoMethods::SequentialXor_BreakBufferToChunks(input_buffer, chunked_buffer, key_size);

    std::vector<byte_buffer> transposed_chunked_buffer{};
    CryptoMethods::SequentialXor_TransposeBuffer(chunked_buffer, transposed_chunked_buffer);

    byte_buffer key{};
    for(byte_buffer buffer : transposed_chunked_buffer)
    {
        unsigned char key_char = 0;
        double score = 0;
        CryptoMethods::SingleByteXor_DecryptBuffer(buffer, key_char, score);
        key.push_back(key_char);
    }

    Printer::WriteIoStream(key, PrintOutputType_T::CHAR);

    byte_buffer output = CryptoMethods::SequentialXor_EncryptBuffer(input_buffer, key);

    Printer::WriteIoStream(output, PrintOutputType_T::CHAR);
}

void Solutions::Challenge7()
{
    /* 1. Get input key & input buffer */
    std::string input_key = "YELLOW SUBMARINE";
    byte_buffer input = {};

    std::ifstream input_file;
    input_file.open("../inputs/7.txt");

    std::string line;
    while(getline(input_file, line))
    {
        byte_buffer line_buffer = FormatConversions::Base64Decoder(line);
        input.insert(input.end(), line_buffer.begin(), line_buffer.end());
    }

    byte_buffer decrypted_buffer = AES::Decrypt(input, FormatConversions::CharString2ByteBuffer(input_key), AES_BlockCipherMode_T::ECB);

    Printer::WriteIoStream(decrypted_buffer, PrintOutputType_T::CHAR);
}

void Solutions::Challenge8()
{
    std::ifstream input_file;
    input_file.open("../inputs/8.txt");

    std::string line;
    while(getline(input_file, line))
    {
        byte_buffer line_buffer = FormatConversions::HexString2ByteBuffer(line);

        for(size_t i=0; i<line_buffer.size(); i+=16)
        {
            bool duplicate_found = false;

            byte_buffer chunk(line_buffer.begin() + i, line_buffer.begin() + i + 16);

            for(size_t j=i+16; j<line_buffer.size(); j+=16)
            {
                byte_buffer chunk_to_compare(line_buffer.begin() + j, line_buffer.begin() + j + 16);

                byte_buffer result = CryptoMethods::XorBuffers(chunk, chunk_to_compare);

                if(std::accumulate(result.begin(), result.end(), 0) == 0)
                {
                    std::cout << "Possible ECB-encoded line: " << std::endl;
                    std::cout << line << std::endl;
                    std::cout << "Repeated chunk: " << std::endl;
                    Printer::WriteIoStream(chunk, PrintOutputType_T::HEX);
                    duplicate_found = true;
                    break;
                }
            }
            if(duplicate_found == true)
            {
                break;
            }
        }
    }
}

void Solutions::Challenge9()
{
    byte_buffer input = FormatConversions::CharString2ByteBuffer("YELLOW SUBMARINE");
    
    AES::PKCS7Padding(input, 20);

    Printer::WriteIoStream(input, HEX);
}

void Solutions::Challenge10()
{
    byte_buffer input;
    std::string input_key = "YELLOW SUBMARINE";
    byte_buffer iv(AES_BLOCK_SIZE_B, 0);

    std::ifstream input_file;
    input_file.open("../inputs/10.txt");

    std::string line;
    while(getline(input_file, line))
    {
        byte_buffer line_buffer = FormatConversions::Base64Decoder(line);
        input.insert(input.end(), line_buffer.begin(), line_buffer.end());
    }

    byte_buffer decrypted_buffer = AES::Decrypt(input, FormatConversions::CharString2ByteBuffer(input_key), AES_BlockCipherMode_T::CBC, iv);

    Printer::WriteIoStream(decrypted_buffer, PrintOutputType_T::CHAR);
}

byte_buffer AES_Encryption_Oracle(byte_buffer input, AES_BlockCipherMode_T& mode)
{
    byte_buffer output;
    /* 1. Choose encryption method - AES or CBC */
    int encryption_method = std::rand() % 2;

    /* 2. Insert some random character at the front and back */
    auto num_bytes_to_emplace = 5 + rand() % 5;
    for(int i=0; i<num_bytes_to_emplace; i++)
    {
        char random_char = rand() % UINT8_MAX;
        input.emplace(input.begin(), random_char);
    }

    num_bytes_to_emplace = AES_BLOCK_SIZE_B - num_bytes_to_emplace;
    for(int i=0; i<num_bytes_to_emplace; i++)
    {
        char random_char = rand() % UINT8_MAX;
        input.emplace_back(random_char);
    }

    /* 3. Generate a random key (and random IV) */
    byte_buffer key(AES_BLOCK_SIZE_B);
    byte_buffer iv(AES_BLOCK_SIZE_B);
    for(int i=0; i<AES_BLOCK_SIZE_B; i++)
    {
        key[i] = rand() % UINT8_MAX;
        iv[i] = rand() % UINT8_MAX;
    }

    if (static_cast<AES_BlockCipherMode_T>(encryption_method) == AES_BlockCipherMode_T::ECB)
    {
        std::cout << "Encrypting with ECB" << std::endl;
        output = AES::Encrypt(input, key, AES_BlockCipherMode_T::ECB);
        mode = AES_BlockCipherMode_T::ECB;
    }
    else
    {
        std::cout << "Encrypting with CBC" << std::endl;
        output = AES::Encrypt(input, key, AES_BlockCipherMode_T::CBC, iv);
        mode = AES_BlockCipherMode_T::CBC;
    }

    return output;
}

bool DetectPattern(const byte_buffer& input)
{
    bool pattern_found = false;

    const byte_buffer compare(AES_BLOCK_SIZE_B, 0);

    for(int i=0; i<input.size() - AES_BLOCK_SIZE_B; i+=AES_BLOCK_SIZE_B)
    {
        byte_buffer b1(input.begin() + i, input.begin() + i + AES_BLOCK_SIZE_B);
        byte_buffer b2(input.begin() + i + AES_BLOCK_SIZE_B, input.begin() + i + 2 * AES_BLOCK_SIZE_B);
        byte_buffer result = CryptoMethods::XorBuffers(b1,b2);
        if(result == compare)
        {
            pattern_found = true;
            break;
        }
    }
    return pattern_found;
}

void Solutions::Challenge11()
{
    byte_buffer input;
    byte_buffer encrypted_input;

    std::ifstream input_file;
    input_file.open("../inputs/11.txt");

    std::string line;
    while(getline(input_file, line))
    {
        byte_buffer line_buffer = FormatConversions::CharString2ByteBuffer(line);
        input.insert(input.end(), line_buffer.begin(), line_buffer.end());
    }

    const int num_trials = 400;
    auto wrong_answers_counter = 0;

    for(int i=0; i<num_trials; i++)
    {
        AES_BlockCipherMode_T mode;
        encrypted_input = AES_Encryption_Oracle(input, mode);
        if( (AES_BlockCipherMode_T::ECB == mode) && (true == DetectPattern(encrypted_input)) )
        {
            std::cout << "Correct! ECB" << std::endl;
        }
        else if( (AES_BlockCipherMode_T::CBC == mode) && (false == DetectPattern(encrypted_input)) )
        {
            std::cout << "Correct! CBC" << std::endl;
        }
        else
        {
            std::cout << "Booooh!" << std::endl;
            wrong_answers_counter++;
        }
    }
    std::cout << "I was wrong " << wrong_answers_counter << " times." << std::endl;
}

byte_buffer Oracle12(byte_buffer input)
{
    const std::string text_to_decode = "Um9sbGluJyBpbiBteSA1LjAKV2l0aCBteSByYWctdG9wIGRvd24gc28gbXkgaGFpciBjYW4gYmxvdwpUaGUgZ2lybGllcyBvbiBzdGFuZGJ5IHdhdmluZyBqdXN0IHRvIHNheSBoaQpEaWQgeW91IHN0b3A/IE5vLCBJIGp1c3QgZHJvdmUgYnkK";

    byte_buffer bytes_to_append = FormatConversions::Base64Decoder(text_to_decode);

    byte_buffer output;

    /* 3. Generate a random key */
    static byte_buffer key(AES_BLOCK_SIZE_B);
    static bool key_initialized = false;

    if(!key_initialized)
    {
        srand(time(NULL));
        for(int i=0; i<AES_BLOCK_SIZE_B; i++)
        {
            key[i] = rand() % UINT8_MAX;
        }
        key_initialized = true;
    }

    input.insert(input.end(), bytes_to_append.begin(), bytes_to_append.end());

    output = AES::Encrypt(input, key, AES_BlockCipherMode_T::ECB);

    return output;
}

void Solutions::Challenge12()
{
    const unsigned int num_ascii_chars = INT8_MAX;
    std::vector<byte_buffer> possible_outputs;
    possible_outputs.reserve(num_ascii_chars);

    /* Find the size of our buffer to *crack* */
    unsigned int data_size = Oracle12(byte_buffer(0)).size();

    /* calculate possible outputs of ECB */
    
    byte_buffer prefix(AES_BLOCK_SIZE_B - 1);
    std::fill(prefix.begin(), prefix.end(), (unsigned char)('A'));
    byte_buffer matcher = prefix;
    byte_buffer decoded_data;

    for(unsigned int current_letter_idx=0; current_letter_idx<data_size; current_letter_idx++)
    {
        prefix.resize(AES_BLOCK_SIZE_B - 1 - current_letter_idx % AES_BLOCK_SIZE_B);
        byte_buffer output = Oracle12(prefix);

        for(unsigned int i=0; i<num_ascii_chars; i++)
        {
            byte_buffer input = matcher;
            input.push_back(i);
            possible_outputs[i] = Oracle12(input);
        }

        for(unsigned int ascii_to_find=0; ascii_to_find<num_ascii_chars; ascii_to_find++)
        {
            byte_buffer test_chunk(possible_outputs[ascii_to_find].begin(), possible_outputs[ascii_to_find].begin() + AES_BLOCK_SIZE_B);
            byte_buffer output_chunk(output.begin() + AES_BLOCK_SIZE_B * (current_letter_idx / AES_BLOCK_SIZE_B), output.begin() + AES_BLOCK_SIZE_B * (1 + current_letter_idx / AES_BLOCK_SIZE_B));

            if(test_chunk == output_chunk)
            {
                decoded_data.push_back((char)ascii_to_find);

                for(auto matcher_idx = 0; matcher_idx < AES_BLOCK_SIZE_B - 1; matcher_idx++)
                {
                    std::swap(matcher[matcher_idx], matcher[matcher_idx+1]);
                }
                matcher[AES_BLOCK_SIZE_B - 2] = ascii_to_find;
            }
        }
    }
    Printer::WriteIoStream(decoded_data, PrintOutputType_T::CHAR);
}

typedef std::pair<std::string, std::string> str_pair;

std::vector<str_pair> ProfileParser(std::string encoded_profile)
{
    std::vector<str_pair> profile_structure;

    std::string key;
    std::string value;

    bool key_propagated = true;

    for(char& c : encoded_profile)
    {
        if(c == '=')
        {
            key_propagated = false;
        }
        else if(c == '&')
        {
            key_propagated = true;
            profile_structure.push_back(str_pair(key,value));
            key.clear();
            value.clear();
        }
        else if((c > AES_BLOCK_SIZE_B) && (c < INT8_MAX))
        {
            if(key_propagated)
            {
                key.push_back(c);   
            }
            else
            {
                value.push_back(c);
            }
        }
    }

    profile_structure.push_back(str_pair(key,value));

    return profile_structure;
}

std::string Profile_for(std::string email)
{
    for(auto it = email.begin(); it != email.end(); it++)
    {
        if((*it == '&') || (*it == '='))
        {
            email.replace(it, it + 1, "");
        }
    }

    std::string decoded_profile;

    std::vector<str_pair> user_structure;
    
    user_structure.emplace_back("email", email + '&');
    user_structure.emplace_back("uid", "10" + '&');
    user_structure.emplace_back("role", "user");

    for(auto entry : user_structure)
    {
        decoded_profile.append(entry.first);
        decoded_profile.append("=");
        decoded_profile.append(entry.second);
    }

    return decoded_profile;
}


byte_buffer Encrypt_Profile(std::string& encoded_profile, byte_buffer key)
{
    byte_buffer profile = FormatConversions::CharString2ByteBuffer(encoded_profile);

    return AES::Encrypt(profile, key, AES_BlockCipherMode_T::ECB);
}

std::string Decrypt_Profile(byte_buffer& profile, byte_buffer key)
{
    byte_buffer result = AES::Decrypt(profile, key, AES_BlockCipherMode_T::ECB);

    std::string decoded_string = FormatConversions::ByteBuffer2CharString(result);

    return decoded_string;
}

void Solutions::Challenge13()
{
    byte_buffer key = FormatConversions::CharString2ByteBuffer("QWERTYUIASDFGHJK");

    /* First let's find an 'email=swift@onet' encrypted buffer */
    std::string crack_profile_1 = Profile_for("swift@onet");
    byte_buffer encrypted_profile_1 = Encrypt_Profile(crack_profile_1, key);

    std::cout << "First output: " << std::endl;
    Printer::WriteIoStream(encrypted_profile_1, PrintOutputType_T::HEX);

    const std::string first_buffer = "3f80a20d39a635d6957249f5ef2874be"; //email=swift@onet

    /* Then the second buffer would be '.pl&uid=10&role=' */

    std::string crack_profile_2 = Profile_for("abcdefghij.pl");
    byte_buffer encrypted_profile_2 = Encrypt_Profile(crack_profile_2, key);

    std::cout << "Second output: " << std::endl;
    Printer::WriteIoStream(encrypted_profile_2, PrintOutputType_T::HEX);

    const std::string second_buffer = "11fb0b656e022457898ebf9f5679793a"; // .pl&uid=10&role=

    /* And a third one, admin is at the beginning of block, so we need to PKCS7-pad it with 11 bytes of value \11 */

    std::string crack_profile_3 = Profile_for("7890123456admin\11\11\11\11\11\11\11\11\11\11\11");
    byte_buffer encrypted_profile_3 = Encrypt_Profile(crack_profile_3, key);

    std::cout << "Third output: " << std::endl;
    Printer::WriteIoStream(encrypted_profile_3, PrintOutputType_T::HEX);

    const std::string third_buffer = "902a0c1fb5d1c5e4f5a80d7e02338e47"; //admin

    byte_buffer attack_profile = FormatConversions::HexString2ByteBuffer(first_buffer + second_buffer + third_buffer);
    std::string decrypted_profile = Decrypt_Profile(attack_profile, key);

    std::vector<str_pair> decoded_profile = ProfileParser(decrypted_profile);
    for(auto p : decoded_profile)
    {
        std::cout << p.first << " " << p.second << std::endl;
    }

}

byte_buffer Oracle14(const byte_buffer& input)
{
    byte_buffer data = input;

    #define BYTES_TO_APPEND 30u
    static bool module_initialized = false;
    
    static byte_buffer random_prefix;
    random_prefix.reserve(BYTES_TO_APPEND);
    static byte_buffer key(AES_BLOCK_SIZE_B);

    if(!module_initialized)
    {
        srand(time(NULL));

        unsigned int random_bytes_count = rand() % BYTES_TO_APPEND;

        for(unsigned int i=0; i<random_bytes_count; i++)
        {
            random_prefix.push_back(rand() % UINT8_MAX);
        }

        for(int i=0; i<AES_BLOCK_SIZE_B; i++)
        {
            key[i] = rand() % UINT8_MAX;
        }

        module_initialized = true;
    }

    const std::string text_to_decode = "Um9sbGluJyBpbiBteSA1LjAKV2l0aCBteSByYWctdG9wIGRvd24gc28gbXkgaGFpciBjYW4gYmxvdwpUaGUgZ2lybGllcyBvbiBzdGFuZGJ5IHdhdmluZyBqdXN0IHRvIHNheSBoaQpEaWQgeW91IHN0b3A/IE5vLCBJIGp1c3QgZHJvdmUgYnkK";

    byte_buffer bytes_to_append = FormatConversions::Base64Decoder(text_to_decode);

    byte_buffer output;

    /* 3. Generate a random key */


    data.insert(data.begin(), random_prefix.begin(), random_prefix.end());

    data.insert(data.end(), bytes_to_append.begin(), bytes_to_append.end());

    output = AES::Encrypt(data, key, AES_BlockCipherMode_T::ECB);

    return output;
}

void Solutions::Challenge14()
{
    /* First let's provide some (X * AES_BLOCK_SIZE_B) long string as a buffer between random-appended bytes and our data to decrypt */

    std::string a_string = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";

    byte_buffer input_findpadding;
    byte_buffer output_findpadding;

    unsigned int num_identical_blocks = 0;
    unsigned int num_identical_blocks_prev = 0;

    while(!a_string.empty())
    {
        num_identical_blocks = 0;
        input_findpadding = FormatConversions::CharString2ByteBuffer(a_string);
        output_findpadding = Oracle14(input_findpadding);

        for(unsigned int i=0; i<output_findpadding.size() - AES_BLOCK_SIZE_B; i+=AES_BLOCK_SIZE_B)
        {
            /* Check for identical adjacent output blocks */
            byte_buffer current_block(output_findpadding.begin() + i, output_findpadding.begin() + i + AES_BLOCK_SIZE_B);
            byte_buffer next_block(output_findpadding.begin() + i + AES_BLOCK_SIZE_B, output_findpadding.begin() + i + 2*AES_BLOCK_SIZE_B);

            if(current_block == next_block)
            {
                num_identical_blocks++;
            }
        }
        if( (num_identical_blocks_prev != num_identical_blocks) && (num_identical_blocks_prev != 0) )
        {
            /* If there are less identical chunks than before popping back one 'A', it means that one of our A's got mixed with data we want to extract */
            break;
        }

        a_string.pop_back();

        num_identical_blocks_prev = num_identical_blocks;
    }

    /* That's our buffer that makes the data we want to extract AES_BLOCK_SIZE_B-aligned */
    a_string.push_back('A');
    byte_buffer a_buffer = FormatConversions::CharString2ByteBuffer(a_string);

    byte_buffer output_findstartinglength = Oracle14(a_buffer);

    unsigned int start_index = 0;

    for(unsigned int i=0; i<output_findstartinglength.size() - AES_BLOCK_SIZE_B; i+=AES_BLOCK_SIZE_B)
    {
        byte_buffer current_block(output_findstartinglength.begin() + i, output_findstartinglength.begin() + i + AES_BLOCK_SIZE_B);
        byte_buffer next_block(output_findstartinglength.begin() + i + AES_BLOCK_SIZE_B, output_findstartinglength.begin() + i + 2*AES_BLOCK_SIZE_B);

        if(current_block == next_block)
        {
            start_index = i + 2 * AES_BLOCK_SIZE_B;
        }
    }

    unsigned int data_to_decode_size = output_findstartinglength.size() - start_index;

    const unsigned int num_ascii_chars = INT8_MAX;
    std::vector<byte_buffer> possible_outputs;
    possible_outputs.reserve(num_ascii_chars);

    byte_buffer prefix(AES_BLOCK_SIZE_B - 1);
    std::fill(prefix.begin(), prefix.end(), (unsigned char)('A'));
    byte_buffer matcher = prefix;
    byte_buffer decoded_data;


    for(unsigned int current_letter_idx=0; current_letter_idx<data_to_decode_size; current_letter_idx++)
    {
        byte_buffer input_buffer = a_buffer;

        prefix.resize(AES_BLOCK_SIZE_B - 1 - current_letter_idx % AES_BLOCK_SIZE_B);

        input_buffer.insert(input_buffer.end(), prefix.begin(), prefix.end());

        byte_buffer current_output = Oracle14(input_buffer);

        for(unsigned int i=0; i<num_ascii_chars; i++)
        {
            byte_buffer test_input = a_buffer;
            test_input.insert(test_input.end(), matcher.begin(), matcher.end());
            test_input.push_back(i);
            possible_outputs[i] = Oracle14(test_input);
        }

        for(unsigned int ascii_to_find=0; ascii_to_find<num_ascii_chars; ascii_to_find++)
        {
            byte_buffer test_block(possible_outputs[ascii_to_find].begin() + start_index, possible_outputs[ascii_to_find].begin() + start_index + AES_BLOCK_SIZE_B);
            byte_buffer output_block(current_output.begin() + start_index + AES_BLOCK_SIZE_B * (current_letter_idx / AES_BLOCK_SIZE_B), current_output.begin() + start_index + AES_BLOCK_SIZE_B * (1 + current_letter_idx / AES_BLOCK_SIZE_B));

            if(test_block == output_block)
            {
                decoded_data.push_back((char)ascii_to_find);

                for(auto matcher_idx = 0; matcher_idx < AES_BLOCK_SIZE_B - 1; matcher_idx++)
                {
                    std::swap(matcher[matcher_idx], matcher[matcher_idx+1]);
                }
                matcher[AES_BLOCK_SIZE_B - 2] = ascii_to_find;
                break;
            }
        }
    }

    Printer::WriteIoStream(decoded_data, PrintOutputType_T::CHAR);
}

void Solutions::Challenge15()
{
    bool padding_valid = false;
    padding_valid = AES::PKCS7PaddingValidator("ICE ICE BABY\x05\x05\x05\x05");

    std::cout << padding_valid << std::endl;
}