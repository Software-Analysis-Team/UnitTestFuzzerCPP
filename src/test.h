#pragma once

#include <vector>
#include <string>
#include <variant>
#include <optional>
#include <memory>
#include <random>

struct Type;

enum class PrimitiveType {
    CHAR,
    INT
};

std::string printPrimitiveType(PrimitiveType type);

struct PointerTo {
    std::shared_ptr<Type> type;
};

struct Type {
    std::variant<PrimitiveType, PointerTo> type;

    [[nodiscard]] std::string print() const;
};

struct TestSignature {
    std::string name;
    std::vector<Type> parameterTypes;
    Type returnType;
};

struct Value {
    Type type;
    int value;

    [[nodiscard]] std::string print() const;

    template <class Generator>
    static Value generate(Generator& gen, const Type& type) {
        std::uniform_int_distribution<int> distribution;
        return Value { type, distribution(gen) };
    }
};

struct Test
{
    std::string name;
    TestSignature signature;
    std::vector<Value> arguments;

    // after test launch
    std::optional<Value> returnValue;

    [[nodiscard]] std::string resultMacroName() const;
    void print(std::ostream &) const;
    void printPreludeGenerator(std::ostream &) const;
    void printFunctionCall(std::ostream &) const;

    template <class Generator>
    static Test generate(Generator gen, std::string name, TestSignature signature) {
        Test test;
        test.name = std::move(name);
        test.signature = std::move(signature);

        for (const auto &type : test.signature.parameterTypes) {
            test.arguments.push_back(Value::generate(gen, type));
        }

        return test;
    }
};
