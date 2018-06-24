#include <string>
#include <vector>

typedef enum
{
    LEFT = 0,
    RIGHT,
    BOTH
} wheelType;

typedef struct
{
    wheelType wheel;
    int speed;
    bool valid;
} wheelCommandType;

std::vector<std::string> split(const char *str, char c = ',');

wheelCommandType parse(const std::string &s);
