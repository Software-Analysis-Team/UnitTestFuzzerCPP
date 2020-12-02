#include "test.h"

#include <sstream>

std::string Value::print() const {
    return printValue(*type, std::to_string(value));
}

std::pair<PrimitiveInteger, PrimitiveInteger> Type::getRange() const {
    if (auto primitiveType = std::get_if<PrimitiveType>(&type)) {
        int bitness = primitiveTypeSize(*primitiveType);
        return { -(1ll << bitness), (1ll << bitness) - 1 };
    } else if (auto pointerTo = std::get_if<PointerTo>(&type)) {
        return pointerTo->type->getRange();
    } else if (auto inRange = std::get_if<InRange>(&type)) {
        auto enclosingRange = inRange->type->getRange();
        return { std::max(enclosingRange.first, inRange->min),
                 std::min(enclosingRange.second, inRange->max) };
    }

    return { 0, 0 };
}

std::string quote(const std::string& s) {
    std::stringstream ss;

    ss << '"';
    bool inEscape = false;

    for (char c : s) {
        if (c == '\n') ss << "\\n";
        else if (c == '\t') ss << "\\t";
        else if (c == '"') ss << "\\\"";
        else if (c == '\0') {
            inEscape = !inEscape;
            if (inEscape) {
                ss << "\" << ";
            } else {
                ss << " << \"";
            }
        } else ss << c;
    }

    ss << '"';

    return ss.str();
}

std::string Test::print() const {
    std::stringstream ss;
    auto call = printFunctionCall();
    ss << signature->print() << "\n";
    ss << "TEST(" << signature->name << "Test, " << name << ") {\n";
    ss << "    ASSERT_EQ(" << call;
    ss << ", " << signature->returnType->printValue('\0' + call + '\0') << ");\n";
    ss << "}\n";
    return ss.str();
}

std::string Test::printGenerator() const {
    return "std::cout << " + quote(print()) + ";\n";
}

std::string Test::printFunctionCall() const {
    std::string res;
    res += signature->name + "(";

    bool first = true;

    for (const auto &arg : arguments) {
        if (!first) {
            res += ", ";
        }
        res += arg.print();
        first = false;
    }

    res += ")";
    return res;
}

std::string printPrimitiveType(PrimitiveType type) {
    switch (type) {
        case PrimitiveType::CHAR: return "char";
        case PrimitiveType::INT: return "int";
    }
    return "<unknown>";
}

int primitiveTypeSize(PrimitiveType type) {
    switch (type) {
        case PrimitiveType::CHAR: return 8;
        case PrimitiveType::INT: return 32;
    }
    return 0;
}

Type::ptr primitiveType(PrimitiveType type) {
    return std::make_shared<Type>(Type { type });
}

Type::ptr primitiveType(PrimitiveType type, PrimitiveInteger min, PrimitiveInteger max) {
    return std::make_shared<Type>( Type { InRange { primitiveType(type), min, max } } );
}

Type::ptr pointerTo(Type::ptr type) {
    return std::make_shared<Type>(Type { PointerTo { std::move(type) } } );
}

std::string TestSignature::print() const {
    std::string res = printType(*returnType) + " " + name;
    res += "(";

    bool first = true;

    for (const auto &type : parameterTypes) {
        if (!first) {
            res += ", ";
        }
        first = false;
        res += printType(*type);
    }

    res += ");\n";
    return res;
}

std::string TestSignature::printInteractor() const {

}
