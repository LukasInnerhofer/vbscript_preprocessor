#include <fstream>
#include <filesystem>
#include <iostream>
#include <string>

#include "vbscript_preprocessor.h"

namespace VbsPp
{

constexpr char const *spaceAndTab{" \t"};

std::vector<std::filesystem::path> includedFiles;

bool processInternal(
    std::string_view sourcePath, 
    std::vector<std::string_view> const &includeDirectories,
    std::string_view outputPath);

void process(
    std::string_view sourcePath, 
    std::vector<std::string_view> const &includeDirectories,
    std::string_view outputPath)
{
    std::string tempFileName{outputPath};
    tempFileName += ".temp";
    
    if (processInternal(sourcePath, includeDirectories, outputPath))
    {
        do
        {
            std::filesystem::rename(outputPath, tempFileName);
        } while (processInternal(tempFileName, includeDirectories, outputPath));
        std::filesystem::rename(tempFileName, outputPath);
    }
}

bool processInternal(
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
    std::string_view::size_type offset;
    bool preprocessLine;
    bool preprocessedFile{false};
    while (std::getline(sourceFile, originalLine))
    {
        preprocessLine = false;
        if (!originalLine.empty())
        {
            line = originalLine;
            offset = line.find_first_not_of(spaceAndTab);
            if (offset < line.size())
            {
                line = line.substr();
                if (line.at(0) == '#')
                {
                    preprocessLine = true;
                    preprocessedFile = true;
                    directive = line.substr(1, line.find_first_of(spaceAndTab) - 1);
                    
                    if (directive == "include")
                    {
                        line = line.substr(line.find_first_of(spaceAndTab));
                        line = line.substr(line.find_first_not_of(spaceAndTab));
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
            
        }
        
        if (!preprocessLine)
        {
            outputFile << originalLine << "\n";
        }
    }
    return preprocessedFile;
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
        includeFilePath = std::filesystem::path{directory} / includeFileName;

        if (std::filesystem::exists(includeFilePath))
        {
            if (std::find(includedFiles.begin(), includedFiles.end(), includeFilePath) != includedFiles.end())
            {
                return;
            }
            includedFiles.push_back(includeFilePath);
            includeFile.open(includeFilePath);
            while(std::getline(includeFile, line))
            {
                outputFile << line << "\n";
            }
            std::cout << "Done.\n";
            return;
        }
    }
    throw std::runtime_error{"Cannot find include file: " + std::string{includeFileName}};
}

}