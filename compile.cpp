#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include "./include/lexing/lex.h"
#include "./include/ast_data_types/ast.h"
#include "./include/grammar/grammar.h"
#include "./include/type_check/TypeCheck.h"
#include "./include/lowering/lowering.h"
#include "./include/codegen/CodeGen.h"

using namespace std;

int main(int argc, char* argv[]) {
    string filePath = argv[1];
    ifstream file(filePath);

    if (!file.is_open()) { 
        cerr << "Failed to open file: " << filePath << endl; 
        return 1;  
    }

    string input((istreambuf_iterator<char>(file)), istreambuf_iterator<char>()); 
    file.close();

    lex l;
    try {
        l.lexing(input);
    } catch (const string e) {
        cout << "Lexing error" << endl;
        return 0;
    }

    int index = 0;
    Program p;
    try {
        program(l.tokens, index, p);

        vector<string> errors;
        ProgramTC(errors, p);
        sort(errors.begin(), errors.end());

        if (errors.size() > 0) {
            cout << p.to_string() << endl;
            for (const string& e : errors) {
                cout << e << endl;
            }
            return 0;
        }
        
    } catch (const int& e) {
        cout << "parse error at token " << e << endl;
        return 0;
    }

    ProgramLIR programLIR = ProgramLIR(p);
    CodeGen cg = CodeGen(programLIR);
    cout << cg.to_string() << endl;
    return 0;
}