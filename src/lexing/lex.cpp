#include "../../include/lexing/lex.h"

lex::lex() {
    this->re = {
        {CE(LE({{48, 57}}), KE(LE({{48, 57}}))), "Num"},
        {CE(LE({{97, 122}, {65, 90}}), KE(LE({{97, 122}, {65, 90}, {48, 57}}))), "Id"},
        {CE(CE(LE({{105, 105}}), LE({{110, 110}})), LE({{116, 116}})), "Int"},
        {CE(CE(CE(CE(CE(LE({{115, 115}}), LE({{116, 116}})), LE({{114, 114}})), LE({{117, 117}})), LE({{99, 99}})), LE({{116, 116}})), "Struct"},
        {CE(CE(LE({{110, 110}}), LE({{105, 105}})), LE({{108, 108}})), "Nil"},
        {CE(CE(CE(CE(LE({{98, 98}}), LE({{114, 114}})), LE({{101, 101}})), LE({{97, 97}})), LE({{107, 107}})), "Break"},
        {CE(CE(CE(CE(CE(CE(CE(LE({{99, 99}}), LE({{111, 111}})), LE({{110, 110}})), LE({{116, 116}})), LE({{105, 105}})), LE({{110, 110}})), LE({{117, 117}})), LE({{101, 101}})), "Continue"},
        {CE(CE(CE(CE(CE(LE({{114, 114}}), LE({{101, 101}})), LE({{116, 116}})), LE({{117, 117}})), LE({{114, 114}})), LE({{110, 110}})), "Return"},
        {CE(LE({{105, 105}}), LE({{102, 102}})), "If"},
        {CE(CE(CE(LE({{101, 101}}), LE({{108, 108}})), LE({{115, 115}})), LE({{101, 101}})), "Else"},
        {CE(CE(CE(CE(LE({{119, 119}}), LE({{104, 104}})), LE({{105, 105}})), LE({{108, 108}})), LE({{101, 101}})), "While"},
        {CE(CE(LE({{110, 110}}), LE({{101, 101}})), LE({{119, 119}})), "New"},
        {CE(CE(LE({{108, 108}}), LE({{101, 101}})), LE({{116, 116}})), "Let"},
        {CE(CE(CE(CE(CE(LE({{101, 101}}), LE({{120, 120}})), LE({{116, 116}})), LE({{101, 101}})), LE({{114, 114}})), LE({{110, 110}})), "Extern"},
        {CE(LE({{102, 102}}), LE({{110, 110}})), "Fn"},
        {LE({{38, 38}}), "Address"},
        {LE({{58, 58}}), "Colon"},
        {LE({{59, 59}}), "Semicolon"},
        {LE({{44, 44}}), "Comma"},
        {LE({{95, 95}}), "Underscore"},
        {CE(LE({{45, 45}}), LE({{62, 62}})), "Arrow"},
        {LE({{43, 43}}), "Plus"},
        {LE({{45, 45}}), "Dash"},
        {LE({{42, 42}}), "Star"},
        {LE({{47, 47}}), "Slash"},
        {CE(LE({{61, 61}}), LE({{61, 61}})), "Equal"},
        {CE(LE({{33, 33}}), LE({{61, 61}})), "NotEq"},
        {LE({{60, 60}}), "Lt"},
        {CE(LE({{60, 60}}), LE({{61, 61}})), "Lte"},
        {LE({{62, 62}}), "Gt"},
        {CE(LE({{62, 62}}), LE({{61, 61}})), "Gte"},
        {LE({{46, 46}}), "Dot"},
        {LE({{61, 61}}), "Gets"},
        {LE({{40, 40}}), "OpenParen"},
        {LE({{41, 41}}), "CloseParen"},
        {LE({{91, 91}}), "OpenBracket"},
        {LE({{93, 93}}), "CloseBracket"},
        {LE({{123, 123}}), "OpenBrace"},
        {LE({{125, 125}}), "CloseBrace"},
    };
}

void lex::lexing(string input) {
    int left = 0, right;
    string phrase;
    while (left < input.length()) {
        if (isspace(input[left])) {
            left++;
            continue;
        }
        right = left + 1;
        while (right < input.length() && !isspace(input[right])) {
            right++;
        }
        
        phrase = input.substr(left, right - left);

        if (phrase.length() >= 2 && (phrase.substr(0, 2) == "//" || phrase.substr(0, 2) == "/*")) {
            if (phrase.substr(0, 2) == "//") {
                right = left + 2;
                while (right < input.length() && input[right] != '\n') {
                    right++;
                }
                left = right;
            }
            else {
                right = left + 3;
                while (right < input.length()) {
                    if (input[right - 1] == '*' && input[right] == '/') {
                        break;
                    }
                    right++;
                }
                left = right + 1;
            }
        }
        else {
            int maxMunch = 0;
            string maxToken = "Error";
            for (auto& pair : re) {
                RegularExpression RE = pair.first;
                string token = pair.second;
                
                int munch = RunNFA(RE.start, phrase);
                if (munch >= maxMunch) {
                    maxMunch = munch;
                    maxToken = token;
                }
            }

            if (maxToken == "Id" || maxToken == "Num") {
                this->tokens.push_back(maxToken + "(" + phrase.substr(0, maxMunch) + ")");
            }
            else  {
                if (maxMunch == 0) {
                    left++;
                    throw "Error";
                }
                else {
                    this->tokens.push_back(maxToken);
                }
            }
            left += maxMunch;
        }
    }
}