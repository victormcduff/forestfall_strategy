
#include<vector>
#include<string>
#include "world_object.h"

#ifndef building_class
#define building_class
class airship : public damageable_object
{
    public:
        airship(coord entered_position) : damageable_object("airship", entered_position, 100) {};
        std::string get_icon() {return " A ";};
};

class defence_tower : public damageable_object
{
    public:
        defence_tower(coord entered_position) : damageable_object("defence_tower", entered_position, 30) {};
        std::string get_icon() {return " T ";};
};


//new class for buildings that operate with resources
class resource_building : public damageable_object
{
    private:
        resources_database input;
        resources_database output;
    public:
        resource_building(std::string type, coord entered_position, int health, resources_database entered_input, resources_database entered_output) : 
            damageable_object(type, entered_position, health),
            input{entered_input},
            output{entered_output}
            {};
        resource_building(resource_building &&obj) : 
            damageable_object(obj.type, obj.position, obj.health),
            input{obj.input},
            output{obj.output}
            {};
        resources_database get_input() {return input;}
        resources_database get_output() {return output;}
};


//like in the case of units, we declare type and health here because those should not be changed by the main code
class mine : public resource_building
{
    public:
        mine(coord entered_position, resources_database entered_input, resources_database entered_output) : resource_building("mine", entered_position, 10, entered_input, entered_output) {};
        std::string get_icon() {return " Mi";};
};

class factory_strongium : public resource_building
{
    public:
        factory_strongium(coord entered_position, resources_database entered_input, resources_database entered_output) : resource_building("factory_strongium", entered_position, 30, entered_input, entered_output) {};
        std::string get_icon() {return " F ";};
};

class factory_aluminium : public resource_building
{
    public:
        factory_aluminium(coord entered_position, resources_database entered_input, resources_database entered_output) : resource_building("factory_aluminium", entered_position, 30, entered_input, entered_output) {};
        std::string get_icon() {return " F ";};
};
#endif