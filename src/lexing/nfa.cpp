#include "../../include/lexing/nfa.h"

bool TransitionState(State& currentState, string input, queue<State>& nextStates) {
    Node* currNode = currentState.currentNode;
    int currIndex = currentState.currentIndex;

    if (currNode->isTerminalNode) {
        return true;
    }

    int entryASCII = (currIndex < input.length()) ? static_cast<int>(input[currIndex]) : -2;
    for (auto& transition : currNode->transitions) {
        int start_range = transition.first.first;
        int end_range = transition.first.second;
        if (start_range == -1 && end_range == -1) {
            nextStates.push(State(currIndex, transition.second));
        }
        if (entryASCII >= start_range && entryASCII <= end_range) {
            nextStates.push(State(currIndex + 1, transition.second));
        }
    }
    
    return false;
}

int RunNFA(Node* startNode, string input) {
    State currentState = State(0, startNode);
    queue<State> nextStates;
    int largestIndex = 0;

    TransitionState(currentState, input, nextStates);
    while (!nextStates.empty()) {
        currentState = nextStates.front();
        nextStates.pop();
        if (TransitionState(currentState, input, nextStates)) {
            largestIndex = max(largestIndex, currentState.currentIndex);
        }
    }
    return largestIndex;
}
