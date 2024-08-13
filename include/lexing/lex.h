#ifndef LEX_H
#define LEX_H

#include "./re.h"
#include <vector>
#include <string>

using namespace std;

struct lex {
    vector<pair<RegularExpression, string>> re;
    vector<string> tokens;

    lex();
    void lexing(string input);
};

#endif