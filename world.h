
#include<iostream>
#include<iomanip>
#include<fstream>
#include<algorithm>
#include<vector>
#include<string>
#include<sstream>
#include<cmath>
#include<math.h>
#include<random>
#include<map>
#include<memory>
#include "cell.h"
#include "building.h"
#include "world_object.h"
#include "unit.h"
#include "resource.h"
#include "coord.h"

#ifndef world_class
#define world_class

typedef std::map<std::string,std::string> help_commands;
typedef std::map<std::string,resources_database> all_resources_database;

class world
{
    private:
        bool play{true}; //will stop the game as soon as this is false

        size_t x_size{0};
        size_t y_size{0};
        int seed{0};
        cell **world_cells; //keeps track of grid cells

        //pointers to various game objects
        std::vector<std::unique_ptr<damageable_object>> buildings;
        std::vector<std::unique_ptr<mobile_object>> units;
        std::vector<std::unique_ptr<mobile_object>> monsters;
        std::vector<std::unique_ptr<resource_building>> resource_buildings;
        std::vector<std::unique_ptr<resource>> resources;

        //stores player's resources and requirements for building / cloning
        resources_database resources_list;
        all_resources_database all_cloning_list;
        all_resources_database all_building_list;

        //stores commands
        static std::vector<std::string> command_list;
        static help_commands help_commands_list;
        
    public:
        world() = default; //only default constructor, parameters will be initialised by the player
        ~world() {delete[] world_cells;}

        void enter_world_size_and_seed();
        void initialise_variables(size_t entered_x, size_t entered_y, int entered_seed);  //this initialises parameters as soon as they are entered
        void declare_clonding_database(); //initialises resource requirements
        void declare_buildings_database();
        void add_initial_resources();

        bool get_play() {return play;} //checks if we're still playing
        void stop() {play = false;} //stops the game

        template <class c_type> c_type convert_from_string(std::string string);
        double calculate_distance(coord object_1, coord object_2);

        size_t get_flat_cell_position(size_t x_pos, size_t y_pos); //to convert xy to world_cells id
        bool compare_coords(coord first, coord second);

        //functions connected to world generation
        void generate_terrain();
        void grow_terrain(std::vector<coord*> ground_coords, std::vector<coord*> checked_coords, size_t position);
        double evaluate_upper_probability(size_t x, size_t y);
        bool check_if_spawn_ground_cell(size_t x, size_t y);
        void fill_in_holes();
        void remove_flat_sees();
        coord get_random_coords(coord upper_left_corner, coord lower_right_corner);
        void generate_crash_site();
        void generate_resources();
        template <class c_type> void spread_certain_resource(int number_needed, int number_done);
        
        //buildings and units
        void spread_monsters();
        bool spawn_unit(std::string unit_type, coord spawn_coord, size_t area);
        void build_building(std::string building_type, coord build_coord);
        bool check_if_cell_occupied(coord cell_coord);
        template <class c_type> bool check_if_same_coords(std::vector<std::unique_ptr<c_type>> &list, coord cell_coord);
        template <class c_type> int find_max_object_id(std::vector<std::unique_ptr<c_type>> &list, std::string object_type);

        //pathfinding
        int find_path(coord start, coord end, std::vector<std::string> exceptions);

        //command management, Part I
        void enter_command();
        void declare_help_commands();
        template <class c_type> size_t find_object_in_list(std::vector<std::unique_ptr<c_type>> &list, std::string object_name);
        template <class A, class V> void perform_attack(std::vector<std::unique_ptr<A>> &list_attacker, std::vector<std::unique_ptr<V>> &list_victim, size_t attacker_id, size_t victim_id);

        //command management, Part II
        void show_help_commands_list(std::vector<std::string> list);
        void process_move_command(std::vector<std::string> list);
        void process_attack_command(std::vector<std::string> list);
        void process_build_command(std::vector<std::string> list);
        void process_buildings_command(std::vector<std::string> list);
        void process_clear_command(std::vector<std::string> list);
        void process_units_command(std::vector<std::string> list);
        void process_resources_command(std::vector<std::string> list);
        void process_clone_command(std::vector<std::string> list);
        void process_turn_command(std::vector<std::string> list);
        void process_info_command(std::vector<std::string> list);

        //resource management
        bool check_if_enough_resources(resources_database checking_unit);
        bool check_if_mine_can_be_built(size_t x_coord, size_t y_coord);
        std::string get_resource_standing_on(coord resource_coords);
        void change_resources(resources_database resources_to_change, int take);
        resources_database get_total_resource_change();

        //monster management
        void monster_random_walk(size_t monster_id);
        bool choose_monster_target(size_t monster_id);

        void show_world();
};




//declared here as these templates are used across several files:

//converts some string input into size_t or int (because static_cast doesn't work with std::string)
template <class c_type> c_type world::convert_from_string(std::string string) 
{
    std::stringstream stream{string};
    c_type new_type;
    stream>>new_type; //conversion via stringstream
    return new_type; 
}

//checks entered co-ordinate agains elements in a unit/building/monster/etc list
template <class c_type> bool world::check_if_same_coords(std::vector<std::unique_ptr<c_type>> &list, coord cell_coord) 
{
    bool occupied{false};
    for(size_t i{0}; i != list.size(); i++) {
        if(compare_coords(list[i]->get_coordinates(), cell_coord)) { //checks if the cell is occupied by some entity from the list
           occupied = true; 
        }
    }

    return occupied;
}

//class A (player's unit / monster) attacks class V (monter / player's unit / building); victim might die (what a pity)
template <class A, class V> void world::perform_attack(std::vector<std::unique_ptr<A>> &list_attacker, std::vector<std::unique_ptr<V>> &list_victim, size_t attacker_id, size_t victim_id)
{
    list_attacker[attacker_id]->do_attack(); //so that the attacker can't attack in this turn anymore 
    list_victim[victim_id]->get_injured(list_attacker[attacker_id]->get_attack()); //injury the victim

    if(list_victim[victim_id]->get_health() > 0) { //if victim is still alive, notify player of the damage
        std::cout<<list_victim[victim_id]->get_type()<<" received "<<list_attacker[attacker_id]->get_attack()<<" damage; "<<list_victim[victim_id]->get_type()<<"'s remaining health is "<<list_victim[victim_id]->get_health()<<std::endl;
    } else { //otherwise, please, die
        size_t find_id{0};
        for (auto iterator = list_victim.begin(); iterator != list_victim.end(); iterator++) {
            if(find_id == victim_id) {
                std::cout<<list_victim[victim_id]->get_type()<<" is dead!"<<std::endl;
                iterator = list_victim.erase(iterator); //erase from vector (unique_ptr takes care of memory clearning)
                break; //get out of loop
            }
            find_id++;
        }
    }
}
#endif