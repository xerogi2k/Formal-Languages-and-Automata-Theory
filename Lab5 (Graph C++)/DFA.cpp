#include "DFA.h"

DFA::DFA(std::vector<std::vector<int>> states,
    std::set<char> alphabet,
    std::vector<std::map<char, int>> transitions,
    int startState,
    std::vector<int> finalStates)
{
    this->states = states;
    this->alphabet = alphabet;
    this->transitions = transitions;
    this->startState = startState;
    this->finalStates = finalStates;
}

void DFA::Write(const std::string& outputFileName)
{
    std::ofstream output(outputFileName);

    output << ";";
    for (int i = 0; i < transitions.size(); i++)
    {
        if (std::find(finalStates.begin(), finalStates.end(), i) != finalStates.end())
            output << "F";
        output << ";";
    }
    output << std::endl
        << ";";

    for (int i = 0; i < transitions.size(); i++)
        output << "q" << i << ";";
    output << std::endl;

    for (char symbol : alphabet)
    {
        output << symbol << ";";
        for (int i = 0; i < transitions.size(); i++)
        {
            if (transitions[i].find(symbol) != transitions[i].end())
                output << "q" << transitions[i][symbol];
            output << ";";
        }
        output << std::endl;
    }
}
