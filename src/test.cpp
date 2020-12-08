#include "test.h"
#include "subprocess.h"

#include <sstream>
#include <utility>
#include <unistd.h>
#include <iostream>

std::string Value::print() const {
    return printValue(*type, value);
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

std::string Test::print() const {
    std::stringstream ss;
    auto call = printFunctionCall();
    ss << "TEST(" << signature->name << "Test, " << name << ") {\n";
    ss << "    ASSERT_EQ(" << call;
    ss << ", " << signature->call(arguments).print() << ");\n";
    ss << "}\n";
    return ss.str();
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

std::string TestSignature::printInvoker() const {
    std::string res = "#include<iostream>\n";
    res += print();
    res += "int main() {\n";

    int i = 0;
    Test test;
    test.signature = this;

    for (const auto& param : parameterTypes) {
        std::string paramName = "arg" + std::to_string(i);
        res += "long " + paramName + "; std::cin >> " + paramName + ";\n";
        test.arguments.push_back(Value { param, paramName });
    }

    res += "auto retval = " + test.printFunctionCall() + ";\n";
    res += printValueSerializer(*returnType, "retval");
    res += "return 0;\n}\n";

    return res;
}

std::string TestSignature::getInvoker() const {
    if (!pathToInvoker.empty()) {
        return pathToInvoker;
    }

    std::string path = "/tmp/UnitTestFuzzerInvoker_" + name;

    Subprocess compiler{{"clang++", "-std=c++17", "-o", path, linkWith, "-x", "c++", "-"}};
    if (!compiler.run(printInvoker())) {
        throw std::runtime_error("Failed to compile invoker");
    }

    return pathToInvoker = path;
}

std::string TestSignature::callSerialized(const std::string& args) const {
    std::string path = getInvoker();
    Subprocess prog{{path}};

    if (!prog.run(args)) {
        throw std::runtime_error("Failed to run invoker");
    }

    return prog.output();
}

Value TestSignature::call(const std::vector<Value>& args) const {
    std::string serialized;
    for (const auto &val : args) {
        serialized += val.value;
        serialized += '\n';
    }

    auto ret = callSerialized(serialized);
    return Value { returnType, ret };
}

TestSignature::TestSignature(TestSignature &&that) noexcept {
    swap(that);
}

TestSignature &TestSignature::operator=(TestSignature &&that) noexcept {
    swap(that);
    return *this;
}

void TestSignature::swap(TestSignature &that) noexcept {
    std::swap(name, that.name);
    std::swap(parameterTypes, that.parameterTypes);
    std::swap(returnType, that.returnType);
    std::swap(linkWith, that.linkWith);
    std::swap(pathToInvoker, that.pathToInvoker);
}

TestSignature::~TestSignature() {
    if (!pathToInvoker.empty()) {
        unlink(pathToInvoker.c_str());
    }
}

TestSignature::TestSignature(std::string name,
                             std::vector<Type::ptr> parameterTypes,
                             Type::ptr returnType,
                             std::string linkWith) :
     name(std::move(name)),
     parameterTypes(std::move(parameterTypes)),
     returnType(std::move(returnType)),
     linkWith(std::move(linkWith))
{}

std::string TestSignature::printStruct(const std::string &structName) const {
    std::stringstream ss;

    ss << "struct " << structName << " {\n";

    int i = 0;
    for (const auto& param : parameterTypes) {
        std::string paramName = "arg" + std::to_string(i++);
        ss << "long " << paramName << ";\n";
    }

    ss << "};\n";

    return ss.str();
}

std::string TestSignature::printFuzzer(int nTests, int interval) const {
    std::stringstream ss;

    ss << "#include <cstring>\n";
    ss << "#include <cstdlib>\n";
    ss << "#include <iostream>\n";
    ss << print();
    ss << printStruct("FuzzArgs");
    ss << "extern \"C\" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {\n";
    ss << "static long cnt, tests;\n";
    ss << "FuzzArgs args;\n";
    ss << "if (size != sizeof(args)) return 0;\n";
    ss << "memcpy(&args, data, size);\n";

    int i = 0;
    Test test;
    test.signature = this;

    for (const auto& param : parameterTypes) {
        std::string paramName = "args.arg" + std::to_string(i);
        test.arguments.push_back(Value { param, paramName });

        auto [lo, hi] = param->getRange();
        if (lo != std::numeric_limits<PrimitiveInteger>::min() || hi != std::numeric_limits<PrimitiveInteger>::max()) {
            PrimitiveIntegerU domain = 1 + hi - lo;
            ss << paramName << " = " << "(" << paramName << " % " << domain << " + " << domain << ") % ";
            ss << domain << " + " << lo << ";\n";
        }
    }

    ss << test.printFunctionCall() << ";\n";
    ss << "if (++cnt % " << interval << " == 0) {\n";
    i = 0;
    for (const auto& param : parameterTypes) {
        std::string paramName = "args.arg" + std::to_string(i);
        ss << "std::cout << \"test \" << " << paramName << " << ' ';\n";
    }
    ss << "std::cout << std::endl;\n";
    ss << "if (++tests >= " << nTests << ") {\n";
    ss << "std::cout << \"exit\" << std::endl;\n";
    ss << "exit(0);\n";
    ss << "}\n";
    ss << "}\n";
    ss << "return 0;\n";
    ss << "}\n";

    std::cerr << ss.str();

    return ss.str();
}

std::string TestSignature::runFuzzer(int nTests) const {
    std::string path = "/tmp/UnitTestFuzzerFuzzer_" + name;

    Subprocess compiler{{"clang++", "-g", "-fsanitize=address,fuzzer", "-std=c++17", "-o", path, linkWith, "-x", "c++", "-"}};
    if (!compiler.run(printFuzzer(nTests, 100))) {
        throw std::runtime_error("Failed to compile fuzzer");
    }

    Subprocess fuzzer{{path}};
    auto res = fuzzer.run("").output();
    unlink(path.c_str());
    return res;
}
