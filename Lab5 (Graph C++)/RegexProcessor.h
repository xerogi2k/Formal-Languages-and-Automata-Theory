#include <string>
#include <set>
#include <fstream>
#include "RegexTree.h"

class RegexProcessor
{
public:
    std::set<char> alphabet;
    RegexProcessor() = default;

    std::string Preprocess(std::string regex);
    void ExtractAlphabet(const std::string& regex);
    void ConvertAndWriteDFA(const std::string& regex, const std::set<char>& alphabet, const std::string& output_file);

};
