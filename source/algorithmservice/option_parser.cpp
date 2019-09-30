#include <algorithmservice/option_parser.h>
#include <algorithmservice/version.h>
#include <unistd.h>
namespace algorithmservice
{

int parse_options(int argc, char *const argv[], CommandArgument &args)
{
    int opt = -1;
    int opt_number = 0;
    do
    {
        opt = ::getopt(argc, argv, kCommandOptions);
        switch (opt)
        {
        case 'v':
            printf("%s version:%s %s\n", algorithmservice::kFileName,
                   algorithmservice::kProductVersion, algorithmservice::kFileVersion);
            printf("%s\n", kCopyright);
            return 0;
        case 'c':
            args.config_filename = std::string(optarg);
            opt_number++;
            break;
        case 'h':
        case '?':
        default:
        {
            if (opt_number <= 0)
            {
                printf("%s\n", kMyHelp);
                return 0;
            }
            break;
        }
        }
    } while (opt != -1);
    return opt_number;
}

} // namespace algorithmservice