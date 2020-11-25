#pragma once

#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::tooling;
// using namespace llvm;
// using namespace clang;
using namespace clang::ast_matchers;

namespace PrintMatcher
{

class FunctionArgsPrinter : public MatchFinder::MatchCallback
{
public:
    virtual void run(const MatchFinder::MatchResult &Result);
};

class StructPrinter : public MatchFinder::MatchCallback
{
public:
    virtual void run(const MatchFinder::MatchResult &Result);
};

template <class T, class M, class P>
void print_matched(T Tool, M Matcher, P Printer)
{
    MatchFinder Finder;

    Finder.addMatcher(Matcher, &Printer);

    assert(! Tool.run(newFrontendActionFactory(&Finder).get()));
}

} // namespace PrintMatcher