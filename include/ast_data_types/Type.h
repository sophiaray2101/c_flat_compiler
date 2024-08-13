#ifndef TYPE_H
#define TYPE_H

#include <string>
#include <vector>
#include <memory>

using namespace std;

// Type
struct Type {
    virtual ~Type() {}
    virtual string to_string() const = 0;
    virtual string get_type() const = 0;
    virtual string print_type() const = 0;
};
struct IntType : Type {
    string to_string() const override;
    string get_type() const override;
    string print_type() const override;
};
struct StructType : Type {
    string name;

    StructType();
    StructType(const string name);

    string to_string() const override;
    string get_type() const override;
    string print_type() const override;
};
struct FnType : Type {
    vector<shared_ptr<Type>> prms;
    shared_ptr<Type> ret; 

    FnType();
    FnType(const vector<shared_ptr<Type>>& prms, shared_ptr<Type> ret);
    ~FnType() override;

    string to_string() const override;
    string get_type() const override;
    string print_type() const override;
};
struct PtrType : Type {
    shared_ptr<Type> ref;

    PtrType(shared_ptr<Type> ref);
    ~PtrType() override;

    string to_string() const override;
    string get_type() const override;
    string print_type() const override;
};
struct AnyType : Type {
    AnyType();
    string to_string() const override;
    string get_type() const override;
    string print_type() const override;
};

#endif