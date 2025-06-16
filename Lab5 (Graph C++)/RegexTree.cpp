#include "RegexTree.h"

RegexTree::RegexTree(std::string regex, std::set<char> alphabet)
{
    rootNode = RegexNode(regex, alphabet);
    followPositions = {};
    rootNode.CalculateFunction(0, followPositions, alphabet);
}

void RegexTree::WriteTree()
{
    rootNode.WriteLevel(0);
}

DFA RegexTree::ConvertToDFA(std::set<char> alphabetSet)
{
    std::vector<std::vector<int>> markedStates;
    std::vector<std::vector<int>> allStates;
    alphabetSet.erase('#');
    alphabetSet.erase('_');
    std::vector<std::map<char, int>> transitions; 
    std::vector<int> initialState = rootNode.firstPositions; 
    std::vector<int> finalStates;

    allStates.push_back(initialState);
    if (HasHashSymbol(initialState))
        finalStates.push_back(0);

    while (allStates.size() - markedStates.size() > 0)
    {
        std::vector<int> currentState;
        for (auto& state : allStates)
        {
            if (std::find(markedStates.begin(), markedStates.end(), state) == markedStates.end())
            {
                currentState = state; 
                break;
            }
        }

        transitions.push_back({}); 
        markedStates.push_back(currentState);

        for (char symbol : alphabetSet)
        {
            std::vector<int> nextState;

            for (int position : currentState)
            {
                if (followPositions[position].key == symbol)
                {
                    nextState.insert(nextState.end(), followPositions[position].value.begin(), followPositions[position].value.end());
                }
            }

            if (nextState.empty())
                continue;

            std::sort(nextState.begin(), nextState.end());
            nextState.erase(std::unique(nextState.begin(), nextState.end()), nextState.end());


            if (std::find(allStates.begin(), allStates.end(), nextState) == allStates.end())
            {
                allStates.push_back(nextState);
                if (HasHashSymbol(nextState))
                {
                    finalStates.push_back(std::find(allStates.begin(), allStates.end(), nextState) - allStates.begin());
                }
            }

            transitions[std::find(allStates.begin(), allStates.end(), currentState) - allStates.begin()][symbol] =
                std::find(allStates.begin(), allStates.end(), nextState) - allStates.begin();
        }
    }

    return DFA(allStates, alphabetSet, transitions,
        std::find(allStates.begin(), allStates.end(), initialState) - allStates.begin(),
        finalStates);
}

bool RegexTree::HasHashSymbol(const std::vector<int>& positions)
{
    for (int position : positions)
    {
        if (followPositions[position].key == '#')
            return true;
    }
    return false;
}
