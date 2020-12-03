#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

#include <vector>

#include "test.h"
#include "test_main.h"
#include "TestBuilder.hpp"

using namespace clang::tooling;
// using namespace llvm;
// using namespace clang;
using namespace clang::ast_matchers;

namespace TestBuilder
{

std::variant<PrimitiveType, PointerTo, InRange> generate_variant(const std::string &type_name)
{
    if(type_name[0] == '*')
    {
        auto type = type_name.substr(1, type_name.size());

        if(type == "char")
        {
            return PointerTo{std::make_shared<Type>(Type{std::variant<PrimitiveType, PointerTo, InRange>(PrimitiveType::CHAR)})};
        }
        else if(type == "int")
        {
            return PointerTo{std::make_shared<Type>(Type{std::variant<PrimitiveType, PointerTo, InRange>(PrimitiveType::INT)})};
        }
        else
        {
            assert(false);
        }
    }
    else
    {
        if(type_name == "char")
        {
            return PrimitiveType::CHAR;
        }
        else if(type_name == "int")
        {
            return PrimitiveType::INT;
        }
        else
        {
            assert(false);
        }
    }
}

void FunctionTestBuilder::run(const MatchFinder::MatchResult &Result) 
{
    if (const clang::FunctionDecl *FD = Result.Nodes.getNodeAs<clang::FunctionDecl>("FunctionDecl"))
    {
        auto name = FD->getNameInfo().getAsString();
        auto return_type = std::make_shared<Type>(Type{generate_variant(FD->getReturnType().getAsString())});
        std::vector<Type::ptr> argument_types;

        auto param = FD->parameters();
        for(const auto &i: param)
        {
            argument_types.emplace_back(std::make_shared<Type>(Type{generate_variant(i->getOriginalType().getAsString())}));
        }

        test_signatures.emplace_back(TestSignature{name, argument_types, return_type});
    }
}

void FunctionTestBuilder::construct_tests()
{
    assert(!is_test_constructed);
    is_test_constructed = true;

    std::random_device rd;
    std::mt19937 generator(rd());
    for(const auto &i: test_signatures)
    {
        tests.emplace_back(Test::generate(generator, i.name, i));
    }  
}

[[nodiscard]] std::pair<std::vector<Test>, std::vector<TestSignature>> FunctionTestBuilder::get_tests_and_signatures()
{
    return std::pair{tests, test_signatures};
}

} // namespace TestBuilder
