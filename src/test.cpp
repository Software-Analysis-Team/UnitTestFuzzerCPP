#include "test.h"

#include <sstream>

std::string Value::print() const {
    return type.printValue(std::to_string(value));
}

std::string Type::print() const {
    if (auto primitiveType = std::get_if<PrimitiveType>(&type)) {
        return printPrimitiveType(*primitiveType);
    } else if (auto pointerTo = std::get_if<PointerTo>(&type)) {
        return pointerTo->type->print() + "*";
    }

    return "<unknown>";
}

std::string Type::printValue(const std::string& value) const {
    if (auto primitiveType = std::get_if<PrimitiveType>(&type)) {
        return "static_cast<" + print() + ">(" + value + ")";
    } else if (auto pointerTo = std::get_if<PointerTo>(&type)) {
        return "new " + pointerTo->type->print() + "{" + pointerTo->type->printValue(value) + "}";
    }

    return "<unknown>";
}

std::string printPrimitiveType(PrimitiveType type) {
    switch (type) {
        case PrimitiveType::CHAR: return "char";
        case PrimitiveType::INT: return "int";
    }
    return "<unknown>";
}

void Test::print(std::ostream &ss) const {
    ss << "TEST(" << signature.name << "Test, " << name << ") {\n";
    ss << "    ASSERT_EQ(";
    printFunctionCall(ss);
    ss << ", " << signature.returnType.printValue(resultMacroName()) << ");\n";
    ss << "}\n";
}

std::string Test::resultMacroName() const {
    return "FUZZ_TEST_" + name;
}

void Test::printPreludeGenerator(std::ostream &ss) const {
    auto macro = resultMacroName();
    auto var = macro + "_result";
    ss << "auto " << var << " = ";
    printFunctionCall(ss);
    ss << ";\n";

    ss << "std::cout << \"#define " << macro << " \" << " << var << " << \"\\n\"";
}

void Test::printFunctionCall(std::ostream &ss) const {
    ss << signature.name << "(";

    bool first = true;

    for (const auto &arg : arguments) {
        if (!first) {
            ss << ", ";
        }

        ss << arg.print();

        first = false;
    }

    ss << ")";
}
