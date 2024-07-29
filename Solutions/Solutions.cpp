#include "Solutions.hpp"
#include "Metrics.hpp"
#include <numeric>

void Solutions::Challenge1()
{
    std::string input = "49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d";

    Printer printer = Printer();

    byte_buffer data = FormatConversions::HexString2ByteBuffer(input);

    std::string output = FormatConversions::Base64Encoder(data);

    printer.WriteIoStream(output);
}

void Solutions::Challenge2()
{
    std::string in1 = "1c0111001f010100061a024b53535009181c";
    std::string in2 = "686974207468652062756c6c277320657965";

    Printer printer = Printer();

    byte_buffer input1 = FormatConversions::HexString2ByteBuffer(in1);
    byte_buffer input2 = FormatConversions::HexString2ByteBuffer(in2);

    byte_buffer output = CryptoMethods::XorBuffers(input1, input2);
    
    printer.WriteIoStream(output, PrintOutputType_T::HEX);
}

void Solutions::Challenge3()
{
    std::string hex_string = "1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736";

    Printer printer = Printer();

    byte_buffer bytes = FormatConversions::HexString2ByteBuffer(hex_string);

    unsigned char key = 0;
    double score = 0;
    byte_buffer result = CryptoMethods::SingleByteXor_DecryptBuffer(bytes, key, score);

    printer.WriteIoStream(result, PrintOutputType_T::CHAR);
}

void Solutions::Challenge4()
{
    std::ifstream input_file;
    input_file.open("../inputs/4.txt");

    Printer printer = Printer();

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
    
    printer.WriteIoStream(best_buffer, PrintOutputType_T::CHAR);

    input_file.close();
}

void Solutions::Challenge5()
{
    Printer printer = Printer();

    const byte_buffer input_buffer = FormatConversions::CharString2ByteBuffer("Burning 'em, if you ain't quick and nimble\nI go crazy when I hear a cymbal");
    const byte_buffer xor_key = FormatConversions::CharString2ByteBuffer("ICE");

    byte_buffer result = CryptoMethods::SequentialXor_EncryptBuffer(input_buffer, xor_key);

    printer.WriteIoStream(result, PrintOutputType_T::HEX);
}

void Solutions::Challenge6()
{
    std::ifstream input_file;
    input_file.open("../inputs/6.txt");

    Printer printer = Printer();

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

    printer.WriteIoStream(key, PrintOutputType_T::CHAR);

    byte_buffer output = CryptoMethods::SequentialXor_EncryptBuffer(input_buffer, key);

    printer.WriteIoStream(output, PrintOutputType_T::CHAR);
}

void Solutions::Challenge7()
{
    Printer printer = Printer();

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

    printer.WriteIoStream(decrypted_buffer, PrintOutputType_T::CHAR);
}

void Solutions::Challenge8()
{
    Printer printer = Printer();

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
                    printer.WriteIoStream(chunk, PrintOutputType_T::HEX);
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
    Printer printer = Printer();

    byte_buffer input = FormatConversions::CharString2ByteBuffer("YELLOW SUBMARINE");
    
    AES::PKCS7Padding(input, 20);

    printer.WriteIoStream(input, HEX);
}

void Solutions::Challenge10()
{
    Printer printer = Printer();

    byte_buffer input;
    std::string input_key = "YELLOW SUBMARINE";
    byte_buffer iv(AES_BLOCK_SIZE_B, 0);

    std::ifstream input_file;
    input_file.open("../inputs/test_decrypt_encrypt.txt");

    std::string line;
    while(getline(input_file, line))
    {
        byte_buffer line_buffer = FormatConversions::Base64Decoder(line);
        input.insert(input.end(), line_buffer.begin(), line_buffer.end());
    }

    printer.WriteIoStream(input, PrintOutputType_T::CHAR);

    byte_buffer encrypted_buffer = AES::Encrypt(input, FormatConversions::CharString2ByteBuffer(input_key), AES_BlockCipherMode_T::CBC, iv);
    byte_buffer decrypted_buffer = AES::Decrypt(encrypted_buffer, FormatConversions::CharString2ByteBuffer(input_key), AES_BlockCipherMode_T::CBC, iv);

    printer.WriteIoStream(decrypted_buffer, PrintOutputType_T::CHAR);

    
}