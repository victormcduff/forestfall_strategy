
#include<vector>
#include<string>
#include "world_object.h"

#ifndef resources_class
#define resources_class
class coal : public resource
{
    public:
        coal(coord entered_position) : resource("coal", entered_position) {};
        std::string get_icon() {return " Co";};
};

class oil : public resource
{
    public:
        oil(coord entered_position) : resource("oil", entered_position) {};
        std::string get_icon() {return " Oi";};
};

class raw_strongium : public resource
{
    public:
        raw_strongium(coord entered_position) : resource("raw_strongium", entered_position) {};
        std::string get_icon() {return " St";};
};

class raw_aluminium : public resource
{
    public:
        raw_aluminium(coord entered_position) : resource("raw_aluminium", entered_position) {};
        std::string get_icon() {return " Al";};
};

#endif