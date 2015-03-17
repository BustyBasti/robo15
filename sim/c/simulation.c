#include "../h/simulation.h"

//Place the bot at a (pseudo) random, valid position
//for better simulation of real start conditions
void init_sim_rnd() {

    srand(time(NULL));
    int r;

    //yields an int between 0 and MAZE_WIDTH(inclusive)
    r = rand() % (MAZE_WIDTH + 1);

    if(Robot_Move(r,0)==ROBOT_FAIL) {
        printf("Trying different position...\n");
        init_sim_rnd(); //recursion is its own reward
        //TODO: abort conditions!
    } else {
        x_pos = r; //Update position on successful placement
        y_pos = 0;

        orientation = 8;
        for (r = rand() % 4; r >= 0; r--){
            orientation = orientation * 2; //yields random direction
        }
    }
}

int turn_left(){
    switch (orientation){
        case NORTH: orientation = WEST;
            break;
        case EAST: orientation = NORTH;
            break;
        case SOUTH: orientation = EAST;
            break;
        case WEST: orientation = SOUTH;
            break;
        default:
            return ROBOT_FAIL;
    }
    return ROBOT_SUCCESS;
}

int turn_right(){
    switch (orientation){
        case NORTH: orientation = EAST;
            break;
        case EAST: orientation = SOUTH;
            break;
        case SOUTH: orientation = WEST;
            break;
        case WEST: orientation = NORTH;
            break;
        default:
            return ROBOT_FAIL;
    }
    return ROBOT_SUCCESS;
}

//go forward in current orientation, regardless of edge or no edge present
int go_straight(){
    return move_d(orientation);
}


//Move in a given direction, no detection/abort in case of missing edge!
int move_d(int direction){
    switch (direction){
        case NORTH: y_pos += 1;
            break;
        case EAST: x_pos += 1;
            break;
        case SOUTH: y_pos -= 1;
            break;
        case WEST: x_pos -= 1;
            break;
    }
    return(Robot_Move(x_pos, y_pos));
}

//return intersection at current position
int scan(){
    return Robot_GetIntersections();
}