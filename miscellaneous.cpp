#include "world.h"

//simple calculation of a distance between two co-ords using Pythagoras' theorem
//not a perfect way (because of the square grid), but a decent enough approximation
double world::calculate_distance(coord object_1, coord object_2)
{
    int x_diff = pow(int(object_1.x()) - int(object_2.x()), 2);
    int y_diff = pow(int(object_1.y()) - int(object_2.y()), 2);
    double distance = pow(x_diff + y_diff, 0.5);

    return distance;
}

//to turn xy co-ords into a position in the world_cells array
size_t world::get_flat_cell_position(size_t x_pos, size_t y_pos)
{
    size_t flat_pos{y_pos+x_pos*y_size};
    return flat_pos;
}

//checks if two co-ords are equal
bool world::compare_coords(coord first, coord second)
{
    if(first.x() == second.x() && first.y() == second.y()) {
        return true;
    } else {
        return false;
    }
}

//picks a co-ordinate at random inside a given area
coord world::get_random_coords(coord upper_left_corner, coord lower_right_corner)
{
    std::random_device seed;
    std::mt19937 generator(seed());
    std::uniform_int_distribution<> distibution_x(upper_left_corner.x(),lower_right_corner.x());
    std::uniform_int_distribution<> distibution_y(upper_left_corner.y(),lower_right_corner.y());
    size_t rand_x{static_cast<size_t>(distibution_x(generator))};
    size_t rand_y{static_cast<size_t>(distibution_y(generator))};

    return coord{rand_x, rand_y};
}


//checks if the cell contains buildings, units or monsters
bool world::check_if_cell_occupied(coord cell_coord) 
{
    bool occupied{false};

    if(check_if_same_coords<mobile_object>(units, cell_coord)) {
        occupied = true;
    }

    if(check_if_same_coords<mobile_object>(monsters, cell_coord)) {
        occupied = true;
    }

    if(check_if_same_coords<resource_building>(resource_buildings, cell_coord)) {
        occupied = true;
    }

    if(check_if_same_coords<damageable_object>(buildings, cell_coord)) {
        occupied = true;
    }
    
    return occupied;
}


//pathfinding algorithm
int world::find_path(coord start, coord end, std::vector<std::string> exceptions)
{   
    std::vector<std::vector<size_t>> paths;
    std::vector<coord*> checked_cells;

    //check if the target cell is a bad cell
    if(std::find(exceptions.begin(), exceptions.end(), world_cells[get_flat_cell_position(end.x(), end.y())]->get_type()) == exceptions.end()) {
        paths.push_back(std::vector<size_t>{end.x(), end.y(), 0}); //number at the end (0 in this case) means distance from end cell
        checked_cells.push_back(new coord{end.x(), end.y()});
    } else {
        return -1;
    }

    size_t i{0};
    //if target cell is not a bad cell, enter a pathfinding loop
    while(i != paths.size()) { //have we checked all cells in the path vector?
        std::vector<coord*> cells_to_consider;

        size_t x{paths[i][0]};
        size_t y{paths[i][1]};
        
        //add neighbouring cells for consideration
        if(y + 1 < y_size) {
            cells_to_consider.push_back(new coord{x, y + 1});
        }
        if(int(y - 1) >= 0) {
            cells_to_consider.push_back(new coord{x, y - 1});
        }
        if(x + 1 < x_size) {
            cells_to_consider.push_back(new coord{x + 1, y});
        }
        if(int(x - 1) >= 0) {
            cells_to_consider.push_back(new coord{x - 1, y});
        }

        for(size_t j{0}; j != cells_to_consider.size(); j++) {
            bool already_checked{false};
            for(size_t k{0}; k != checked_cells.size(); k++) { //check if already checked some of those cells_to_consider
                if(compare_coords(*checked_cells[k], *cells_to_consider[j])) {
                    already_checked = true;
                }
            }
            if(already_checked == false) {
                if(std::find(exceptions.begin(), exceptions.end(), world_cells[get_flat_cell_position(cells_to_consider[j]->x(), cells_to_consider[j]->y())]->get_type()) == exceptions.end()) {
                    paths.push_back(std::vector<size_t>{cells_to_consider[j]->x(), cells_to_consider[j]->y(), paths[i][2]+1}); //add to path vector if cell is good, and increase distance from end cell
                    checked_cells.push_back(cells_to_consider[j]); //ensure we won't check this cell again
                }
            }
        }
        i++;
    }

    int counter{-1}; //will return -1 if no path found
    for(size_t i{0}; i != paths.size(); i++) {
        if(paths[i][0] == start.x() && paths[i][1] == start.y()) {
            counter = paths[i][2]; //extract how far the start cell is
        }
    }

    return counter;

}


//shows world
void world::show_world()
{
    std::ostringstream os;
    for(size_t j{0}; j<y_size; j++) {
        if(j == 0) {
            os<<"x/y";
            for(size_t i{0}; i<x_size; i++) {
                if(i>=9) { //show x-axis
                    os<<" "<<int(i);
                } else {
                    os<<" "<<int(i)<<" ";
                }
            }
            os<<"\n";
        }
        
        for(size_t i{0}; i<x_size; i++) {
            bool object_on_cell{false};
            bool resource_on_cell{false};
            bool unit_on_cell{false};
            bool has_number{false};

            if(i == 0) {
                if(j>9) {  //show y-axis
                    os<<int(j)<<" ";
                } else {
                    os<<" "<<int(j)<<" ";
                }
            }

            if(units.size() > 0) { //shows icon of a unit (if there are any)
                for(size_t k{0}; k != units.size(); k++) {
                    if(units[k]->get_coordinates().x() == i && units[k]->get_coordinates().y() == j) {
                        unit_on_cell = true;
                        os<<units[k]->get_icon();
                    }
                }
            }
            
            if(monsters.size() > 0) { //shows icon of a monster (if there are any)
                for(size_t k{0}; k != monsters.size(); k++) {
                    if(monsters[k]->get_coordinates().x() == i && monsters[k]->get_coordinates().y() == j) {
                        unit_on_cell = true;
                        os<<monsters[k]->get_icon();
                    }
                }
            }

            for(size_t k{0}; k != buildings.size(); k++) { //shows icon of a building (there will be at least airship)
                if(compare_coords(buildings[k]->get_coordinates(), coord{i,j})) {
                    if(unit_on_cell == false) { //buildings will be obscured by units standing on them
                        object_on_cell = true;
                        os<<buildings[k]->get_icon();
                    }
                }
            }

            if(resource_buildings.size() > 0) { //shows icon of a resource building, if any
                for(size_t k{0}; k != resource_buildings.size(); k++) {
                    if(compare_coords(resource_buildings[k]->get_coordinates(), coord{i,j})) {
                        if(unit_on_cell == false) { //buildings will be obscured by units standing on them
                            object_on_cell = true;
                            os<<resource_buildings[k]->get_icon();
                        }
                    }
                }
            }

            for(size_t k{0}; k != resources.size(); k++) {
                if(compare_coords(resources[k]->get_coordinates(), coord{i,j})) {
                    if(unit_on_cell == false && object_on_cell == false) { //resources will be obscured by units/buildings standing on them
                        resource_on_cell = true;
                        os<<resources[k]->get_icon();
                    }
                }
            }

            //shows land/water/forest if there are no buildings, units, resources or monsters on top of them
            if(object_on_cell == false && unit_on_cell == false && has_number == false && resource_on_cell == false) {
                if(world_cells[get_flat_cell_position(i,j)]->get_type() == "water") {
                    os<<" ~ ";
                } else if(world_cells[get_flat_cell_position(i,j)]->get_type() == "forest"){
                    os<<" | ";
                } else {
                    os<<" # ";
                }
            }
        }
        os<<"\n";
    }

    std::cout<<os.str();
}


//compares a list of resources with what player has
bool world::check_if_enough_resources(resources_database checking_unit)
{
    bool enough_resources{true};
    for(auto iterator{checking_unit.begin()}; iterator != checking_unit.end(); iterator++) {
        if(resources_list[iterator->first] < iterator->second) {
            enough_resources = false;
        }
    }

    return enough_resources;
}

//resource change (+ve or -ve)
void world::change_resources(resources_database resources_to_change, int take)
{
    for(auto iterator{resources_to_change.begin()}; iterator != resources_to_change.end(); iterator++) {
        resources_list[iterator->first] += take * iterator->second;
    }
}

//extracts all imput and output from all mines and factories
resources_database world::get_total_resource_change()
{
    resources_database input_output;
    if(resource_buildings.size() > 0) {
        for(size_t i{0}; i != resource_buildings.size(); i++) {
            resources_database resources_consumed{resource_buildings[i]->get_input()};
            resources_database resources_produced{resource_buildings[i]->get_output()};

            if(check_if_enough_resources(resources_consumed)) {
                if(resources_consumed.size() > 0) {
                    for(auto iterator{resources_consumed.begin()}; iterator != resources_consumed.end(); iterator++) {
                        input_output[iterator->first] -= iterator->second;
                    }
                }
                
                if(resources_produced.size() > 0) {
                    for(auto iterator{resources_produced.begin()}; iterator != resources_produced.end(); iterator++) {
                        input_output[iterator->first] += iterator->second;
                    }
                }
            } else { //sends a warning that some factories might be in need of some resources
                std::cout<<"Warning! Not enough raw material. These resources are needed:"<<std::endl;
                for(auto iterator{resources_consumed.begin()}; iterator != resources_consumed.end(); iterator++) {
                    std::cout<<iterator->first<<": "<<iterator->second<<std::endl;
                }
            }
        }
    }

    return input_output;
}

