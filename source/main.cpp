#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "libcommandline/multi_option.h"
#include "libcommandline/single_option.h"
#include "libcommandline/parser.h"

#include "vbscript_preprocessor.h"

typedef struct
{
    NonNullSharedPtr<LibCommandLine::MultiOption> includeDirectories;
    NonNullSharedPtr<LibCommandLine::SingleOption> outputFile;
} CmdArgs;

int main(int argc, char const *argv[])
{
    using namespace LibCommandLine;
    CmdArgs cmdArgs{
        makeNonNullShared<MultiOption>('i'),
        makeNonNullShared<SingleOption>('o', Option::Necessity::Required)
    };
    Parser::addOption(cmdArgs.includeDirectories);
    Parser::addOption(cmdArgs.outputFile);
    Parser::expectOperands(Parser::ExpectedOperands::One);
    try
    {
        Parser::parse(argc, argv);
    }
    catch(std::runtime_error const &e)
    {
        std::cerr << e.what() << "\n";
        return 1;
    }

    try 
    {
        VbsPp::process(
            Parser::getOperands().front(), 
            cmdArgs.includeDirectories->getArguments(),
            cmdArgs.outputFile->getArgument());
    } 
    catch (std::runtime_error const &e) 
    {
        std::cerr << e.what() << "\n";
        return 1;
    }
    catch (std::exception const &e)
    {
        std::cerr << "Unknown exception: " << e.what() << "\n";
        return 1;
    }
}
