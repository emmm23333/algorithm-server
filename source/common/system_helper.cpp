#include <uuid/uuid.h>
#include <common/system_helper.h>
#include <ustd/string.h>
#include <string.h>
namespace algorithmservice
{
namespace systemhelper
{

std::string create_uuid()
{
    uuid_t uuid_hex;
    uuid_generate(uuid_hex);

    char value[37] = {0};
    snprintf(value, 37, "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
             uuid_hex[0], uuid_hex[1], uuid_hex[2],
             uuid_hex[3], uuid_hex[4], uuid_hex[5],
             uuid_hex[6], uuid_hex[7], uuid_hex[8],
             uuid_hex[9], uuid_hex[10], uuid_hex[11],
             uuid_hex[12], uuid_hex[13], uuid_hex[14],
             uuid_hex[15]);
    return ustd::to_lower_string(value);
}

void trim_blank_char(std::string &source)
{
    while (!source.empty())
    {
        char ch = source.back();
        if (ch == ' ' || ch == '\n' || ch == '\t' || ch == '\r')
        {
            source.pop_back();
        }
        else
        {
            break;
        }
    }

    while (!source.empty())
    {
        char ch = source.front();
        if (ch == ' ' || ch == '\n' || ch == '\t' || ch == '\r')
        {
            source = source.substr(1);
        }
        else
        {
            break;
        }
    }

    return;
}

bool execute_cmd(const std::string &cmd, std::vector<std::string> &results)
{
    std::string result;
    char buf_ps[1024];
    char ps[1024] = {0};
    FILE *ptr;
    strcpy(ps, cmd.c_str());
    if ((ptr = popen(ps, "re")) != NULL)
    {
        while (fgets(buf_ps, 1024, ptr) != NULL)
        {
            result += buf_ps;
        }
        pclose(ptr);
        ptr = NULL;
    }
    else
    {
        return false;
    }

    trim_blank_char(result);
    if (!result.empty())
    {
        ustd::split_string(result, "\n", results);
        for (auto iter = results.begin(); iter != results.end();)
        {
            trim_blank_char(*iter);
            if (iter->empty())
            {
                iter = results.erase(iter);
            }
            else
            {
                ++iter;
            }
        }
    }

    return true;
}

bool execute_cmd(const std::string &cmd, std::string &result)
{
    char buf_ps[1024] = {0};
    FILE *ptr = popen(cmd.c_str(), "re");
    if (ptr != NULL)
    {
        do
        {
            if (fgets(buf_ps, 1024 - 1, ptr) != NULL)
            {
                result += buf_ps;
                memset(buf_ps, 0, 1024);
            }
        } while (!feof(ptr));

        pclose(ptr);
        ptr = NULL;
    }
    else
    {
        return false;
    }

    trim_blank_char(result);
    return true;
}
} // namespace systemhelper
} // namespace algorithmservice