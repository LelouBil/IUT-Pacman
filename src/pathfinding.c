#include "./pathfinding.h"

int get_hcost(Case* c);
int get_cheapest_node();
void path_finder( Partie* p);
void check_neighbours(Node* n, Partie* p);
int closed_node_in_case(Case* c);
int open_node_in_case(Case* c);
void set_cost(Node* n, Node* neightbour, Partie* p);

int open_node_index = 0;
int closed_node_index = 0;

static Node* path[array_max];

Case* path_init(Case* start, Partie* p){
    for (int i = 0; i < array_max; ++i) {
        open_node[i] = NULL;
        closed_node[i] = NULL;
    }

    Node* start_node = open_node[0];
    start_node->node_case = start;
    start_node->f_cost = abs(p->pacman.case_pacman->x - start->x) + abs(p->pacman.case_pacman->y - start->y);
    start_node->h_cost = abs(p->pacman.case_pacman->x - start->x) + abs(p->pacman.case_pacman->y - start->y);
    start_node->g_cost = 0;
    start_node->parent = NULL;


    closed_node_index = 0;

    path_finder(p);

    return path[0]->node_case;
}

void path_finder(Partie* p){
    while(1) {
        for (int i = 0; i < array_max; ++i) {
            path[i] = NULL;
        }

        Node *n = open_node[get_cheapest_node()];
        open_node[get_cheapest_node()] = NULL;
        closed_node[++closed_node_index] = n;

        if ((n->node_case->x == p->pacman.case_pacman->x) && (n->node_case->y == p->pacman.case_pacman->y)) {
            return;
        }

        check_neighbours(n, p);
    }
}


void check_neighbours(Node* n, Partie* p){

    for (int dir = DIR_HAUT; dir < DIR_GAUCHE + 1; ++dir) {
        Case* target = get_case_at(p, n->node_case, dir);
        if(!target->wall && closed_node_in_case(target) == -1) {
            if (open_node_in_case(target) == -1 || n->f_cost < open_node[open_node_in_case(target)]->f_cost) {
                if (open_node_in_case(target) == -1) {
                    Node* neighbour = open_node[++open_node_index];
                    neighbour->node_case = target;
                    neighbour->parent = n;
                    set_cost(n, neighbour, p);
                } else {
                    Node *neighbour = open_node[open_node_in_case(target)];
                    set_cost(n, neighbour, p);
                    neighbour->parent = n;

                }
            }
        }
    }

    path_finder(p);
}

int get_cheapest_node(){
   int max_f_cost = array_max, index;

    for (int i = 0; i < array_max; ++i) {
        if(open_node[index]->f_cost<max_f_cost){index = i;}
    }

    return index;
}

int closed_node_in_case(Case* c){
    for (int i = 0; i < array_max; ++i) {
        if(closed_node[i] != NULL && (closed_node[i]->node_case->x == c->x && closed_node[i]->node_case->y == c->y)){
            return i;
        }
    }
    return -1;
}

int open_node_in_case(Case* c){
    for (int i = 0; i < array_max; ++i) {
        if(open_node[i] != NULL && (open_node[i]->node_case->x == c->x && open_node[i]->node_case->y == c->y)){
            return i;
        }
    }
    return -1;
}

void set_cost(Node* n, Node* neightbour, Partie* p){
    neightbour->g_cost = n->g_cost + 1;

    neightbour->h_cost = abs(p->pacman.case_pacman->x - n->node_case->x) + abs(p->pacman.case_pacman->y - n->node_case->y);

    neightbour->f_cost = neightbour->h_cost + neightbour->g_cost;
}