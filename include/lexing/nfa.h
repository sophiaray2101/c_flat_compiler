#ifndef NFA_H
#define NFA_H

#include <vector>
#include <utility>
#include <string>
#include <queue>

using namespace std;

struct Node {
    vector<pair<pair<int, int>, Node*>> transitions;
    bool isTerminalNode = false;
};

struct State {
    int currentIndex;
    Node *currentNode;

    State(int currIndex, Node* currNode) : currentIndex(currIndex), currentNode(currNode) {}
};

bool TransitionState(State& currentState, string input, queue<State>& nextStates);
int RunNFA(Node* startNode, string input);

#endif