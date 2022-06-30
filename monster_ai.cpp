#include "world.h"

//move (if possible) a monster to a random cell within monster's movement area
void world::monster_random_walk(size_t monster_id)
{
    int movement{monsters[monster_id]->get_movement() / 2};
    coord monst_coord{monsters[monster_id]->get_coordinates()};
    coord rand_coord = get_random_coords(coord{monst_coord.x()-movement,monst_coord.y()-movement}, coord{monst_coord.x()+movement,monst_coord.y()+movement});
    int counter{find_path(monst_coord, rand_coord, monsters[monster_id]->get_forbidden_cells())}; //try to find path to random co-ordinates

    if(counter != -1 && counter <= monsters[monster_id]->get_movement() && check_if_cell_occupied(rand_coord) == false) { //checks if there's path, if cell isn't too far, and if it isn't occupied
        monsters[monster_id]->move(rand_coord, counter);
    }
}

//if something is within monster's attack range, choose what target (min heatlh) to attack (will prioritise units over buildings)
bool world::choose_monster_target(size_t monster_id)
{
    bool attacked{false};
    int min_building_health{10000};
    int min_resource_building_health{10000};
    int min_unit_health{10000}; //will try to find a target with minimum health
    size_t min_building_id{0};
    size_t min_resource_building_id{0};
    size_t min_unit_id{0};
    bool found_unit_target{false};
    bool found_building_target{false};
    bool found_resource_building_target{false};
    for(size_t i{0}; i != buildings.size(); i++) {
        if(calculate_distance(monsters[monster_id]->get_coordinates(), buildings[i]->get_coordinates()) <= monsters[monster_id]->get_range()) {
            if(buildings[i]->get_health() < min_building_health) {
                min_building_health = buildings[i]->get_health();
                min_building_id = i;
                found_building_target = true;
            }
        }
    }

    for(size_t i{0}; i != resource_buildings.size(); i++) {
        if(calculate_distance(monsters[monster_id]->get_coordinates(), resource_buildings[i]->get_coordinates()) <= monsters[monster_id]->get_range()) {
            if(resource_buildings[i]->get_health() < min_resource_building_health) {
                min_resource_building_health = resource_buildings[i]->get_health();
                min_resource_building_id = i;
                found_resource_building_target = true;
            }
        }
    }
    
    for(size_t i{0}; i != units.size(); i++) {
        if(calculate_distance(monsters[monster_id]->get_coordinates(), units[i]->get_coordinates()) <= monsters[monster_id]->get_range()) {
            if(units[i]->get_health() < min_unit_health) {
                min_unit_health = units[i]->get_health();
                min_unit_id = i;
                found_unit_target = true;
            }
        }
    }

    if(found_unit_target) { //prefer to attack player's units (because units can attack back...)
        perform_attack<mobile_object, mobile_object>(monsters, units, monster_id, min_unit_id);
        attacked = true;
    } else if (found_resource_building_target) { //otherwise will attack resource-producing buildings
        perform_attack<mobile_object, resource_building>(monsters, resource_buildings, monster_id, min_resource_building_id);
        attacked = true;
    } else if (found_building_target) { //and finally, all other types of buildings
        perform_attack<mobile_object, damageable_object>(monsters, buildings, monster_id, min_building_id);
        attacked = true;
    }

    return attacked;
}