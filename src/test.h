#pragma once

#include <utility>
#include <vector>
#include <string>
#include <variant>
#include <optional>
#include <memory>

struct Type;

using PrimitiveInteger = long;
using PrimitiveIntegerU = unsigned long;

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
};

struct TestSignature;

struct Test
{
    std::string name;
    const TestSignature *signature;
    std::vector<Value> arguments;

    [[nodiscard]] std::string print() const;
    [[nodiscard]] std::string printFunctionCall() const;
};


struct TestSignature {
    std::string name;
    std::vector<Type::ptr> parameterTypes;
    Type::ptr returnType;
    std::string linkWith;

    [[nodiscard]] std::string print() const;
    [[nodiscard]] std::string printStruct(const std::string &structName) const;
    [[nodiscard]] std::string callSerialized(const std::string& args) const;
    [[nodiscard]] Value call(const std::vector<Value>& args) const;
    [[nodiscard]] std::vector<Test> fuzz(int nTests) const;

    TestSignature() = default;
    TestSignature(std::string name,
                  std::vector<Type::ptr> parameterTypes,
                  Type::ptr returnType,
                  std::string linkWith);
    TestSignature(TestSignature &&that) noexcept;
    TestSignature &operator=(TestSignature &&that) noexcept;
    ~TestSignature();
    void swap(TestSignature &that) noexcept;

private:
    mutable std::string pathToInvoker;
    [[nodiscard]] std::string printInvoker() const;
    [[nodiscard]] std::string getInvoker() const;

    [[nodiscard]] std::string printFuzzer(int nTests, int interval) const;
    [[nodiscard]] std::string runFuzzer(int nTests) const;
};
