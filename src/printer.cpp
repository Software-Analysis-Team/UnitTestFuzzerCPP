#include "test.h"

struct TypePrinter {
    std::string s;

    void visitPrimitiveType(PrimitiveType x) {
        s = printPrimitiveType(x);
    }

    void visitPointerTo(const PointerTo& x) {
        s = printType(*x.type) + "*";
    }

    void visitInRange(const InRange& x) {
        s = printType(*x.type);
    }
};

std::string printType(const Type &type) {
    TypePrinter typePrinter;
    type.accept(typePrinter);
    return typePrinter.s;
}

struct ValuePrinter {
    std::string value, s;

    void visitPrimitiveType(PrimitiveType x) {
        s = "static_cast<" + printPrimitiveType(x) + ">(" + value + ")";
    }

    void visitPointerTo(const PointerTo& x) {
        s = "new " + printType(*x.type) + "{" + printValue(*x.type, value) + "}";
    }

    void visitInRange(const InRange& x) {
        s = printValue(*x.type, value);
    }
};

std::string printValue(const Type &type, const std::string &value) {
    ValuePrinter valuePrinter;
    valuePrinter.value = value;
    type.accept(valuePrinter);
    return valuePrinter.s;
}
