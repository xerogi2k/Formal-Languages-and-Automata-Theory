#pragma once
#include "RegexNode.h"
#include "DFA.h"
#include <vector>
#include <map>
#include <set>
#include <string>
#include <algorithm>

class RegexTree
{
public:
    RegexNode rootNode;
    std::vector<Pair> followPositions;

    RegexTree(std::string regex, std::set<char> alphabet);

    void WriteTree();

    DFA ConvertToDFA(std::set<char> alphabetSet);

private:
    bool HasHashSymbol(const std::vector<int>& positions);
};