#ifndef ALGORITHMSERVICE_OPTION_PARSER_H_
#define ALGORITHMSERVICE_OPTION_PARSER_H_

#include <string>

namespace algorithmservice
{
    static const char kCommandOptions[] = "h?vc:";
    static const char kMyHelp[] =
    "Usage: controlservice [options] [file]...\n"
    "Options:\n"
    "-h                    Display this information\n"
    "-v                    Display version\n"
    "-c <file>             Specify config file with absolute path\n";

struct CommandArgument
{
    std::string config_filename;
};

int parse_options(int argc, char* const argv[], CommandArgument& args);

} // namespace algorithmservice

#endif // ALGORITHMSERVICE_OPTION_PARSER_H_