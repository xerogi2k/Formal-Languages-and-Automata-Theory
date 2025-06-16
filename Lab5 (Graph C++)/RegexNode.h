#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <set>
#include "Pair.h"

class RegexNode
{
public:
    bool isNullable;
    std::vector<int> firstPositions;
    std::vector<int> lastPositions;
    char symbol;
    int nodePosition = -1;
    std::vector<RegexNode> childNodes;

    RegexNode();
    RegexNode(std::string regex, std::set<char> alphabet);

    int CalculateFunction(int position, std::vector<Pair>& followPositions, std::set<char> alphabet);
    void WriteLevel(int level);

private:
    std::string TrimParentheses(std::string regex);
    bool IsAlphabetCharacter(char character, std::set<char> alphabet);
    bool IsConcatenationOperator(char character, std::set<char> alphabet);
};