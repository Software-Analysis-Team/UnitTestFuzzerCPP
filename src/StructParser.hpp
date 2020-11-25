#pragma once

#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

#include "PrintMatcher.hpp"

using namespace clang::tooling;
// using namespace llvm;
// using namespace clang;
using namespace clang::ast_matchers;

namespace StructParser
{
auto print_structures(auto Tool, auto Matcher)
{
    PrintMatcher::StructPrinter Printer;
    MatchFinder Finder;

    Finder.addMatcher(Matcher, &Printer);

    assert(! Tool.run(newFrontendActionFactory(&Finder).get()));
}
} //namespace StructParser