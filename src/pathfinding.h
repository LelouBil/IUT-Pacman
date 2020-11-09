#pragma once

#include "partie.h"
#include "utils.h"
#include "display.h"


#define PATHFINDER_INFINITE 100

#define PATHFINDER_DEBUG_LEVEL 1

typedef struct Node Node;

struct Node{
    Case* case_node;
    int step;
    int dir;
    Node* next_node;
    Node* previous_node;
};

Node* node_matrix[21][27];


int path_init(Case* pos, Partie* p);