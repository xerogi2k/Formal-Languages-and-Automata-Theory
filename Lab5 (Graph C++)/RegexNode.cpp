#include "RegexNode.h"

bool IsBracketsValid(const std::string& regex)
{
    int openBrackets = 0;
    for (char ch : regex)
    {
        if (ch == '(')
            openBrackets++;
        else if (ch == ')')
            openBrackets--;
        if (openBrackets < 0)
            return false;
    }
    if (openBrackets == 0)
        return true;
    return false;
}

bool IsOperationsValid(const std::string& regex)
{
    for (int i = 0; i < regex.size(); i++)
    {
        if (regex[i] == '*' or regex[i] == '+')
        {
            if (i == 0)
                return false;
            if (regex[i - 1] == '(' or regex[i - 1] == '|')
                return false;
        }
        if (regex[i] == '|')
        {
            if (i == 0 or i == regex.size() - 1)
                return false;
            if (regex[i - 1] == '(' or regex[i - 1] == '|')
                return false;
            if (regex[i + 1] == ')' or regex[i - 1] == '|')
                return false;
        }
    }
    return true;
}

bool IsRegexValid(const std::string& regex)
{
    return IsBracketsValid(regex) and IsOperationsValid(regex);
}


RegexNode::RegexNode() {}

RegexNode::RegexNode(std::string regex, std::set<char> alphabet)
{
    std::cout << regex << std::endl;
    if (regex.size() == 1 && IsAlphabetCharacter(regex[0], alphabet))
    {
        symbol = regex[0];
        isNullable = (symbol == '_');
        return;
    }

    int kleene = -1, plus = -1, orOperator = -1, concatenation = -1;
    int index = 0;

    while (index < regex.size())
    {
        if (regex[index] == '(')
        {
            int bracketLevel = 1;
            index++;
            while (bracketLevel != 0 && index < regex.size())
            {
                if (regex[index] == '(')
                    bracketLevel++;
                else if (regex[index] == ')')
                    bracketLevel--;
                index++;
            }
        }
        else
        {
            index++;
        }

        if (index == regex.size()) break;

        if (IsConcatenationOperator(regex[index], alphabet))
        {
            if (concatenation == -1)
                concatenation = index;
            continue;
        }

        if (regex[index] == '*')
        {
            if (kleene == -1)
                kleene = index;
            continue;
        }

        if (regex[index] == '+')
        {
            if (plus == -1)
                plus = index;
            continue;
        }

        if (regex[index] == '|')
        {
            if (orOperator == -1)
                orOperator = index;
            index++;
        }
    }

    if (orOperator != -1)
    {
        symbol = '|';
        childNodes.push_back(RegexNode(TrimParentheses(regex.substr(0, orOperator)), alphabet));
        childNodes.push_back(RegexNode(TrimParentheses(regex.substr(orOperator + 1)), alphabet));
    }
    else if (concatenation != -1)
    {
        symbol = '.';
        childNodes.push_back(RegexNode(TrimParentheses(regex.substr(0, concatenation)), alphabet));
        childNodes.push_back(RegexNode(TrimParentheses(regex.substr(concatenation)), alphabet));
    }
    else if (plus != -1)
    {
        symbol = '+';
        childNodes.push_back(RegexNode(regex.substr(0, plus) + regex.substr(0, plus) + '*', alphabet));
    }
    else if (kleene != -1)
    {
        symbol = '*';
        childNodes.push_back(RegexNode(TrimParentheses(regex.substr(0, kleene)), alphabet));
    }
}

int RegexNode::CalculateFunction(int position, std::vector<Pair>& followPositions, std::set<char> alphabet)
{
    if (IsAlphabetCharacter(symbol, alphabet)) 
    {
        firstPositions = { position };
        lastPositions = { position };
        nodePosition = position;
        followPositions.push_back({ symbol, {} });
        return position + 1;
    }

    for (auto& child : childNodes)
        position = child.CalculateFunction(position, followPositions, alphabet);

    if (symbol == '.')
    {
        if (childNodes[0].isNullable)
        {
            firstPositions = childNodes[0].firstPositions;
            firstPositions.insert(firstPositions.end(), childNodes[1].firstPositions.begin(), childNodes[1].firstPositions.end());
            std::sort(firstPositions.begin(), firstPositions.end());
            firstPositions.erase(std::unique(firstPositions.begin(), firstPositions.end()), firstPositions.end());
        }
        else
            firstPositions = childNodes[0].firstPositions;

        if (childNodes[1].isNullable)
        {
            lastPositions = childNodes[0].lastPositions;
            lastPositions.insert(lastPositions.end(), childNodes[1].lastPositions.begin(), childNodes[1].lastPositions.end());
            std::sort(lastPositions.begin(), lastPositions.end());
            lastPositions.erase(std::unique(lastPositions.begin(), lastPositions.end()), lastPositions.end());
        }
        else
            lastPositions = childNodes[1].lastPositions;

        isNullable = childNodes[0].isNullable && childNodes[1].isNullable;

        for (int i : childNodes[0].lastPositions)
            for (int j : childNodes[1].firstPositions)
                if (std::find(followPositions[i].value.begin(), followPositions[i].value.end(), j) == followPositions[i].value.end())
                {
                    followPositions[i].value.push_back(j);
                    std::sort(followPositions[i].value.begin(), followPositions[i].value.end());
                }
    }
    else if (symbol == '|') 
    {
        firstPositions = childNodes[0].firstPositions;
        firstPositions.insert(firstPositions.end(), childNodes[1].firstPositions.begin(), childNodes[1].firstPositions.end());
        std::sort(firstPositions.begin(), firstPositions.end());
        firstPositions.erase(std::unique(firstPositions.begin(), firstPositions.end()), firstPositions.end());

        lastPositions = childNodes[0].lastPositions;
        lastPositions.insert(lastPositions.end(), childNodes[1].lastPositions.begin(), childNodes[1].lastPositions.end());
        std::sort(lastPositions.begin(), lastPositions.end());
        lastPositions.erase(std::unique(lastPositions.begin(), lastPositions.end()), lastPositions.end());

        isNullable = childNodes[0].isNullable || childNodes[1].isNullable;
    }
    else if (symbol == '*' || symbol == '+') 
    {
        firstPositions = childNodes[0].firstPositions;
        lastPositions = childNodes[0].lastPositions;
        if (symbol == '*')
            isNullable = true;
        else
            isNullable = childNodes[0].isNullable;

        for (int i : childNodes[0].lastPositions)
            for (int j : childNodes[0].firstPositions)
                if (std::find(followPositions[i].value.begin(), followPositions[i].value.end(), j) == followPositions[i].value.end())
                {
                    followPositions[i].value.push_back(j);
                    std::sort(followPositions[i].value.begin(), followPositions[i].value.end());
                }
    }

    return position;
}

void RegexNode::WriteLevel(int level)
{
    std::cout << level << " " << symbol << " [ ";
    for (auto& pos : firstPositions)
        std::cout << pos << " ";
    std::cout << "] [ ";
    for (auto& pos : lastPositions)
        std::cout << pos << " ";
    std::cout << "] " << isNullable << " " << nodePosition << std::endl;

    for (auto& child : childNodes)
        child.WriteLevel(level + 1);
}

std::string RegexNode::TrimParentheses(std::string regex)
{
    while (regex[0] == '(' && regex[regex.size() - 1] == ')' && IsRegexValid(regex.substr(1, regex.size() - 2)))
        regex = regex.substr(1, regex.size() - 2);
    return regex;
}

bool RegexNode::IsAlphabetCharacter(char character, std::set<char> alphabet)
{
    return alphabet.find(character) != alphabet.end();
}

bool RegexNode::IsConcatenationOperator(char character, std::set<char> alphabet)
{
    return character == '(' || IsAlphabetCharacter(character, alphabet);
}
