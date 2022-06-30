#include "world.h"

//splits input into fragments separated by empty spaces
std::vector<std::string> parse_entered_command(std::string entered_line, std::vector<std::string> parsed_command)
{
    std::string split = " ";
    size_t separation = entered_line.find(split);
    std::string first_bit = entered_line.substr(0,separation);

    if(first_bit != "") { //to exclude excessive spaces
        parsed_command.push_back(first_bit);
    }

    if(separation != std::string::npos) {
        std::string remaining_bit = entered_line.substr(separation+split.size());
        return parse_entered_command(remaining_bit, parsed_command); //if something is left in the line, parse again!
    } else {
        return parsed_command;
    }
    
}

//takes care of accepting the size of the world and the seed from user's input
void world::enter_world_size_and_seed()
{
    std::string input;
    std::cout<<"Please enter world's dimensions and seed (e.g. 50 50 424242): ";
    std::getline(std::cin, input);
    std::vector<std::string> parsed_command;
    parsed_command = parse_entered_command(input, parsed_command);
    std::cout<<"\n";

    if(parsed_command.size() != 3) { //incorrect size
        std::cerr<<"Error! Please try again."<<std::endl;
        enter_world_size_and_seed();
    //conversion function fill return 0 if there are letters in the input; hence, we can check for non-int, zero or negative input at the same time
    } else if (convert_from_string<int>(parsed_command[0]) <= 0 || 
        convert_from_string<int>(parsed_command[1]) <= 0 || 
        convert_from_string<int>(parsed_command[2]) <= 0) {
        std::cerr<<"Error! Non-int, zero or negative input. Please try again."<<std::endl;
        enter_world_size_and_seed();
    } else {
        initialise_variables(convert_from_string<size_t>(parsed_command[0]), convert_from_string<size_t>(parsed_command[1]), convert_from_string<int>(parsed_command[2]));
    }
}

//initialises all member variables of world
void world::initialise_variables(size_t entered_x, size_t entered_y, int entered_seed)
{
    x_size = entered_x;
    y_size = entered_y;
    seed = entered_seed;
    world_cells = new cell*[x_size*y_size];
    declare_help_commands();
    declare_clonding_database();
    declare_buildings_database();
    add_initial_resources();
}



void world::enter_command() 
{
    std::string input;
    std::cout<<"Command: ";
    std::getline(std::cin, input);
    std::vector<std::string> parsed_command;
    parsed_command = parse_entered_command(input, parsed_command);
    std::cout<<"\n";

    //errors:
    int command_not_found{0};
    int bad_command{1};
    int bad_help{2};
    int no_path{3};
    int target_too_far{4};
    int bad_unit{5};
    int not_enough_resources{6};
    int bad_clone{7};
    int cannot_clone{8};
    int no_monster{9};
    int path_blocked{10};
    int out_of_range{11};
    int already_attacked{12};
    int only_worker_can_clear{13};
    int non_forest_cell{14};
    int only_worker_can_build{15};
    int cannot_build_in_forest{16};
    int mine_needs_resources{17};

    try {
        auto command_found = std::find(begin(command_list), end(command_list), parsed_command[0]);
        if(command_found != end(command_list)) { //compares entered command against those stored in the database
            if (parsed_command[0] == "help") {
                show_help_commands_list(parsed_command);
            } else if (parsed_command[0] == "move") {
                process_move_command(parsed_command); 
            } else if (parsed_command[0] == "attack") {
                process_attack_command(parsed_command);
            } else if (parsed_command[0] == "build") {
                process_build_command(parsed_command); 
            } else if (parsed_command[0] == "buildings") {
                process_buildings_command(parsed_command);
            } else if (parsed_command[0] == "clear") {
                process_clear_command(parsed_command);
            } else if (parsed_command[0] == "units") {
                process_units_command(parsed_command);
            } else if (parsed_command[0] == "resources") {
                process_resources_command(parsed_command);
            } else if (parsed_command[0] == "clone") {
                process_clone_command(parsed_command);
            } else if (parsed_command[0] == "show") {
                show_world();
            } else if (parsed_command[0] == "info") {
                process_info_command(parsed_command);
            } else if (parsed_command[0] == "turn") {
                process_turn_command(parsed_command);
            } else if (parsed_command[0] == "end") {
                stop();
            }
        } else {
            throw(command_not_found);
        }
    } catch(int error_flag) { //error handling
        if(error_flag == command_not_found) {
            std::cerr<<"Error: command not found! Consider using 'help'"<<std::endl;
        } else if (error_flag == bad_command) {
            std::cerr<<"Error: command not used correctly! Consider typing help [command]"<<std::endl;
        } else if (error_flag == bad_help) {
            std::cerr<<"Error: can't find this command! Consider using 'help'"<<std::endl;
        } else if (error_flag == no_path) {
            std::cerr<<"Error: can't find path! Some of the cells might be impenetrable. Type info [unit_name] to check which cells unit can go over"<<std::endl;
        } else if (error_flag == target_too_far) {
            std::cerr<<"Error: target is too far! Consider typing info [unit_name] to check unit's remaining movement"<<std::endl;
        } else if (error_flag == path_blocked) {
            std::cerr<<"Error: cell already occupied!"<<std::endl;
        } else if (error_flag == bad_unit) {
            std::cerr<<"Error: can't find unit! Consider using 'units'"<<std::endl;
        } else if (error_flag == not_enough_resources) {
            std::cerr<<"Error: not enough resources to do this operation!"<<std::endl;
        } else if (error_flag == bad_clone) {
            std::cerr<<"Error: can't find unit to clone! Consider typing 'clone'"<<std::endl;
        } else if (error_flag == cannot_clone) {
            std::cerr<<"Error: cannot clone! All land around airship is occupied. Consider moving some of the units away"<<std::endl;
        } else if (error_flag == no_monster) {
            std::cerr<<"Error: there is nothing to attack at these co-ordinates!"<<std::endl;
        } else if (error_flag == out_of_range) {
            std::cerr<<"Error: enemy is out of range! Consider using info [unit_name] to check unit's range of attack"<<std::endl;
        } else if (error_flag == already_attacked) {
            std::cerr<<"Error: this unit has already attacked during this turn!"<<std::endl;
        } else if (error_flag == only_worker_can_clear) {
            std::cerr<<"Error: only a worker unit can clear the Forest!"<<std::endl;
        } else if (error_flag == only_worker_can_build) {
            std::cerr<<"Error: only a worker unit can build!"<<std::endl;
        } else if (error_flag == non_forest_cell) {
            std::cerr<<"Error: trying to clear cell with no Forest!"<<std::endl;
        } else if (error_flag == cannot_build_in_forest) {
            std::cerr<<"Error: cannot build in Forest! Consider clearning the cell first."<<std::endl;
        } else if (error_flag == mine_needs_resources) {
            std::cerr<<"Error: mine needs to be build on a resource!"<<std::endl;
        }
    }

    std::cout<<"\n";
}

//this looks for object in a list (units or else) and checks it against what player entered
template <class c_type> size_t world::find_object_in_list(std::vector<std::unique_ptr<c_type>> &list, std::string object_name)
{
    size_t found_object_id{0};
    bool found_object{false};
    for(size_t i{0}; i < list.size(); i++) {
        std::string type{list[i]->get_type()};
        std::string object_id{list[i]->get_id()};
        std::string new_name = type + "_" + object_id; //forms the name with id

        if(new_name == object_name) {
            found_object = true;
            found_object_id = i;
        }
    }

    if(found_object) {
        return found_object_id;
    } else {
        throw(5); //bad object
    }
}

void world::show_help_commands_list(std::vector<std::string> list) 
{
    if(list.size() == 1) {
        for(auto iterator{help_commands_list.begin()}; iterator != help_commands_list.end(); iterator++) {
            std::cout<<iterator->second<<std::endl; //just lists everything
        }
    } else if(list.size() == 2) {
        auto help_command_found = help_commands_list.find(list[1]);
        if(help_command_found != end(help_commands_list)) {
            std::cout<<help_commands_list[list[1]]<<std::endl; //lists what a particular command does
        } else {
            throw(2); //bad help
        }
    } else {
        throw(1); //bad command
    }
}

void world::process_move_command(std::vector<std::string> list)
{
    if(list.size() == 4) {
        size_t found_unit_id{find_object_in_list<mobile_object>(units, list[1])};

        size_t x_pos{convert_from_string<size_t>(list[2])};
        size_t y_pos{convert_from_string<size_t>(list[3])};
        coord end_pos{x_pos, y_pos}; //create coord object out of player's input
        int counter{find_path(units[found_unit_id]->get_coordinates(), end_pos, units[found_unit_id]->get_forbidden_cells())}; //generates path

        if(counter == -1) {
            throw(3); //no path found
        } else if(counter <= units[found_unit_id]->get_movement()) {
            if(check_if_cell_occupied(end_pos)) {
                throw(10); //cell is occupied by something
            } else {
                units[found_unit_id]->move(end_pos, counter);
                std::cout<<"Unit moved!"<<std::endl;
            }
        } else {
            throw(4); //target too far
        }
    
    } else {
        throw(1); //bad command
    } 
}

void world::process_attack_command(std::vector<std::string> list) 
{
    if(list.size() == 4) {
        size_t found_unit_id{find_object_in_list<mobile_object>(units, list[1])};

        size_t x_pos{convert_from_string<size_t>(list[2])};
        size_t y_pos{convert_from_string<size_t>(list[3])};
        coord new_pos{x_pos, y_pos};

        bool monster_found{false};
        size_t monster_id{0};
        for(size_t i{0}; i != monsters.size(); i++) {
            if(compare_coords(monsters[i]->get_coordinates(), new_pos)) {
                monster_found = true; //so there IS a monster hiding at these co-ords!
                monster_id = i;
            }
        }

        if(monster_found) {
            double distance = calculate_distance(monsters[monster_id]->get_coordinates(), units[found_unit_id]->get_coordinates());
            if (distance <= units[found_unit_id]->get_range()) {
                if (units[found_unit_id]->check_if_attacked()) {
                    throw(12); //already attacked during this turn
                } else {
                    perform_attack<mobile_object, mobile_object>(units, monsters, found_unit_id, monster_id);
                }
            } else {
                throw(11); //monster too far 
            }
        } else {
            throw(9); //nothing to attack (what a pity)
        }
    
    } else {
        throw(1); //bad command
    } 
}


void world::process_build_command(std::vector<std::string> list) 
{
    if(list.size() == 2 || list.size() == 3) {
        size_t found_unit_id{find_object_in_list<mobile_object>(units, list[1])};
        size_t x_coord = units[found_unit_id]->get_coordinates().x();
        size_t y_coord = units[found_unit_id]->get_coordinates().y();
        
        if(units[found_unit_id]->get_type() != "worker") {
            throw(15); //only proletariat can build things!!
        } else if (world_cells[get_flat_cell_position(x_coord, y_coord)]->get_type() == "forest"){
            throw(16); //oops no building in the Forest (of the night...)
        } else {
            if (list.size() == 2) {
                bool can_build_something{false};
                bool can_build_mine{false};
                for(auto iterator{all_building_list.begin()}; iterator != all_building_list.end(); iterator++) {
                    resources_database checking_building{all_building_list[iterator->first]};
                    if(check_if_enough_resources(checking_building)) {
                        if(iterator->first == "mine") {
                            if(check_if_mine_can_be_built(x_coord, y_coord)) {
                                can_build_something = true;
                                can_build_mine = true; //specific conditions for mine building
                            }
                        } else {
                            can_build_something = true;
                        }

                        if(can_build_something == true) {
                            if(iterator->first == "mine") {
                                if(can_build_mine) { //show mine's requirements if can build one
                                    std::cout<<iterator->first<<"\n("<<get_resource_standing_on(coord{x_coord, y_coord})<<")\n";
                                    for(auto iterator_resources{checking_building.begin()}; iterator_resources != checking_building.end(); iterator_resources++) {
                                        std::cout<<iterator_resources->first<<": "<<iterator_resources->second<<std::endl;
                                    }
                                    std::cout<<"\n";
                                }
                            } else {
                                std::cout<<iterator->first<<std::endl;
                                for(auto iterator_resources{checking_building.begin()}; iterator_resources != checking_building.end(); iterator_resources++) {
                                    std::cout<<iterator_resources->first<<": "<<iterator_resources->second<<std::endl;
                                }
                                std::cout<<"\n";
                            }
                        }
                    }
                }

                if(can_build_something == false){
                    std::cout<<"Sorry, you do not have enough resources to build anything."<<std::endl;
                }
            } else { //i.e. list size == 3, means a request to build something specific
                bool found_building{false};
                for(auto iterator{all_building_list.begin()}; iterator != all_building_list.end(); iterator++) {
                    if(iterator->first == list[2]) {
                        found_building = true;
                        resources_database checking_building{all_building_list[iterator->first]};
                        if(check_if_enough_resources(checking_building)) {
                            if(iterator->first == "mine") {
                                if(check_if_mine_can_be_built(x_coord, y_coord) == false) {
                                    throw(17); //no resources at your cell, no mine!
                                }
                            }

                            change_resources(checking_building, -1); //consume resources to build
                            build_building(list[2], coord{x_coord, y_coord});

                        } else {
                            throw(6); //not enough resources
                        }
                    }
                
                }

                if(found_building == false) {
                    throw(17); //there is no such building
                }
            }
        }
    } else {
        throw(1); //bad command
    }
}

bool world::check_if_mine_can_be_built(size_t x_coord, size_t y_coord)
{
    bool stand_on_resource{false};
    if(check_if_same_coords<resource>(resources, coord{x_coord, y_coord})) {
        stand_on_resource = true; //self-evident
    }

    return stand_on_resource;
}

//returns the name of a resource player's unit stands on (needed to determine mine's output)
std::string world::get_resource_standing_on(coord resource_coords)
{
    std::string resource_id;
    for(size_t i{0}; i != resources.size(); i++) {
        if(compare_coords(resources[i]->get_coordinates(), resource_coords)) {
            resource_id = resources[i]->get_type();
        }
    }
    return resource_id;
}

//cuts Forest down
void world::process_clear_command(std::vector<std::string> list) 
{
    if(list.size() == 2) {
        size_t found_unit_id{find_object_in_list<mobile_object>(units, list[1])};
        size_t x_coord = units[found_unit_id]->get_coordinates().x();
        size_t y_coord = units[found_unit_id]->get_coordinates().y();
        
        if(units[found_unit_id]->get_type() != "worker") {
            throw(13); //only proletariat can cut trees down (others are just too WEAK)!
        } else if (world_cells[y_coord+x_coord*y_size]->get_type() != "forest") {
            throw(14); //this ain't no Forest
        } else {
            world_cells[y_coord+x_coord*y_size]->cleanse();
            std::cout<<"Forest successfully cleared!"<<std::endl;
        }
    } else {
        throw(1); //bad command
    }
}

void world::process_units_command(std::vector<std::string> list)
{
    if(list.size() == 1) {
        for(size_t i{0}; i != units.size(); i++) { //shows where all units are, and gives their names (type + _ + id)
            std::cout<<units[i]->get_type()<<"_"<<units[i]->get_id()<<" at "<<"x:"<<units[i]->get_coordinates().x()<<" y:"<<units[i]->get_coordinates().y()<<std::endl;
        }
    
    } else {
        throw(1); //bad command
    }
}


//shows all resources and input/output per turn
void world::process_resources_command(std::vector<std::string> list) 
{
    if(list.size() == 1) {
        resources_database input_output = get_total_resource_change();
        std::string sign{"+"};

        for(auto iterator{resources_list.begin()}; iterator != resources_list.end(); iterator++) {
            if(input_output[iterator->first] > 0) {
                sign = "+";
            } else {
                sign = "";
            }
            
            std::cout<<iterator->first<<": "<<iterator->second<<" ("<<sign<<input_output[iterator->first]<<")"<<std::endl;
        }
    
    } else {
        throw(1); //bad command
    }
}

void world::process_clone_command(std::vector<std::string> list)
{
    if(list.size() == 1) { //lists all units available for cloning (if player has enough resources)
        bool can_clone_someone{false};
        for(auto iterator{all_cloning_list.begin()}; iterator != all_cloning_list.end(); iterator++) {
            resources_database checking_unit{all_cloning_list[iterator->first]};
            if(check_if_enough_resources(checking_unit)) {
                can_clone_someone = true;
                std::cout<<iterator->first<<"\n";
                for(auto iterator_resources{checking_unit.begin()}; iterator_resources != checking_unit.end(); iterator_resources++) {
                    std::cout<<iterator_resources->first<<": "<<iterator_resources->second<<std::endl;
                }
                std::cout<<"\n";
            }
        }

        if(can_clone_someone == false){
            std::cout<<"Sorry, you do not have enough resources to clone anyone."<<std::endl; //pity
        }
    
    } else if(list.size() == 2) { //want to clone someone specific
        bool found_unit{false};
        for(auto iterator{all_cloning_list.begin()}; iterator != all_cloning_list.end(); iterator++) {
            if(iterator->first == list[1]) {
                found_unit = true;
                resources_database checking_unit{all_cloning_list[iterator->first]};
                if(check_if_enough_resources(checking_unit)) {
                    bool success = spawn_unit(iterator->first, buildings[0]->get_coordinates(), 1); //clones
                    if(success) {
                        change_resources(checking_unit, -1); //consumes resources
                    }
                } else {
                    throw(6); //not enough resources
                }
            }
        }
        
        if(found_unit == false) {
            throw(7); //no such unit
        }
    } else {
        throw(1); //bad command
    }
}


//lists all buildings with their input and putput
void world::process_buildings_command(std::vector<std::string> list)
{
    if(list.size() == 1) {
        if(resource_buildings.size() > 0) {
            for(size_t i{0}; i != resource_buildings.size(); i++) {
                std::cout<<resource_buildings[i]->get_type()<<" at x:"<<resource_buildings[i]->get_coordinates().x()<<" y:"<<resource_buildings[i]->get_coordinates().y()<<std::endl;
                resources_database resources_consumed{resource_buildings[i]->get_input()};
                resources_database resources_produced{resource_buildings[i]->get_output()};

                if(check_if_enough_resources(resources_consumed)) { //checks that player has enough resources to be consumed by resource_buildings[i]
                    if(resources_consumed.size() > 0) {             //otherwise, the building doesn't work & doesn't appear here
                        std::cout<<"Consumes: "<<std::endl;
                        for(auto iterator{resources_consumed.begin()}; iterator != resources_consumed.end(); iterator++) {
                            std::cout<<iterator->first<<": "<<iterator->second<<std::endl;
                        }
                        std::cout<<"\n";
                    }
                    
                    if(resources_produced.size() > 0) {
                        std::cout<<"Produces: "<<std::endl;
                        for(auto iterator{resources_produced.begin()}; iterator != resources_produced.end(); iterator++) {
                            std::cout<<iterator->first<<": "<<iterator->second<<std::endl;
                        }
                        std::cout<<"\n";
                    }
                }
            }
        } else {
            std::cout<<"You have no buildings that produce or consume resources!"<<std::endl;
        }
    } else {
        throw(1); //bad command
    }
}


//gives information about a particular unit
void world::process_info_command(std::vector<std::string> list)
{
    if(list.size() == 2) {
        size_t found_unit_id{find_object_in_list<mobile_object>(units, list[1])};

        std::cout<<"Name: "<<list[1]<<std::endl;
        std::cout<<"Health: "<<units[found_unit_id]->get_health()<<std::endl;
        std::cout<<"Attack: "<<units[found_unit_id]->get_attack()<<std::endl;
        std::cout<<"Range: "<<units[found_unit_id]->get_range()<<std::endl;
        if(units[found_unit_id]->check_if_attacked()){
            std::cout<<"Already attacked: YES"<<std::endl;
        } else {
            std::cout<<"Already attacked: NO"<<std::endl;
        }
        std::cout<<"Max momement: "<<units[found_unit_id]->get_max_movement()<<std::endl;
        std::cout<<"Remaining movement: "<<units[found_unit_id]->get_movement()<<std::endl;
        std::cout<<"Cannot go across: ";

        std::vector<std::string> forbidden{units[found_unit_id]->get_forbidden_cells()};
        for(size_t i{0}; i <= forbidden.size(); i++) {
            std::cout<<forbidden[i]<<"  ";
        }
        std::cout<<"\n";
    }
    else {
        throw(1); //bad command
    }
}


//takes care of changes due to a new turn
void world::process_turn_command(std::vector<std::string> list) 
{
    if(list.size() == 1) {
        for(size_t i{0}; i != units.size(); i++) {
            units[i]->turn(); //restores movement and attack
        }
        
        if(monsters.size() > 0) {
            for(size_t i{0}; i != monsters.size(); i++) {
                if(choose_monster_target(i) == false) { //checks if monster can attack someone
                    monster_random_walk(i); //otherwise, random walk
                }
                monsters[i]->turn(); //restores movement and attack
            }
        }

        spread_monsters(); //to restore any dead monsters (keep their density constant); will go down as the Forest cells are getting cut down

        resources_database input_output = get_total_resource_change();
        change_resources(input_output, 1); //changes player's resources due to input/output from mines and factories
        show_world(); //updates the map
    }
    else {
        throw(1); //bad command
    }
}