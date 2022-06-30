#include "world.h"

void world::generate_terrain() 
{
    for(size_t i{0}; i<x_size; i++) {
        for(size_t j{0}; j<y_size; j++) {
            world_cells[get_flat_cell_position(i,j)] = new water("water"); //first fill the world with water
        }
    }

    std::cout<<"World sea generated"<<std::endl;

    std::vector<coord*> ground_coords; //vector of co-ordinates; will store the growing land
    std::vector<coord*> checked_ground_coords; //to ensure we don't check same cell twice
    size_t centre_x = int(x_size / 2);
    size_t centre_y = int(y_size / 2);
    coord centre{centre_x, centre_y};
    ground_coords.push_back(new coord{centre_x, centre_y});

    grow_terrain(ground_coords, checked_ground_coords, 0);
    std::cout<<"Ground generated"<<std::endl;
    fill_in_holes();
    remove_flat_sees();
    std::cout<<"Holes filled"<<std::endl;
    generate_crash_site();
    std::cout<<"Crash-site generated"<<std::endl;
    generate_resources();
    std::cout<<"Resources spread"<<std::endl;
    spread_monsters();
    std::cout<<"Initial monsters added!"<<std::endl;
}

//use ordinary pointers here because these vectors will not leave the scope of this function
void world::grow_terrain(std::vector<coord*> ground_coords, std::vector<coord*> checked_coords, size_t position) 
{
    size_t position_x{ground_coords[position]->x()}; //extract x-y of the point around which a new chunk of the world will grow
    size_t position_y{ground_coords[position]->y()};

    for(size_t i{position_x - 1}; i<=position_x + 1; i++) {
        for(size_t j{position_y - 1}; j<=position_y + 1; j++) {
            if (i >= 0 && i <= x_size && j >= 0 && j <= y_size) { //check that we are inside the world

                bool checked{false};
                for(size_t k{0}; k != checked_coords.size(); k++) {
                    if(compare_coords(coord{checked_coords[k]->x(), checked_coords[k]->y()}, coord{i,j})) { //check if this cell has been checked before
                        checked = true;
                    }
                }
                
                if(checked == false) {
                    if (check_if_spawn_ground_cell(i, j)) {
                        world_cells[get_flat_cell_position(i,j)] = new land("forest");
                        ground_coords.push_back(new coord{i,j}); //add only if probability is good
                    }
                    checked_coords.push_back(new coord{i,j});
                }
            }
        }
    }

    //if haven't yet checked all ground, repeat (and grow more ground)
    if (position != ground_coords.size() - 1) {
        grow_terrain(ground_coords, checked_coords, position+1);
    }
}


//evaluate max probability of there being a land cell at given co-ordinates
double world::evaluate_upper_probability(size_t x_point, size_t y_point) 
{
    int x = int(x_point);
    int y = int(y_point);
    int centre_x = int(x_size / 2);
    int centre_y = int(y_size / 2);
    int x_margin = 0;
    int y_margin = 0;

    double m_x = 0.8 / (centre_x - x_margin); //find gradient
    double m_y = 0.5 / (centre_y - y_margin);
    double b_x = -m_x * x_margin; //simple linear funciton
    double b_y = -m_y * y_margin;

    int shift_x = x;
    int shift_y = y;

    if (x > centre_x) {
        shift_x = abs(x - 2 * centre_x); //to make sure our linear function goes DOWN with x, if x > centre
    }

    if (y > centre_y) {
        shift_y = abs(y - 2 * centre_y); //same goes for y
    }

    double prob_x = m_x * shift_x + b_x;
    double prob_y = m_y * shift_y + b_y;
    double prob_total = (prob_x + prob_y) / 2.5; //normalise

    return prob_total;
}

bool world::check_if_spawn_ground_cell(size_t x, size_t y) 
{
    std::random_device generator_seed;
    std::mt19937 generator(generator_seed()); //activate generator
    generator.seed(int(150*x*y)+seed); //give generator some seed
    std::uniform_real_distribution<double> distribution(0.0,1.0);
    double upper_limit = evaluate_upper_probability(x, y);
    double generated_number = distribution(generator); //pick some number at random
    if (generated_number < upper_limit) { //compare with upper probability of there being land at these co-ords
        return true;
    } else {
        return false;
    }
}


//to fill holes in the ground which are smaller than max_limit_to_fill
void world::fill_in_holes()
{
    size_t max_limit_to_fill{3};

    for(size_t i{0}; i<x_size; i++) {
        for(size_t j{0}; j<y_size; j++) {
            if (world_cells[get_flat_cell_position(i,j)]->get_type() == "forest") {
                if(i + 1 < x_size) {
                    if (world_cells[get_flat_cell_position(i+1,j)]->get_type() == "water") { //if we found water next to a land cell
                        bool can_fill{false};
                        size_t position;
                        size_t shift{1};
                        while(shift <= max_limit_to_fill+1) { //check the hole is smaller than max lim
                            if(i + shift < x_size) { //check we're inside the world
                                if (world_cells[get_flat_cell_position(i+shift,j)]->get_type() == "forest") { //finally, the (holy) ground
                                    can_fill = true;
                                    position = i + shift;
                                }
                            }
                            shift++; //keep on shifting until land is found, or we're out of max limit 
                        }

                        if(can_fill) { //found a hole to fill
                            shift = 1;
                            while(i + shift <= position) {
                                world_cells[get_flat_cell_position(i+shift,j)] = new land("forest"); //keep on shifting and filling cells with land
                                shift++;
                            }
                        }
                    }
                }
            }
        }
    }
}


//even after hole-filling, some locked bodies of water remain; this takes care of most of the flat (1 cell thick) seas
void world::remove_flat_sees()
{
    for(size_t i{0}; i<x_size; i++) {
        for(size_t j{0}; j<y_size; j++) {
            if (world_cells[get_flat_cell_position(i,j)]->get_type() == "forest") {
                if(i + 1 < x_size) {
                    if(world_cells[get_flat_cell_position(i+1,j)]->get_type() == "water") {
                        bool can_fill{false};
                        bool can_continue{true};
                        size_t position;
                        size_t shift{1};
                        while(can_continue) {
                            if(i + shift < x_size && j - 1 >= 0 && j + 1 < y_size) { //inside the world?
                                //ensures we're dealing with FLAT seas
                                if(world_cells[get_flat_cell_position(i+shift,j-1)]->get_type() == "forest" && world_cells[get_flat_cell_position(i+shift,j+1)]->get_type() == "forest") {
                                    if(world_cells[get_flat_cell_position(i+shift,j)]->get_type() == "forest") { //struck land?
                                        can_fill = true;
                                        can_continue = false;
                                        position = i + shift;
                                    } else { //no, no land yet
                                        shift++;
                                    }
                                } else {
                                    can_continue = false;
                                }
                            } else {
                                can_continue = false;
                            }
                        }

                        if(can_fill) {
                            shift = 1;
                            while(i + shift <= position) {
                                world_cells[j+(i + shift)*y_size] = new land("forest"); //fill them all
                                shift++;
                            }
                        }
                    }
                }
            }
        }
    }
}


//generates resources according to their densities
void world::generate_resources()
{
    double coal_density{1.0/30};
    double oil_density{1.0/50};
    double strongium_density{1.0/40};
    double aluminium_density{1.0/50};
    int land_cells{0};

    for(size_t i{0}; i < x_size * y_size; i++) {
        if(world_cells[i]->get_type() != "water") {
            land_cells++;
        }
    }

    //now, spread certain resources, with the total = number of land cells times density
    spread_certain_resource<coal>(int(coal_density * land_cells), 0);
    spread_certain_resource<oil>(int(oil_density * land_cells), 0);
    spread_certain_resource<raw_strongium>(int(strongium_density * land_cells), 0); 
    spread_certain_resource<raw_aluminium>(int(aluminium_density * land_cells), 0); 
}


//takes care of actual resource spread by trying random co-ordinates
template <class c_type> void world::spread_certain_resource(int number_needed, int number_done) 
{
    coord rand_coord = get_random_coords(coord{0,0}, coord{x_size-1, y_size-1});
    if(world_cells[get_flat_cell_position(rand_coord.x(), rand_coord.y())]->get_type() == "water") {
        spread_certain_resource<c_type>(number_needed, number_done); //water cell, try again
    } else {
        bool cell_occupied{false};
        if(resources.size() > 0) {
            if(check_if_same_coords<resource>(resources, rand_coord)) { //there's already another resource, try again
                cell_occupied = true;
            }

            if(check_if_same_coords<damageable_object>(buildings, rand_coord)) { //there is a building (airship), try again
                cell_occupied = true;
            }
        }

        if(cell_occupied == false) {
            std::unique_ptr<c_type> resource(new c_type{rand_coord});
            resources.push_back(std::move(resource));
            number_done++;

            if(number_done < number_needed) { //keep going until we've reached max number
                spread_certain_resource<c_type>(number_needed, number_done);
            }
        } else {
            spread_certain_resource<c_type>(number_needed, number_done);
        }
    }
}


//generates the place where airship will be
void world::generate_crash_site()
{
    coord rand_coord = get_random_coords(coord{0,0}, coord{x_size-1, y_size-1});
    if(world_cells[get_flat_cell_position(rand_coord.x(), rand_coord.y())]->get_type() == "water") {
        generate_crash_site(); //water cell, try again
    } else {
        std::unique_ptr<damageable_object> airship_obj(new airship{rand_coord});
        buildings.push_back(std::move(airship_obj)); //create new building object
        
        size_t clearing_size{1};
        for(size_t i{rand_coord.x()-clearing_size}; i<=rand_coord.x()+clearing_size; i++) {
            for(size_t j{rand_coord.y()-clearing_size}; j<=rand_coord.y()+clearing_size; j++) {
                if (i >=0 && i < x_size && j >= 0 && j < y_size) { //inside the world?
                    if (world_cells[get_flat_cell_position(i, j)]->get_type() == "forest") {
                        world_cells[get_flat_cell_position(i, j)]->cleanse(); //clear some area around the airship
                    }
                }
            }
        }
        spawn_unit("worker", rand_coord, 1); //add first unit
    }
}