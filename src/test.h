#pragma once

#include <vector>
#include <string>
#include <any>
#include <optional>
#include <memory>

enum class Kind {
    PRIMITIVE_TYPE,
    POINTER
};

enum class PrimitiveType {
    CHAR,
    INT
};

std::string printPrimitiveType(PrimitiveType type);

struct Type {
    union {
        PrimitiveType primitiveType;
        std::unique_ptr<Type> pointerTo;
    };

    Kind kind;

    [[nodiscard]] std::string print() const;
};

std::any fillValue(Type x);

struct TestSignature {
    std::string name;
    std::vector<Type> parameterTypes;
    Type returnType;
};

struct Value {
    Type type;
    int value;
    [[nodiscard]] std::string print() const;
};

struct Test
{
    std::string name;
    TestSignature signature;
    std::vector<Value> arguments;

    // after test launch
    Value returnValue;

    [[nodiscard]] std::string print() const;
};
