#pragma once

#include <string_view>
#include <vector>

namespace VbsPp
{

void process(
    std::string_view sourcePath, 
    std::vector<std::string_view> const &includeDirectories,
    std::string_view outputPath);
void includeFile(
    std::string_view includePath,
    std::vector<std::string_view> const &includeDirectories,
    std::ofstream &outputFile);

}