#include "world.h"

std::vector<std::string> world::command_list{"move", "attack", "build", "buildings", "clear", "turn", "end", "info", "clone", "units", "resources", "show", "help"};
help_commands world::help_commands_list{};

//initialise list of commands that appear when you type help
void world::declare_help_commands() 
{
    help_commands_list["move"] =      "move [unit_name] [x_coord] [y_coord]      moves a selected unit to xy";
    help_commands_list["attack"] =    "attack [unit_name] [x_coord] [y_coord]    selected unit attacks enemy at xy";
    help_commands_list["build"] =     "build [unit_name] [building_name]         selected worker builds something; if no building name entered, lists available options";
    help_commands_list["buildings"] = "buildings                                 lists all buildings and their resource consumption / production";
    help_commands_list["clear"] =     "clear [unit_name]                         selected worker clears the forest";
    help_commands_list["turn"] =      "turn                                      next turn";
    help_commands_list["end"] =       "end                                       finish game";
    help_commands_list["help"] =      "help [command]                            gives help";
    help_commands_list["info"] =      "info [unit_name]                          details of the selected unit (health, etc)";
    help_commands_list["units"] =     "units                                     lists all available units";
    help_commands_list["show"] =      "show                                      updates the map (is NOT end of turn)";
    help_commands_list["resources"] = "resources                                 shows available resources";
    help_commands_list["clone"] =     "clone [unit_name]                         if without name, shows list of units available for cloning; if with name, generates a new unit";
    
    help_commands_list["z_symbols"] = "\n~ water \n# ground \n| forest";
    help_commands_list["z_units"] = "\nM monster \nW worker \nS solder \nFi fire brigade";
    help_commands_list["z_buildings"] = "\nA airship \nF factory \nT defence tower \nMi mine";
    help_commands_list["z_resources"] = "\nCo coal \nSt strongium (some strong metal) \nAl aluminium \nOi oil";
}

//initialise database that stores what resources are needed for unit cloning
void world::declare_clonding_database()
{
    resources_database worker_resources;
    worker_resources["strongium"] = 5;

    resources_database soldier_resources;
    soldier_resources["strongium"] = 10;

    resources_database flamethrower_trooper_resources;
    flamethrower_trooper_resources["strongium"] = 10;
    flamethrower_trooper_resources["aluminium"] = 5;

    resources_database sniper_resources;
    sniper_resources["strongium"] = 15;
    sniper_resources["aluminium"] = 3;

    all_cloning_list["worker"] = worker_resources;
    all_cloning_list["soldier"] = soldier_resources;
    all_cloning_list["flamethrower_trooper"] = flamethrower_trooper_resources;
    all_cloning_list["sniper"] = sniper_resources;
}

//initialise database that stores what resources are needed for building
void world::declare_buildings_database()
{
    resources_database mine_resources;
    mine_resources["strongium"] = 1;

    resources_database tower_resources;
    tower_resources["strongium"] = 5;

    resources_database strongium_factory_resources;
    strongium_factory_resources["raw_strongium"] = 3;
    strongium_factory_resources["coal"] = 1;

    resources_database aluminium_factory_resources;
    aluminium_factory_resources["raw_aluminium"] = 3;
    aluminium_factory_resources["coal"] = 1;

    all_building_list["mine"] = mine_resources;
    all_building_list["factory_strongium"] = strongium_factory_resources;
    all_building_list["factory_aluminium"] = aluminium_factory_resources;
    all_building_list["defence_tower"] = tower_resources;
}

//initialise player's resources
void world::add_initial_resources()
{
    resources_list["coal"] = 5;
    resources_list["oil"] = 0;
    resources_list["aluminium"] = 0;
    resources_list["strongium"] = 5;

    resources_list["raw_strongium"] = 0;
    resources_list["raw_aluminium"] = 0;
}