#include "../h/algorithm.h"


void init(){
    int i, j;
    for(i = 0; i < 2 * MAZE_WIDTH; i++){
        for(j = 0; j < MAZE_HEIGHT + 2; j++) {
            ptrmap[i][j] = NULL;
        }
    }
    current_node = create_node();
    ptrmap[current_position.x + MAZE_WIDTH][current_position.y + MAZE_HEIGHT] =
            current_node;

    global_instructions = malloc(sizeof(struct instructions));

    tokencount = 0;
    discovered_everything = 0;
}


//Discover surrounding connected nodes and create them in map
void discover(){

    if (!current_node->visited){
        int new_intersection = 0;
        new_intersection = scan();

        //go through all possible directions, look if there are undiscovered
        // nodes nearby
        int i;
        for (i = 0; i <= 3; i++) {
            struct coord neighbour_position =
                    shift_coordinates(current_position,i);
            if (direction_detect(new_intersection, i)) {
                if (ptrmap[neighbour_position.x + MAZE_WIDTH]
                          [neighbour_position.y + MAZE_WIDTH]
                            == NULL) {
                    struct node *discovered_node = create_node();
                    //update ptrmap with new node
                    ptrmap[neighbour_position.x + MAZE_WIDTH]
                          [neighbour_position.y + MAZE_HEIGHT] =
                            discovered_node;
                    discovered_node->position = neighbour_position;
                }
                //set up pointers of current and new node, fill in new nodes position
                current_node->compass[i] = ptrmap[neighbour_position.x + MAZE_WIDTH]
                                                 [neighbour_position.y +
                                                                    MAZE_HEIGHT];
                ptrmap[neighbour_position.x + MAZE_WIDTH]
                      [neighbour_position.y + MAZE_HEIGHT]
                        ->compass[(i+2) % 4] = current_node;
            }

        }
        current_node->visited = 1; //current node is explored
        if (direction_detect(new_intersection, (orientation+3)%4)) printf
            ("LINKS\n");
        if (direction_detect(new_intersection, orientation)) printf
                    ("GERADEAUS\n");
        if (direction_detect(new_intersection, (orientation+1)%4)) printf
                    ("RECHTS\n");
        printf("---------------------------------\n");
    }
}


//Returns 1 if the wanted cardinal direction is available at a intersection,
//0 if not
int direction_detect(int given_intersection, int wanted_direction) {
    //Northern node?
    if ( ((given_intersection >> 4)&1) && (wanted_direction == NORTH ||
                                            wanted_direction == 0)) {
        return 1;
    }

    //Eastern node?
    else if (((given_intersection >> 7)&1) && (wanted_direction == EAST ||
                                                wanted_direction == 1)) {
        return 1;
    }

    //Southern node?
    else if (((given_intersection >> 5)&1) && (wanted_direction == SOUTH ||
                                                wanted_direction == 2)) {
        return 1;
    }

    //Western node?
    else if (((given_intersection >> 6)&1) && (wanted_direction == WEST ||
                                                wanted_direction == 3)){
        return 1;
    }
    return 0;
}

//turn bot in a specific direction
void turn_d(int direction){
    int diff = ((orientation - direction + 4)%4);
    switch (diff){
        case 0:
            break;
        case 1:
            turn_left();
            break;
        case 2:
            turn_left();
            turn_left();
            break;
        case 3:
            turn_right();
            break;
    }
    orientation = direction;
}


//return pointer to new, empty node
struct node *create_node(){
    struct node *new_node;
    new_node = malloc(sizeof(struct node));
    new_node->visited = 0;
    int i;
    for (i=0; i<4; i++){
        new_node->compass[i]=NULL;
    }
    new_node->position.x = 0;
    new_node->position.y = 0;
    return new_node;
}

//look for graph-wise closest unvisited node and return its coord
struct coord bfs_closest_unvisited_node() {
    int i;
    struct element  *queue;
    queue = NULL;
    struct element *seen;
    seen = NULL;
    struct coord wanted;
    struct coord position;
    list_append(&queue,current_position);
    struct coord neighbour_position;
    do {
        position = queue->node_position;
        list_append(&seen, position);
        for (i = 3; i >= 0; i--) {
            neighbour_position = shift_coordinates(position,i);

            if ((ptrmap[position.x + MAZE_WIDTH][position.y +
                    MAZE_HEIGHT])->compass[i] != NULL) {
                if (ptrmap[position.x + MAZE_WIDTH][position.y + MAZE_HEIGHT]
                        ->compass[i]->visited == 0) {
                    wanted = shift_coordinates(position, i);
                    destroy_list(queue);
                    destroy_list(seen);
                    return wanted; //return the coords of found node
                }

                if (!list_search(seen,neighbour_position) &&
                        !list_search(queue,neighbour_position)){
                    list_append(&queue, neighbour_position);
                }
            }
        }
        list_remove_first(&queue);
    } while (queue != NULL);
    discovered_everything = 1;
    return current_position;
}

//look for shortest path to given coordinates
struct instructions *bfs_path_to_node(struct coord goal) {
    int i;
    struct element  *queue;
    queue = NULL;
    struct element *seen;
    seen = NULL;
    struct coord cur_position;
    cur_position = current_position;
    current_node->bfs_reached_from = 8;
    list_append(&queue,cur_position);

    struct coord neighbour_position;

    while (queue != NULL) {
        list_append(&seen, cur_position);
        if ((cur_position.x  == goal.x) && (cur_position.y == goal.y)){
            global_instructions = create_path(goal);
            reset_nodes_bfs();
            destroy_list(queue);
            destroy_list(seen);
            return global_instructions;
        }

        for (i = 3; i >= 0; i--) {
            neighbour_position = shift_coordinates(cur_position,i);
            if ((ptrmap[cur_position.x + MAZE_WIDTH]
                       [cur_position.y + MAZE_HEIGHT])->compass[i] != NULL) {
                if ((!list_search(seen,neighbour_position)) &&
                        (!list_search(queue,neighbour_position))){
                    list_append(&queue, neighbour_position);
                    ptrmap[neighbour_position.x + MAZE_WIDTH]
                          [neighbour_position.y + MAZE_HEIGHT]
                            ->bfs_reached_from = i;
                }
            }
        }

        list_remove_first(&queue);
        cur_position = queue->node_position;
    }

    global_instructions->path[0] = 8;
    return global_instructions;
}

//appends the list the given pointer is pointing to with the given coord
void list_append(struct element **start, struct coord discovered) {

    struct element **previous = start;
    struct element *current = *start;
    while (current != NULL) {
        previous = &(current->next);
        current = current->next;
    }
    struct element *new_element = malloc(sizeof(struct element));
    new_element->node_position = discovered;
    new_element->next = NULL;
    *previous = new_element;
}

//remove the first entry of a list and update the pointer the dp is pointing to
void list_remove_first(struct element **start) {

    struct element *temp;

    if (*start == NULL) {
        return;
    }
    temp = *start;
    *start = temp->next;
    free(temp);
}


//completely free the list given at start
void destroy_list (struct element *start) {
    while (start != NULL) {
        struct element *next = start->next;
        free(start);
        start = next;
    }
}

int list_search(struct element *start, struct coord tofind) {
    struct element *iter = start;
    while (iter != NULL){
        if (iter->node_position.x == tofind.x &&
                iter->node_position.y == tofind.y) {
            return 1;
        }
        iter = iter->next;
    }
    return 0;
}

void reset_nodes_bfs(){
    int i, j;
    for(i = 0; i < 2 * MAZE_WIDTH; i++){
        for(j = 0; j < MAZE_HEIGHT + 2; j++) {
            if (ptrmap[i][j] != NULL) {
                ptrmap[i][j]->bfs_reached_from = 9;
            }
        }
    }
}

//translates iterable and pre-defined directions and returns the respective
// coords
struct coord shift_coordinates(struct coord old, int direction){
    switch (direction) {
        case 0:
            old.y += 1;
            return old;
        case NORTH:
            old.y += 1;
            return old;
        case 1:
            old.x += 1;
            return old;
        case EAST:
            old.x += 1;
            return old;
        case 2:
            old.y -= 1;
            return old;
        case SOUTH:
            old.y -= 1;
            return old;
        case 3:
            old.x -= 1;
            return old;
        case WEST:
            old.x -= 1;
            return old;
        default: return old;
    }
}

int follow_instructions(struct instructions *instr){
    int i;
    for (i = 0; instr->path[i] != 8; i++){
        turn_d(instr->path[i]);
        go_straight();
    }
    return ROBOT_SUCCESS;
}

struct instructions *create_path(struct coord goal_position){
    struct instructions temp;
    //print_bfs_rf_ptrmap();
    int i;
    for (i=0; (goal_position.x != current_position.x)
              || (goal_position.y != current_position.y); i++){
        temp.path[i] = ptrmap[goal_position.x + MAZE_WIDTH]
                        [goal_position.y + MAZE_HEIGHT]->bfs_reached_from;
        if (temp.path[i] == 9) {
        }
        goal_position = shift_coordinates(goal_position, (temp.path[i] + 2) % 4);
    }
    int j;
    for (j = 0, i -= 1; i >= 0; i--, j++){
        global_instructions->path[j] = temp.path[i];
    }
    global_instructions->path[j] = 8;
    return global_instructions;
}


//outputs ptrmaps bfs_reached_from to stdout
void print_bfs_rf_ptrmap(){
    int i, j;
    for(j =  0; j < MAZE_HEIGHT + 2; j++){
        for(i = 0; i < 2 * MAZE_WIDTH; i++) {
            if (ptrmap[i][j] != NULL) {
                printf("%d ", ptrmap[i][j]->bfs_reached_from);
            } else {
                printf("X ");
            }
        }
        printf("\n");
    }
}
