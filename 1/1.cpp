#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <algorithm>
#include <fstream>
#include <queue>
#include <iomanip>

#define BASE64_TOKEN_NUM_CHARS  6u

typedef std::vector<unsigned short> byte_buffer;

static const std::string base64_chars =
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";

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

namespace Metrics
{
    
    int LetterFrequency(std::string str)
    {
        const std::array<char, 24> most_frequent_letters = {'E','T','A','O','I','N','S','H','R','D','L','U',
                                              'e','t','a','o','i','n','s','h','r','d','l','u'};
        int score = 0;
        
        for(char letter : str)
        {
            if(std::find(most_frequent_letters.begin(), most_frequent_letters.end(), letter) != most_frequent_letters.end())
            {
                score++;
            }
        }
        return score;
    }
    

    int LetterFrequency(byte_buffer input)
    {
        int score = 0;
        const std::array<char, 12> most_frequent_letters = {'e','t','a','o','i','n','s','h','r','d','l','u'};
                                              

        for(unsigned short letter : input)
        {
            if(std::find(most_frequent_letters.begin(), most_frequent_letters.end(), static_cast<char>(letter)) != most_frequent_letters.end())
            {
                score++;
            }
        }
        return score;
    }

    int HammingDistance(std::string str1, std::string str2)
    {
        unsigned int distance = 0;
        if(str1.length() == str2.length())
        {
            for(unsigned int i=0; i<str1.length();i++)
            {
                int single_char_distance = 0;
                short x = str1[i] ^ str2[i];

                while(x > 0)
                {
                    single_char_distance += x & 1;
                    x >>= 1;
                }
                distance += single_char_distance;
            }
        }
        return distance;
    }

    int HammingDistance(byte_buffer in1, byte_buffer in2)
    {
        unsigned int distance = 0;
        if(in1.size() == in2.size())
        {
            for(unsigned int i=0; i<in1.size();i++)
            {
                int single_byte_distance = 0;
                unsigned short x = in1[i] ^ in2[i];

                while(x > 0)
                {
                    single_byte_distance += x & 1;
                    x >>= 1;
                }
                distance += single_byte_distance;
            }
        }
        return distance;
    }
}

namespace CryptoConversions
{
    std::string Hex2Base64(std::string hex_input)
    {
        std::string base64_output{};

        unsigned long index = 0;
        while(index < hex_input.length())
        {
            std::string str_token = hex_input.substr(index, BASE64_TOKEN_NUM_CHARS);

            index += BASE64_TOKEN_NUM_CHARS;

            unsigned long token = 0;
            for(unsigned int i=0; i<str_token.length(); i+=2)
            {
                token |= std::stoi(str_token.substr(i,2), 0, 16) << (16 - 4 * i);
            }

            std::string base64_token{};
            unsigned int mask = 0xFC0000;
            const int mask_limits[4] = {0x0, 0x1000, 0x40, 0x0};

            unsigned int num_tokens = 0;
            while(num_tokens < 4)
            {
                if(mask <= mask_limits[str_token.length()/2])
                {
                    base64_token.push_back('=');                    
                }
                else
                {
                    short base64_idx = (token & mask) >> (18 - num_tokens * 6);
                    mask >>= 6;
                    base64_token.push_back(base64_chars[base64_idx]);
                }
                num_tokens++;
            }

            base64_output.append(base64_token);
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

    std::vector<unsigned short> Base64Decoder(std::string input)
    {
        std::vector<unsigned short> output{};

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

    std::vector<int> XorBuffers(std::string buffer_1, std::string buffer_2)
    {
        std::vector<int> output_buffer{};
        if(buffer_1.length() == buffer_2.length())
        {
            for(size_t i=0; i<buffer_1.length(); i+=2)
            {
                uint8_t result = std::stoi(buffer_1.substr(i,2), 0, 16) ^ std::stoi(buffer_2.substr(i,2), 0, 16);
                output_buffer.push_back(result);
            }
        }
        return output_buffer;
    }

    std::string DecryptXorString(std::string in_buffer, char decryptor, int& score)
    {
        std::string decrypted_buffer{};
        for(size_t i=0; i<in_buffer.length(); i+=2)
        {
            char result_char = std::stoi(in_buffer.substr(i,2), 0, 16) ^ decryptor;

            if(result_char > 127)
            {
                decrypted_buffer.clear();
                break;
            }
            decrypted_buffer.push_back(result_char);
        }

        score = Metrics::LetterFrequency(decrypted_buffer);

        return decrypted_buffer;
    }

    std::string EncryptSequentialXor(std::string in_buffer, std::string key)
    {
        size_t key_max_idx {key.length()};
        std::cout << "Max idx: " << key_max_idx << std::endl;
        int key_idx {0};
        std::string out_buffer{};

        for(char x : in_buffer)
        {
            out_buffer.push_back(x ^ key[key_idx % key_max_idx]);
            key_idx++;
        }

        return out_buffer;
    }

    byte_buffer EncryptSequentialXor(byte_buffer in_buffer, byte_buffer key)
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


    void PrintByteBuffer(byte_buffer input)
    {
        for(unsigned short x : input)
        {
            std::cout << std::setfill('0') << std::setw(2) << std::hex << x;
        }
        std::cout << std::endl;
    }

    unsigned int FindPossibleKeySize(byte_buffer input)
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
                distance *= 100;
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

    void BreakBufferToChunks(byte_buffer& input, std::vector<byte_buffer>& output, unsigned int chunk_length)
    {
        unsigned int i = 0;
        while(i < input.size())
        {
            byte_buffer chunk(input.begin() + i, input.begin() + std::min(static_cast<size_t>(i) + chunk_length, input.size()));
            output.push_back(chunk);
            i += chunk_length;
        }
    }

    void TransposeBuffers(std::vector<byte_buffer>& input, std::vector<byte_buffer>& output)
    {
        /* Assuming that the first chunk contains full key length */
        /* ... which is a correct assumption */

        unsigned int key_size = input[0].size();
        unsigned int i = 0;

        for(int i=0; i<key_size; i++)
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

    byte_buffer DecryptSingleByteXor(byte_buffer in_buffer, unsigned short& key)
    {
        int best_score = 0;
        unsigned short best_char = 0;
        byte_buffer result_buffer{};
        
        for(unsigned short coding_char = 0; coding_char<255; coding_char++)
        {
            byte_buffer decrypted_buffer{};

            for(size_t i=0; i<in_buffer.size(); i++)
            {
                unsigned short result = in_buffer[i] ^ coding_char;

                if(result > 127)
                {
                    decrypted_buffer.clear();
                    break;
                }
                decrypted_buffer.push_back(result);
            }

            int score = Metrics::LetterFrequency(decrypted_buffer);
            if(score > best_score)
            {
                best_score = score;
                best_char = coding_char;
                result_buffer = decrypted_buffer;
            }

        }

        key = best_char;
        return result_buffer;
    }

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
    CryptoConversions::TransposeBuffers(test, test2);

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