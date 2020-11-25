#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

#include <vector>
#include <cassert>
#include <random>

#include "test.h"
#include "test_main.h"

using namespace clang::tooling;
// using namespace llvm;
// using namespace clang;
using namespace clang::ast_matchers;

namespace TestBuilder
{

std::variant<PrimitiveType, PointerTo, InRange> generate_variant(const std::string &type_name);

class FunctionTestBuilder : public MatchFinder::MatchCallback
{
private:
    std::vector<TestSignature> test_signatures;
    std::vector<Test> tests;
    bool is_test_constructed = false;

public:
    virtual void run(const MatchFinder::MatchResult &Result);

    void construct_tests();

    [[nodiscard]] std::pair<std::vector<Test>, std::vector<TestSignature>> get_tests_and_signatures();

};

[[nodiscard]] auto get_tests_and_signatures(auto Tool, auto Matcher)
{
    FunctionTestBuilder Builder;
    MatchFinder Finder;

    Finder.addMatcher(Matcher, &Builder);

    assert(! Tool.run(newFrontendActionFactory(&Finder).get()));

    Builder.construct_tests();

    return Builder.get_tests_and_signatures();
}


} //namespace TestBuilder
