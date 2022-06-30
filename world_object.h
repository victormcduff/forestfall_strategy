#include "coord.h"
#include<map>

#ifndef basic_world_objects
#define basic_world_objects

//define a new class for resource management. 
//defined here because I didn't want world_object.h to reference world.h (that'd be silly)
typedef std::map<std::string,int> resources_database;

//even though it is a base class of all game objects save for cells, it is NOT an abstract base class
//because a set of functions (get_type, get_coords) should have exactly the same functionality across all its children
class world_object
{
    protected:
        std::string type;
        coord position;
    public:
        world_object(std::string entered_type, coord entered_position) : type{entered_type}, position{entered_position} {}
        std::string get_type() {return type;}
        coord get_coordinates() {return position;}
        virtual std::string get_icon()=0; //this one is virtual because it will be changed depending on the object
        virtual ~world_object(){}
};

class resource : public world_object
{
    public:
        resource(std::string entered_type, coord entered_position) : world_object(entered_type, entered_position) {}
        resource(resource &&res) : world_object(res.type, res.position) {} //move constructor to store in the unique_ptr vector in world class
        virtual ~resource(){}
};

class damageable_object : public world_object
{
    protected:
        int health;
    public:
        damageable_object(std::string entered_type, coord entered_position, int entered_health) : world_object(entered_type, entered_position), health{entered_health} {}
        damageable_object(damageable_object &&obj) : world_object(obj.type, obj.position), health{obj.health} {} //move constructor, same reason
        virtual ~damageable_object(){}
        int get_health() {return health;} //add new functions connected to health (but not safety)
        void get_injured(int damage) {health -= damage;}
};


class mobile_object : public damageable_object
{
    protected:
        std::vector<std::string> forbidden; //will list cell types which this object can't get across (i.e. water)
        int movement;
        int movement_left;
        std::string unit_id;
        int attack;
        int range;
        bool attacked{false};

    public:
        mobile_object(coord entered_position, std::string entered_unit_id, std::string entered_type, int entered_movement, std::vector<std::string> forbidden_cells, int entered_attack, int entered_range, int entered_health) : 
            damageable_object(entered_type, entered_position, entered_health),
            forbidden{forbidden_cells},
            movement{entered_movement},
            movement_left{entered_movement},
            unit_id{entered_unit_id},
            attack{entered_attack},
            range{entered_range} {}
        mobile_object(mobile_object &&obj) : 
            damageable_object(obj.type, obj.position, obj.health),
            forbidden{obj.forbidden},
            movement{obj.movement},
            movement_left{obj.movement_left},
            unit_id{obj.unit_id},
            attack{obj.attack},
            range{obj.range} {} //move constructor
        virtual ~mobile_object(){}
        int get_attack() {return attack;} //attack functions
        int get_range() {return range;}
        int get_movement() {return movement_left;}
        int get_max_movement() {return movement;}
        bool check_if_attacked() {return attacked;}
        void do_attack() {attacked = true;} //to ensure it won't attack again this turn
        std::string get_id() {return unit_id;};
        void move(coord new_pos, int moved_cells) {position = new_pos; movement_left -= moved_cells;} //movement
        std::vector<std::string> get_forbidden_cells() {return forbidden;}
        void turn() {movement_left = movement; attacked = false;}; //restores movement and attack capability
};
#endif