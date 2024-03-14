#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <algorithm>
#include <fstream>
#include <queue>
#include <iomanip>

#include "Common.hpp"

static const byte_buffer letter_frequencies = {82, 15, 28, 34, 127, 22, 20, 61, 70, 2, 10, 40, 24, 67, 75, 19, 1, 60, 63, 91, 28, 10, 24, 1, 20, 1};

class FileReader
{
    std::ifstream file;
    std::vector<std::string> lines;

    public:
    FileReader(std::string file_name)
    {
        file.open(file_name);
        
        if(file.is_open())
        {
            std::string line{};
            while(std::getline(file, line))
            {
                lines.push_back(line);
            }
        }
        file.close();
    }
    std::vector<std::string> GetLines()
    {
        return lines;
    }
};

class FileWriter
{
    std::ofstream file;

    public:
    FileWriter(std::string file_name)
    {
        file.open(file_name);
    }

    std::ostream& operator<<(std::string line)
    {
        file << line;
    }

    ~FileWriter()
    {
        file.close();
    }
};

typedef struct Line
{
    int metric;
    std::string line;
} Line;

bool CompareLines(Line a, Line b)
{
    bool return_value = false;
    if(a.metric < b.metric)
    {
        return_value = true;
    }

    return return_value;
}

int main(void)
{
    /* Challenge 1.
    std::string input;
    std::cin >> input;

    std::string output = CryptoConversions::Hex2Base64(input);
    std::cout << output << std::endl;
    */

    /* Challenge 2.
    std::string input1, input2;
    std::cin >> input1 >> input2;
    std::vector output = CryptoConversions::XorBuffers(input1, input2);
    
    for(auto element : output)
    {
        std::cout << std::hex << element;
    }
    */

    /* Challenge 3. 
    1b 37 37 33 31 36 3f 78 15 1b 7f 2b 78 34 31 33 3d 78 39 78 28 37 2d 36 3c 78 37 3e 78 3a 39 3b 37 36
    C  o  o k  i   n  g -- M  C   ' s  --  l  i  k  e --  a -- p  o  u  n  d  -- o  f  -- b  a  c  o  n
    */

   /* Challenge 4. 
   FileReader file_reader("/home/swift/cryptopals/1/4.txt");
   FileWriter file_writer("/home/swift/cryptopals/1/4_output.txt");


    for(unsigned char coding_char = 0; coding_char<255; coding_char++)
    {
        file_writer << "Decoding: " << static_cast<int>(coding_char) << "\n\n";

        int line_index{0};

        std::priority_queue<Line, std::vector<Line>, std::function<bool(Line, Line)>> queue(CompareLines);

        for(std::string line : file_reader.GetLines())
        {
            int new_score = 0;
            std::string output_line = CryptoConversions::DecryptXorString(line, coding_char, new_score);
            if(output_line.length() > 0)
            {
                Line x{.metric = new_score, .line = output_line};
                queue.push(x);
                
            }
            line_index++;
        }

        int idx = 0;
        while( (!queue.empty()) && (idx < 2) )
        {
            Line output = queue.top();
            queue.pop();
            file_writer << " " << output.metric << " " << output.line << "\n\n";
            idx++;
        }
    }
    */

   /* Challenge 5. 

    const std::string input_string = "Burning 'em, if you ain't quick and nimble\nI go crazy when I hear a cymbal";

    const std::string xor_key = "ICE";

    std::string result1 = CryptoConversions::EncryptSequentialXor(input_string, xor_key);

    for(char x : result1)
    {
        std::cout << std::setfill('0') << std::setw(2) << std::hex << static_cast<short>(x);
    }
    std::cout << std::endl;

    */

   /* Challenge 6. */
   FileReader file_reader("/home/swift/cryptopals/1/6.txt");

   byte_buffer input_buffer;

   for(std::string line : file_reader.GetLines())
   {
        std::vector<unsigned short> encoded_line = CryptoConversions::Base64Decoder(line);
        input_buffer.insert(input_buffer.end(), encoded_line.begin(), encoded_line.end());
   }

    short key_size = CryptoConversions::FindPossibleKeySize(input_buffer);

    std::vector<byte_buffer> test{};
    CryptoConversions::BreakBufferToChunks(input_buffer, test, key_size);

    std::vector<byte_buffer> test2{};
    CryptoConversions::TransposeBuffer(test, test2);

    byte_buffer key{};
    for(byte_buffer buf1 : test2)
    {
        unsigned short key_char = 0;
        CryptoConversions::DecryptSingleByteXor(buf1, key_char);
        key.push_back(key_char);
    }

    std::cout << std::endl;
    for(unsigned short byte : key)
    {
        std::cout << static_cast<char>(byte);
    }
    std::cout << std::endl;

    byte_buffer output = CryptoConversions::EncryptSequentialXor(input_buffer, key);

    for(unsigned char byte : output)
    {
        std::cout << byte;
    }
    

}