#include "../../include/codegen/DataCG.h"

DataCG::DataCG() {}
string DataCG::to_string() const {
    string label = this->isFunc ? this->name + "_" : this->name;
    string res = this->isFunc ? ".quad \"" + this->name + "\"" : ".zero 8";
    return ".globl " + label + "\n" + label + ": " + res + "\n\n\n";
}