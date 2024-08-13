#ifndef RE_H
#define RE_H

#include <utility>
#include "nfa.h"

using namespace std;

struct RegularExpression {
    Node* start;
    Node* end;
};

struct LE : RegularExpression {
    vector<pair<int, int>> literals;
    pair<Node*, Node*> ConvertToNFA();

    LE(vector<pair<int, int>> literals) {
        this->literals = literals;
        pair<Node*, Node*> nfa = this->ConvertToNFA();
        this->start = nfa.first;
        this->end = nfa.second;
    }
};
struct KE : RegularExpression {
    RegularExpression inner;
    pair<Node*, Node*> ConvertToNFA();

    KE(RegularExpression inner) {
        this->inner = inner;
        pair<Node*, Node*> nfa = this->ConvertToNFA();
        this->start = nfa.first;
        this->end = nfa.second;
    }
};

struct UE : RegularExpression {
    RegularExpression A;
    RegularExpression B;
    pair<Node*, Node*> ConvertToNFA();

    UE(RegularExpression A, RegularExpression B) {
        this->A = A;
        this->B = B;
        pair<Node*, Node*> nfa = this->ConvertToNFA();
        this->start = nfa.first;
        this->end = nfa.second;
    }
};

struct CE : RegularExpression {
    RegularExpression left;
    RegularExpression right;
    pair<Node*, Node*> ConvertToNFA();

    CE(RegularExpression left, RegularExpression right) {
        this->left = left;
        this->right = right;
        pair<Node*, Node*> nfa = this->ConvertToNFA();
        this->start = nfa.first;
        this->end = nfa.second;
    }
};

#endif