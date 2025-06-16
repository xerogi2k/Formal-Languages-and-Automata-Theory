#pragma once
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <string>

class DFA
{
private:
    std::vector<std::vector<int>> states;
    std::set<char> alphabet;
    std::vector<std::map<char, int>> transitions;
    int startState;
    std::vector<int> finalStates;

public:
    DFA(std::vector<std::vector<int>> states,
        std::set<char> alphabet,
        std::vector<std::map<char, int>> transitions,
        int startState,
        std::vector<int> finalStates);

    void Write(const std::string& outputFileName);
};