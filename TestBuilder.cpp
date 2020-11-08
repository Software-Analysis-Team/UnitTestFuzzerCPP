#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

#include <iostream>
#include <vector>

#include "test.h"
#include "test_main.h"

using namespace clang::tooling;
// using namespace llvm;
// using namespace clang;
using namespace clang::ast_matchers;

static llvm::cl::OptionCategory MyToolCategory("my-tool options");

namespace GenerateTest
{

class FunctionArgsPrinter : public MatchFinder::MatchCallback
{
public:
    virtual void run(const MatchFinder::MatchResult &Result) 
    {
        if (const clang::FunctionDecl *FD = Result.Nodes.getNodeAs<clang::FunctionDecl>("FunctionDecl"))
        {
            std::cout << FD->getReturnType().getAsString() << std::endl << FD->getNameInfo().getAsString() << " (" << std::endl;
            auto param = FD->parameters();
            bool first = 1;
            for(const auto &i: param)
            {
                if(!first)
                    std::cout << ", ";
                else
                    std::cout << "  ";
                first = 0;
                std::cout << i->getOriginalType().getAsString() << ' ' << i->getDeclName().getAsString();
                std::cout << std::endl;
            }

            std::cout << ")"  << std::endl << std::endl;
        }
    }
};



// using namespace std;

std::variant<PrimitiveType, PointerTo, InRange> generate_variant(const std::string &s)
{
    if(s[0] == '*')
    {
        auto type = s.substr(1, s.size());

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
        if(s == "char")
        {
            return PrimitiveType::CHAR;
        }
        else if(s == "int")
        {
            return PrimitiveType::INT;
        }
        else
        {
            assert(false);
        }
    }
}


std::vector<TestSignature> test_signatures;

class FunctionTestBuilder : public MatchFinder::MatchCallback
{
public:
    virtual void run(const MatchFinder::MatchResult &Result) 
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
};

std::vector<Test> tests;

void construct_tests()
{
    std::random_device rd;

    std::mt19937 generator(rd());


    for(const auto &i: test_signatures)
    {
        tests.emplace_back(Test::generate(generator, i.name, i));
    }   
}


[[nodiscard]] auto generate_tests_and_signatures(auto Tool)
{
    DeclarationMatcher Matcher = functionDecl().bind("FunctionDecl");

    

    // FunctionArgsPrinter Printer;
    FunctionTestBuilder Builder;
    MatchFinder Finder;

    Finder.addMatcher(Matcher, &Builder);

    assert(! Tool.run(newFrontendActionFactory(&Finder).get()));

    construct_tests();

    return std::pair{test_signatures, tests};
}

} // namespace GenerateTest


int main(int argc, char **argv) 
{
    const char **cargv = const_cast<const char **>(argv);

    CommonOptionsParser OptionsParser(argc, cargv, MyToolCategory);

    ClangTool Tool(OptionsParser.getCompilations(),
                   OptionsParser.getSourcePathList());


    auto [s, t] = GenerateTest::generate_tests_and_signatures(Tool);

    auto tst = t.at(0);
    auto sign = s.at(0);

    // std::cout << tst.print() << std::endl << std::endl;

    TestMain test_main(1, argv);

    test_main.fuzz(sign, 5).run();


    // // test_main.add(tst);
    // for(auto tst: t)
    //     test_main.add(tst);

    // test_main.run();

    

    return 0;
}