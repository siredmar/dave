#include <string>
#include <vector>
#include "cmdparser.h"

std::vector<std::string> split(const char *str, char c = ',')
{
    std::vector<std::string> result;
    do
    {
        const char *begin = str;
        while(*str != c && *str)
            str++;
        result.push_back(std::string(begin, str));
    } while (0 != *str++);

    return result;
}

wheelCommandType parse(const std::string &s)
{
    wheelCommandType ret;
    ret.valid = true;
    std::vector<std::string> list;
    list = split(s.c_str());
    int index = 0;
    for(auto & s : list)
    {
        switch(index)
        {
            case 0:
                if(s == "l")
                {
                    ret.wheel = LEFT;
                }
                else if(s == "r")
                {
                    ret.wheel = RIGHT;
                }
                else if(s == "b")
                {
                    ret.wheel = BOTH;
                }
                else if(s == "s")
                {
                    ret.wheel = BOTH;
                }
                else
                {
                    ret.valid = false;
                }
                break;

            case 1:
                 ret.speed = std::stoi(s);
                break;

            default:
                ret.valid = false;
                break;
        }
        index++;
    }
    return ret;
}