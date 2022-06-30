#include "world.h"

//to locate the maximum id, which is needed to spawn new units of the same type
template <class c_type> int world::find_max_object_id(std::vector<std::unique_ptr<c_type>> &list, std::string object_type)
{
    int max_object_id{0};
    for(size_t i{0}; i < list.size(); i++) {
        std::string type{list[i]->get_type()};
        std::string object_id{list[i]->get_id()};
        
        if(type == object_type) { //same type?
            int object_id_int = convert_from_string<int>(object_id);
            if(object_id_int > max_object_id) {
                max_object_id = object_id_int;
            }
        }
    }

    return max_object_id;
}


//spawns units, be it cloned units or monsters
bool world::spawn_unit(std::string unit_type, coord spawn_coord, size_t area)
{
    bool unit_added{false};
    for(size_t i{spawn_coord.x()-area}; i<=spawn_coord.x()+area; i++) {     //scans the area around spawn_coord for a good spawning cell
        for(size_t j{spawn_coord.y()-area}; j<=spawn_coord.y()+area; j++) { //needed for airship cloning, monsters have 0 area of scanning (just a single cell)
            if (i >=0 && i < x_size && j >= 0 && j < y_size) {
                if (world_cells[get_flat_cell_position(i,j)]->get_type() != "water") {
                    if(check_if_cell_occupied(coord{i,j}) == false) { //good cell?
                        if(unit_added != true) {
                            unit_added = true;
                            int max_id = find_max_object_id<mobile_object>(units, unit_type);
                            std::string max_id_new = std::to_string(max_id+1); //new unit id
                            if(unit_type == "worker") { //now, creation of a new object and a unique pointer to it
                                std::unique_ptr<worker> unit_object(new worker{coord{i,j}, max_id_new});
                                units.push_back(std::move(unit_object));
                            } else if(unit_type == "soldier") {
                                std::unique_ptr<soldier> unit_object(new soldier{coord{i,j}, max_id_new});
                                units.push_back(std::move(unit_object));
                            } else if(unit_type == "flamethrower_trooper") {
                                std::unique_ptr<flamethrower_trooper> unit_object(new flamethrower_trooper{coord{i,j}, max_id_new});
                                units.push_back(std::move(unit_object));
                            } else if(unit_type == "sniper") {
                                std::unique_ptr<sniper> unit_object(new sniper{coord{i,j}, max_id_new});
                                units.push_back(std::move(unit_object));
                            } else if(unit_type == "monster") {
                                std::unique_ptr<monster> unit_object(new monster{coord{i,j}, max_id_new});
                                monsters.push_back(std::move(unit_object));
                            }
                        }
                    }
                }
            }
        }
    }

    if(unit_added == false) {
        throw(8); //all cells are occupied (around the airship)
    } else {
        if(unit_type != "monster") { //don't say this if we spawn monsters
            std::cout<<"Unit successfully cloned!"<<std::endl;
        }
        return true;
    }
}

void world::spread_monsters()
{
    double monster_density{1.0/50};
    int forest_cells{0};

    for(size_t i{0}; i < x_size * y_size; i++) {
        if(world_cells[i]->get_type() == "forest") {
            forest_cells++;
        }
    }

    if(forest_cells > 0) { //so no monster spawning if no Forest
        while(int(monsters.size()) < int(forest_cells * monster_density + 2)) { //will spawn at least 2
            coord rand_coord = get_random_coords(coord{0,0}, coord{x_size-1, y_size-1});
            if(world_cells[get_flat_cell_position(rand_coord.x(), rand_coord.y())]->get_type() != "water") {
                bool cell_occupied{false};
                if(check_if_cell_occupied(rand_coord)) {
                    cell_occupied = true; //things (player's units, buildings) on the same cell?
                }

                if(monsters.size() > 0) {
                    if(check_if_same_coords<mobile_object>(monsters, rand_coord)) {
                        cell_occupied = true; //other monsters on the same cell?
                    }
                }

                if(cell_occupied == false) { //if nothing on this cell, create a monster!
                    spawn_unit("monster", rand_coord, 0);
                } //otherwise, loop until we have enough monsters
            }
        }
    }
}


void world::build_building(std::string building_type, coord build_coord)
{
    resources_database entered_input;
    resources_database entered_output;

    std::cout<<"Building successfully built!"<<std::endl;
    if(building_type == "mine") {
        std::string resource_id;
        for(size_t i{0}; i != resources.size(); i++) {
            if(compare_coords(resources[i]->get_coordinates(), build_coord)) {
                resource_id = resources[i]->get_type();
            }
        }
        entered_output[resource_id] = 2; //mine will generate 2 units of a given resource

        std::unique_ptr<resource_building> building_object(new mine(build_coord, entered_input, entered_output));
        resource_buildings.push_back(std::move(building_object));

    } else if(building_type == "factory_strongium") {

        //2 coal and raw metal in, one refined bit out
        entered_output["strongium"] = 1;
        entered_input["coal"] = 2;
        entered_input["raw_strongium"] = 2;

        std::unique_ptr<resource_building> building_object(new factory_strongium(build_coord, entered_input, entered_output));
        resource_buildings.push_back(std::move(building_object));

    } else if(building_type == "factory_aluminium") {

        //2 coal and raw metal in, one refined bit out
        entered_output["aluminium"] = 1;
        entered_input["coal"] = 2;
        entered_input["raw_aluminium"] = 2;

        std::unique_ptr<resource_building> building_object(new factory_aluminium(build_coord, entered_input, entered_output));
        resource_buildings.push_back(std::move(building_object));

    } else if(building_type == "defence_tower") {
        std::unique_ptr<damageable_object> building_object(new defence_tower(build_coord));
        buildings.push_back(std::move(building_object));
    }
}
