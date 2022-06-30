
#include<vector>
#include<string>
#include "cell.h"
#include "coord.h"
#include "world_object.h"

#ifndef unit_class
#define unit_class

//attack strength, health, movement etc are all declared here
//main code knows nothing of it as it shouldn't be changed

class monster : public mobile_object
{
    public:
        monster(coord entered_position, std::string entered_unit_id) : 
            mobile_object(entered_position, entered_unit_id, "monster", 5, {"water"}, 5, 1, 10) {};
        std::string get_icon() {return " M ";};
};


class worker : public mobile_object
{
    public:
        worker(coord entered_position, std::string entered_unit_id) : 
            mobile_object(entered_position, entered_unit_id, "worker", 3, {"water"}, 1, 1, 5) {};
        std::string get_icon() {return " W ";};
};

class soldier : public mobile_object
{
    public:
        soldier(coord entered_position, std::string entered_unit_id) : 
            mobile_object(entered_position, entered_unit_id, "soldier", 4, {"water", "forest"}, 5, 3, 15) {};
        std::string get_icon() {return " S ";};
};

class sniper : public mobile_object
{
    public:
        sniper(coord entered_position, std::string entered_unit_id) : 
            mobile_object(entered_position, entered_unit_id, "sniper", 3, {"water", "forest"}, 7, 5, 5) {};
        std::string get_icon() {return " Sn";};
};

class flamethrower_trooper : public mobile_object
{
    public:
        flamethrower_trooper(coord entered_position, std::string entered_unit_id) : 
            mobile_object(entered_position, entered_unit_id, "flamethrower_trooper", 2, {"water", "forest"}, 10, 3, 10) {};
        std::string get_icon() {return " Fl";};
};

#endif