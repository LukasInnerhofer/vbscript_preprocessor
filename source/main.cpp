#include <iostream>

#include "libcommandline/multi_option.h"
#include "libcommandline/parser.h"

typedef struct
{
    NonNullSharedPtr<LibCommandLine::MultiOption> includeDirectories;
} CmdArgs;

int main(int argc, char const *argv[])
{
    CmdArgs cmdArgs{
        makeNonNullShared<LibCommandLine::MultiOption>('i')
    };
    LibCommandLine::Parser::addOption(cmdArgs.includeDirectories);
    LibCommandLine::Parser::parse(argc, argv);
}
