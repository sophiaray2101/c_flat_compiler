#include "../../include/lexing/re.h"

using namespace std;

pair<Node*, Node*> LE::ConvertToNFA() {
    Node* start = new Node();
    Node* end = new Node();
    end->isTerminalNode = true; 
    end->transitions = {};
    for (auto& ranges : this->literals) {
       start->transitions.push_back({ranges, end});
    }

    return {start, end};
}

pair<Node*, Node*> KE::ConvertToNFA() {
    this->inner.end->isTerminalNode = false;

    Node* start = new Node();
    Node* end = new Node();
    end->isTerminalNode = true; 
    end->transitions = {};

    start->transitions = {{{-1, -1}, this->inner.start},
                          {{-1, -1}, end}};
    this->inner.end->transitions.push_back({{-1, -1}, end});
    this->inner.end->transitions.push_back({{-1, -1}, this->inner.start});

    return {start, end};
}

pair<Node*, Node*> UE::ConvertToNFA() {
    this->A.end->isTerminalNode = false;
    this->B.end->isTerminalNode = false;

    Node* start = new Node();
    Node* end = new Node();
    end->isTerminalNode = true; 
    end->transitions = {};

    start->transitions = {{{-1, -1}, this->A.start},
                          {{-1, -1}, this->B.start}};
    this->A.end->transitions.push_back({{-1, -1}, end});
    this->B.end->transitions.push_back({{-1, -1}, end});

    return {start, end};
}

pair<Node*, Node*> CE::ConvertToNFA() {
    this->left.end->isTerminalNode = false;

    Node* start = this->left.start;
    Node* end = this->right.end;

    this->left.end->transitions.push_back({{-1, -1}, this->right.start});

    return {start, end};
}