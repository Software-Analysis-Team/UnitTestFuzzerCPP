#include "PrintMatcher.hpp"
#include "TestBuilder.hpp"
#include "StructParser.hpp"
#include "test.h"
#include "test_main.h"

static llvm::cl::OptionCategory MyToolCategory("my-tool options");

void generate_tests(int argc, char **argv)
{
    const char **cargv = const_cast<const char **>(argv);

    CommonOptionsParser OptionsParser(argc, cargv, MyToolCategory);

    ClangTool Tool(OptionsParser.getCompilations(),
                   OptionsParser.getSourcePathList());

    DeclarationMatcher Matcher = functionDecl().bind("FunctionDecl");

    auto [t, s] = TestBuilder::get_tests_and_signatures(Tool, Matcher);

    auto sign = s.at(0);

    TestMain test_main(1, argv);
    test_main.fuzz(sign, 5).run();
}

void test_struct_parsing(int argc, char **argv)
{
    const char **cargv = const_cast<const char **>(argv);

    CommonOptionsParser OptionsParser(argc, cargv, MyToolCategory);

    ClangTool Tool(OptionsParser.getCompilations(),
                   OptionsParser.getSourcePathList());

    DeclarationMatcher Matcher = recordDecl(isStruct()).bind("StructDecl");

    StructParser::print_structures(Tool, Matcher);
}

int main(int argc, char **argv) 
{

    test_struct_parsing(argc, argv);

    return 0;
}