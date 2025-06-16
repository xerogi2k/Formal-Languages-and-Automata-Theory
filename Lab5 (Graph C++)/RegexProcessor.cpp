#include "RegexProcessor.h"

std::string RegexProcessor::Preprocess(std::string regex)
{
    while (regex.find(" ") != std::string::npos)
        regex.erase(regex.find(" "), 1);

    for (int i = 0; i < regex.size(); i++)
    {
        while (i < regex.size() - 1 &&
            ((regex[i] == '+' && regex[i + 1] == '*') ||
                (regex[i] == '*' && regex[i + 1] == '+')))
            regex.replace(i, 2, "*");
    }

    for (int i = 0; i < regex.size(); i++)
    {
        while (i < regex.size() - 1 && regex[i] == regex[i + 1] &&
            (regex[i] == '*' || regex[i] == '+'))
            regex.erase(i, 1);
    }

    regex = "(" + regex + ")#";

    while (regex.find("()") != std::string::npos)
        regex.erase(regex.find("()"), 2);

    return regex;
}

void RegexProcessor::ExtractAlphabet(const std::string& regex)
{
    for (char ch : regex)
        if (ch != '(' && ch != ')' && ch != '+' && ch != '*' && ch != '|')
            alphabet.insert(ch);
}

void RegexProcessor::ConvertAndWriteDFA(const std::string& regex, const std::set<char>& alphabet, const std::string& output_file)
{
    auto tree = RegexTree(regex, alphabet);
    auto auth = tree.ConvertToDFA(alphabet);
    auth.Write(output_file);
}