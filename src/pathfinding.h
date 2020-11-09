#pragma once

#include "partie.h"
#include "utils.h"
#include "display.h"


#define PATHFINDER_DEBUG_LEVEL 1
#define array_max 100

typedef struct Node Node;

struct Node{
    Case* node_case;
    int f_cost; //sum of h_cost and g_cost
    int h_cost; //dist from finish (static straight line)
    int g_cost; //dist from start (following neighbours, this is dynamic)
    Node* parent;
};

Node* open_node[array_max];
Node* closed_node[array_max];

Case* path_init(Case *start, Partie* p);