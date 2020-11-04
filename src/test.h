#pragma once

#include <vector>
#include <string>
#include <variant>
#include <optional>
#include <memory>
#include <random>

struct Type;

using PrimitiveInteger = long;

enum class PrimitiveType {
    CHAR,
    INT
};

[[nodiscard]] std::string printPrimitiveType(PrimitiveType type);
[[nodiscard]] int primitiveTypeSize(PrimitiveType type);

struct PointerTo {
    std::shared_ptr<Type> type;
};

struct InRange {
    std::shared_ptr<Type> type;
    PrimitiveInteger min, max;
};

struct Type {
    std::variant<
            PrimitiveType,
            PointerTo,
            InRange
    > type;

    using ptr = std::shared_ptr<Type>;

    [[nodiscard]] std::string print() const;
    [[nodiscard]] std::string printValue(const std::string &value) const;
    [[nodiscard]] std::pair<PrimitiveInteger, PrimitiveInteger> getRange() const;
};

[[nodiscard]] Type::ptr primitiveType(PrimitiveType type);
[[nodiscard]] Type::ptr primitiveType(PrimitiveType type, PrimitiveInteger min, PrimitiveInteger max);
[[nodiscard]] Type::ptr pointerTo(Type::ptr type);

struct TestSignature {
    std::string name;
    std::vector<Type::ptr> parameterTypes;
    Type::ptr returnType;

    [[nodiscard]] std::string print() const;
};

struct Value {
    Type::ptr type;
    PrimitiveInteger value;

    [[nodiscard]] std::string print() const;

    template <class Generator>
    static Value generate(Generator& gen, const Type::ptr& type) {
        auto [a, b] = type->getRange();
        std::uniform_int_distribution<PrimitiveInteger> distribution{a, b};
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

    [[nodiscard]] std::string print() const;
    [[nodiscard]] std::string printGenerator() const;
    [[nodiscard]] std::string printFunctionCall() const;

    template <class Generator>
    static Test generate(Generator &gen, std::string name, TestSignature signature) {
        Test test;
        test.name = std::move(name);
        test.signature = std::move(signature);

        for (const auto &type : test.signature.parameterTypes) {
            test.arguments.push_back(Value::generate(gen, type));
        }

        return test;
    }
};

std::string quote(const std::string& s);
