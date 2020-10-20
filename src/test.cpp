#include "test.h"

#include <sstream>

std::string Value::print() const {
    return "static_cast<" + type.print() + ">(" + std::to_string(value) + ")";
}

std::string Type::print() const {
    switch (kind) {
        case Kind::PRIMITIVE_TYPE: return printPrimitiveType(primitiveType);
        case Kind::POINTER: return pointerTo->print() + "*";
    }
}

std::string printPrimitiveType(PrimitiveType type) {
    switch (type) {
        case PrimitiveType::CHAR: return "char";
        case PrimitiveType::INT: return "int";
    }
}

std::string Test::print() const {
    std::stringstream ss;
    ss << "TEST(" << signature.name << "Test, " << name << ") {\n";
    ss << "    EXPECT_EQ(" << signature.name << "(";

    bool first = true;

    for (const auto &arg : arguments) {
        if (!first) {
            ss << ", ";
        }

        ss << arg.print();

        first = false;
    }
    ss << "), " << returnValue.print() << ");\n";
    ss << "}\n";
    return ss.str();
}
