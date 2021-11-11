#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "libcommandline/parser.h"
#include "libcommandline/multi_option.h"
#include "libcommandline/single_option.h"

#include "vbscript_preprocessor.h"

namespace CmdArgs
{
    using namespace LibCommandLine;
    auto includeDirectories{
        makeNonNullShared<MultiOption>('i', "include directory")
    };
    auto outputFile{
        makeNonNullShared<SingleOption>('o', "output file", Option::Necessity::Required)
    };
}

int main(int argc, char const *argv[])
{
    using namespace LibCommandLine;
    Parser parser{
        {CmdArgs::includeDirectories, CmdArgs::outputFile}, 
        Parser::ExpectedOperands::One, 
        "input file"};
    try
    {
        parser.parse(argc, argv);
    }
    catch (std::runtime_error const &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        parser.printHelp(std::cout);
        return 1;
    }

    if (*parser.getHelpFlag())
    {
        return 0;
    }

    try 
    {
        VbsPp::process(
            parser.getOperands()->front(), 
            *CmdArgs::includeDirectories->getArguments(),
            CmdArgs::outputFile->getArgument());
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
