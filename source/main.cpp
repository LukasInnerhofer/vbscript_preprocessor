#include <filesystem>
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
void includeFile(
    std::string_view includePath,
    std::vector<std::string_view> const &includeDirectories,
    std::ofstream &outputFile);

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

    try 
    {
        process(
            Parser::getOperands().front(), 
            cmdArgs.includeDirectories->getArguments(),
            cmdArgs.outputFile->getArgument());
    } 
    catch (std::runtime_error const &e) 
    {
        std::cerr << e.what();
        return 1;
    }
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
        throw std::runtime_error{"Failed to open source file \"" + std::string{sourcePath} + "\"\n"};
    }
    if (!outputFile)
    {
        throw std::runtime_error{"Failed to open output file \"" + std::string{outputPath} + "\"\n"};
    }

    std::string originalLine;
    std::string_view line;
    std::string_view directive;
    bool preprocess;
    while (std::getline(sourceFile, originalLine))
    {
        preprocess = false;
        if (!originalLine.empty())
        {
            line = originalLine;
            line = line.substr(line.find_first_not_of(" \t"));
            if (line.at(0) == '#')
            {
                preprocess = true;
                directive = line.substr(1, line.find_first_of(" \t") - 1);
                
                if(directive == "include")
                {
                    line = line.substr(line.find_first_of(" \t"));
                    line = line.substr(line.find_first_not_of(" \t"));
                    includeFile(
                        line,
                        includeDirectories,
                        outputFile);
                }
                else 
                {
                    std::cout << "Unsupported directive \"" << directive << "\"\n";    
                }
            }
        }
        
        if (!preprocess)
        {
            outputFile << originalLine << "\n";
        }
    }
}

void includeFile(
    std::string_view includeFileName,
    std::vector<std::string_view> const &includeDirectories,
    std::ofstream &outputFile)
{
    std::filesystem::path includeFilePath;
    std::ifstream includeFile;
    std::string line;

    for (auto directory : includeDirectories)
    {
        includeFilePath = std::filesystem::path{directory};
        includeFilePath /= includeFileName;
        if (std::filesystem::exists(includeFilePath))
        {
            includeFile.open(includeFilePath);
            while(std::getline(includeFile, line))
            {
                outputFile << line << "\n";
            }
            return;
        }
    }
    throw std::runtime_error{"Cannot find include file \"" + std::string{includeFileName} + "\""};
}
