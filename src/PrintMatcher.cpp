#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

#include <iostream>

#include "PrintMatcher.hpp"

namespace PrintMatcher
{

void FunctionArgsPrinter::run(const MatchFinder::MatchResult &Result) 
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

void StructPrinter::run(const MatchFinder::MatchResult &Result) 
{
    if (const clang::RecordDecl *FD = Result.Nodes.getNodeAs<clang::RecordDecl>("StructDecl"))
    {
        std::cout << FD->getNameAsString() << std::endl;
        auto first_member = FD->findFirstNamedDataMember();

        std::cout << "{\n";

        auto f = FD->fields();

        for(auto i: f)
        {
            std::cout << "    " << i->getType().getAsString() << ' ' << i->getNameAsString() << std::endl;
        }

        std::cout << "}";

        std::cout << std::endl;
        std::cout << std::endl;

    }
}

} //namespace PrintMatcher
