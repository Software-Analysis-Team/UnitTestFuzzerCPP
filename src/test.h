#pragma once

#include <utility>
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

    [[nodiscard]] std::pair<PrimitiveInteger, PrimitiveInteger> getRange() const;

    template<class Visitor>
    void accept(Visitor &visitor) const {
        if (auto primitiveType = std::get_if<PrimitiveType>(&type)) {
            visitor.visitPrimitiveType(*primitiveType);
        } else if (auto pointerTo = std::get_if<PointerTo>(&type)) {
            visitor.visitPointerTo(*pointerTo);
        } else if (auto inRange = std::get_if<InRange>(&type)) {
            visitor.visitInRange(*inRange);
        }
    }
};

std::string printType(const Type &type);
std::string printValue(const Type &type, const std::string &value);
std::string printValueSerializer(const Type &type, const std::string &value);

[[nodiscard]] Type::ptr primitiveType(PrimitiveType type);
[[nodiscard]] Type::ptr primitiveType(PrimitiveType type, PrimitiveInteger min, PrimitiveInteger max);
[[nodiscard]] Type::ptr pointerTo(Type::ptr type);

struct Value {
    Type::ptr type;
    std::string value;

    [[nodiscard]] std::string print() const;

    template <class Generator>
    static Value generate(Generator& gen, const Type::ptr& type) {
        auto [a, b] = type->getRange();
        std::uniform_int_distribution<PrimitiveInteger> distribution{a, b};
        return Value { type, std::to_string(distribution(gen)) };
    }
};

struct TestSignature {
    std::string name;
    std::vector<Type::ptr> parameterTypes;
    Type::ptr returnType;
    std::string linkWith;

    mutable std::string pathToInvoker;

    [[nodiscard]] std::string print() const;
    [[nodiscard]] std::string printInvoker() const;
    [[nodiscard]] std::string getInvoker() const;
    [[nodiscard]] std::string callSerialized(const std::string& args) const;
    [[nodiscard]] Value call(const std::vector<Value>& args) const;

    TestSignature() = default;
    TestSignature(std::string name,
                  std::vector<Type::ptr> parameterTypes,
                  Type::ptr returnType,
                  std::string linkWith);
    TestSignature(TestSignature &&that) noexcept;
    TestSignature &operator=(TestSignature &&that) noexcept;
    ~TestSignature();
    void swap(TestSignature &that) noexcept;
};


struct Test
{
    std::string name;
    const TestSignature *signature;
    std::vector<Value> arguments;

    // after test launch
    std::optional<Value> returnValue;

    [[nodiscard]] std::string print() const;
    [[nodiscard]] std::string printFunctionCall() const;

    template <class Generator>
    static Test generate(Generator &gen, std::string name, TestSignature *signature) {
        Test test;
        test.name = std::move(name);
        test.signature = signature;

        for (const auto &type : test.signature->parameterTypes) {
            test.arguments.push_back(Value::generate(gen, type));
        }

        return test;
    }
};
