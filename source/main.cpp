#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "libcommandline/parser.h"
#include "libcommandline/multi_option.h"
#include "libcommandline/single_option.h"

#include "vbscript_preprocessor.h"

struct CmdArgs
{
    NonNullSharedPtr<LibCommandLine::MultiOption> includeDirectories;
    NonNullSharedPtr<LibCommandLine::SingleOption> outputFile;
};

int main(int argc, char const *argv[])
{
    using namespace LibCommandLine;
    CmdArgs cmdArgs{
        makeNonNullShared<MultiOption>('i', "include directory"),
        makeNonNullShared<SingleOption>('o', "output file", Option::Necessity::Required)
    };
    Parser::addOption(cmdArgs.includeDirectories);
    Parser::addOption(cmdArgs.outputFile);
    Parser::expectOperands(Parser::ExpectedOperands::One);
    try
    {
        Parser::parse(argc, argv);
    }
    catch (std::runtime_error const &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        Parser::printHelp(std::cout);
        return 1;
    }

    if (*Parser::getHelpFlag())
    {
        return 0;
    }

    try 
    {
        VbsPp::process(
            Parser::getOperands()->front(), 
            cmdArgs.includeDirectories->getArguments(),
            cmdArgs.outputFile->getArgument());
    } 
    catch (std::runtime_error const &e) 
    {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    catch (std::exception const &e)
    {
        std::cerr << "Unknown exception: " << e.what() << "\n";
        return 1;
    }
}
