#ifndef _ROBO15_ALGORITHM_H_
#define _ROBO15_ALGORITHM_H_

#include "Configuration.h"

//Absolute coordinates
typedef struct coord{
    int x;
    int y;
} coord;

struct coord current_position;

typedef struct node *maze;
typedef struct node{
    struct coord position;
    maze compass[4];
    int visited;
    int bfs_reached_from;
} node;
struct node *current_node;


//einfach verkette Liste als Warteschlange im bfs
typedef struct element *list;
typedef struct element{
    struct coord node_position;
    struct element *next;
} element;

typedef struct instructions{
    int path[50];
} instructions;

void list_append(struct element **start, struct coord discovered);

void list_remove_first(struct element **start);

void destroy_list(struct element *start);

int list_search(struct element *start, struct coord tofind);

void reset_nodes_bfs();

void init();

void discover();

int direction_detect(int given_intersection, int wanted_direction);

void turn_d(int direction);

struct node *create_node();

struct coord bfs_closest_unvisited_node();

struct instructions *bfs_path_to_node(struct coord goal);

struct coord shift_coordinates(struct coord old, int direction);

struct instructions *create_path(struct coord goal_position);

int follow_instructions(struct instructions *instr);

void print_ptrmap();

void print_bfs_rf_ptrmap();

#endif //_ROBO15_ALGORITHM_H_
