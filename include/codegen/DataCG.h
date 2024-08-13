#ifndef DATACG_H
#define DATACG_H

#include <string>

using namespace std;

// DataCG
struct DataCG {
    string name;
    bool isFunc;        

    DataCG();
    string to_string() const;
};

#endif