#include "Solutions.hpp"
#include "Metrics.hpp"

void Solutions::Challenge1()
{
    std::string input;
    std::cin >> input;

    Printer printer = Printer();

    byte_buffer data = FormatConversions::HexString2ByteBuffer(input);

    std::string output = FormatConversions::Base64Encoder(data);

    printer.WriteIoStream(output);
}

void Solutions::Challenge2()
{
    std::string in1, in2;
    std::cin >> in1 >> in2;

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
    input_file.open("../4.txt");

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

}

void Solutions::Challenge6()
{

}