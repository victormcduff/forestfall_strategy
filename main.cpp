#include "world.h"

int main() {
    try {
        world world; //create world object
        world.enter_world_size_and_seed();
        world.generate_terrain();
        world.show_world();

        while(world.get_play()) { //until the player deliberately enters END, will keep on playing
            world.enter_command(); //main loop: execute player's commands, go to the turn, repeat
        }
    }
    catch(std::bad_alloc memFail) { //checks if the world dimensions are reasonable
        std::cerr<<"Error! The world is too big."<<std::endl;
    }

    return 0;
}

