#include "test.h"

#include <sstream>

std::string Value::print() const {
    return "static_cast<" + type.print() + ">(" + std::to_string(value) + ")";
}
std::string Type::print() const {
    if (auto primitiveType = std::get_if<PrimitiveType>(&type)) {
        return printPrimitiveType(*primitiveType);
    } else if (auto pointerTo = std::get_if<PointerTo>(&type)) {
        return pointerTo->type->print() + "*";
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

std::string Test::print() const {
    std::stringstream ss;
    ss << "TEST(" << signature.name << "Test, " << name << ") {\n";
    ss << "    " << signature.name << "(";

    bool first = true;

    for (const auto &arg : arguments) {
        if (!first) {
            ss << ", ";
        }

        ss << arg.print();

        first = false;
    }
    ss << ");\n";
    ss << "}\n";
    return ss.str();
}
