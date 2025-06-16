#include <iostream>
#include <fstream>
#include <string>
#include "RegexProcessor.h"

int main(int argc, char* argv[]) 
{
    if (argc < 3) 
    {
        std::cout << "Usage: " << argv[0] << " <input.txt> <output.csv>" << std::endl;
        return 1;
    }
    std::ifstream input(argv[1]);
    std::string regex;
    getline(input, regex);

    RegexProcessor processor;

    regex = processor.Preprocess(regex);
    processor.ExtractAlphabet(regex);
    processor.ConvertAndWriteDFA(regex, processor.alphabet, argv[2]);

    return 0;
}
