#include <fstream>
#include <iostream>
#include <string>

#include "libcommandline/multi_option.h"
#include "libcommandline/single_option.h"
#include "libcommandline/parser.h"

typedef struct
{
    NonNullSharedPtr<LibCommandLine::MultiOption> includeDirectories;
    NonNullSharedPtr<LibCommandLine::SingleOption> outputFile;
} CmdArgs;

bool checkOperands();
void process(
    std::string_view sourcePath, 
    std::vector<std::string_view> const &includeDirectories,
    std::string_view outputPath);

int main(int argc, char const *argv[])
{
    using namespace LibCommandLine;
    CmdArgs cmdArgs{
        makeNonNullShared<MultiOption>('i'),
        makeNonNullShared<SingleOption>('o')
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
        std::cerr << e.what();
        return 1;
    }

    process(
        Parser::getOperands().front(), 
        cmdArgs.includeDirectories->getArguments(),
        cmdArgs.outputFile->getArgument());
}

void process(
    std::string_view sourcePath, 
    std::vector<std::string_view> const &includeDirectories,
    std::string_view outputPath)
{
    std::ifstream sourceFile{sourcePath.data()};
    std::ofstream outputFile{outputPath.data(), std::ofstream::out};

    if (!sourceFile)
    {
        std::cerr << "Failed to open source file \"" << sourcePath << "\"\n";
        return;
    }
    if (!outputFile)
    {
        std::cerr << "Failed to open output file \"" << outputPath << "\"\n";
        return;
    }

    std::string originalLine;
    std::string_view line;
    while (std::getline(sourceFile, originalLine))
    {
        if (originalLine.empty())
        {
            continue;
        }
        line = originalLine;
        line = line.substr(line.find_first_not_of(" \t"));
        if (line.at(0) == '#')
        {
            // TODO
        }
        else
        {
            outputFile << originalLine << "\n";
        }
    }
}
