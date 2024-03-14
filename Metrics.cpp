#include "Common.hpp"
#include "Metrics.hpp"
#include <cmath>

const std::array<double, 26> Letter_Frequencies = 
{
    0.082,
    0.015,
    0.028,
    0.043,
    0.127,
    0.022,
    0.020,
    0.061,
    0.070,
    0.0015,
    0.0077,
    0.04,
    0.024,
    0.067,
    0.075,
    0.019,
    0.0095,
    0.06,
    0.063,
    0.091,
    0.028,
    0.0098,
    0.024,
    0.0015,
    0.02,
    0.0074
};

bool IsLetter(unsigned short byte)
{
    bool is_letter = false;
    if( (byte >= 65 && byte <= 90) || (byte >= 97 && byte <= 122) )
    {
        is_letter = true;
    }
    return is_letter;
}

char LetterAlphabetPosition(char letter)
{
    if(letter <= 90)
    {
        letter -= 65;
    }
    else if(letter >= 97)
    {
        letter -= 97;
    }
    return letter;
}

double Metrics::LetterFrequency(byte_buffer input)
{
    double score = 0;

    std::array<double, 26> letters_distribution = {};
    unsigned int num_letters = 0;

    for(unsigned short letter : input)
    {
        if(IsLetter(letter))
        {
            letters_distribution[LetterAlphabetPosition(letter)]++;
            num_letters++;
        }
    }
    for(size_t i=0; i<26; i++)
    {
        letters_distribution[i] /= num_letters;
    }

    for(size_t i=0; i<26; i++)
    {
        score += sqrt(letters_distribution[i] * Letter_Frequencies[i]);
    }

    return score;
}

int Metrics::LetterFrequency2(byte_buffer input)
{
    int score = 0;

    const std::array<char, 24> most_frequent_letters = {'e','t','a','o','i','n','s','h','r','d','l','u', 'E','T','A','O','I','N','S','H','R','D','L','U'};
                                            
    for(unsigned short letter : input)
    {
        if(std::find(most_frequent_letters.begin(), most_frequent_letters.end(), static_cast<char>(letter)) != most_frequent_letters.end())
        {
            score++;
        }
    }
    return score;
}

int Metrics::HammingDistance(byte_buffer in1, byte_buffer in2)
{
    unsigned int distance = -1;
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