#include "./pathfinding.h"
#include <signal.h>
#include "../lib/libgraphique.h"
#include "display.h"




typedef struct Node {
    int empty;
    Case* node_case;
    int f_cost; //sum of h_cost and g_cost
    int h_cost; //dist from finish (static straight line)
    int g_cost; //dist from start (following neighbours, this is dynamic)
    struct Node* parent;
} Node;


int get_hcost(Case *c);

int get_cheapest_node();

int path_finder(Partie *p);

int get_g_cost(Node *parent);

void check_neighbours(Node *parent, Partie *p);

int closed_node_in_case(Case *c);

int open_node_in_case(Case *c);

void set_cost(Node *n, Node *neightbour, Partie *p);

int get_f_cost(int nodeI, Node *parent, Partie *partie);

int get_h_cost(const Node *n, const Partie *p);

int open_node_index = 0;
int closed_node_index = 0;


Node open_node[array_max];
Node closed_node[array_max];


int path_init(Case *start, Partie *p) {
    if (meme_case(start,p->pacman.case_pacman)){
        return -1;
    }
    for (int i = 0; i < array_max; ++i) {
        open_node[i] = (Node) {1};
        closed_node[i] = (Node) {1};
    }

    Node *start_node = &open_node[0];
    start_node->node_case = start;
    start_node->empty = 0;
    start_node->f_cost = abs(p->pacman.case_pacman->x - start->x) + abs(p->pacman.case_pacman->y - start->y);
    start_node->h_cost = abs(p->pacman.case_pacman->x - start->x) + abs(p->pacman.case_pacman->y - start->y);
    start_node->g_cost = 0;
    start_node->parent = NULL;


    closed_node_index = 0;
    open_node_index = 0;


    int path = path_finder(p);

    if(path == 0){
        return -1;
    }


    Node cur = closed_node[closed_node_index];



    while(!meme_case(cur.parent->node_case,start)){
        cur = *cur.parent;
    }


    return dir_from_to(start,cur.node_case);
}

void dessiner_parent(Node *node){
    if(node->parent != NULL) {
        int dir = dir_from_to(node->parent->node_case, node->node_case);

        Point center = to_point(get_case_center(node->node_case));

        switch (dir) {
            case DIR_BAS:
                center.y += 10;
                break;
            case DIR_HAUT:
                center.y -= 10;
                break;
            case DIR_GAUCHE:
                center.x += 10;
                break;
            case DIR_DROITE:
                center.x -= 10;
                break;

        }
        dessiner_disque(center, 2, white);
    }
}

int path_finder(Partie* p){
    while(1) {

//        for (int x = 0; x < p->xmax; x++) {
//            for (int y = 0; y < p->ymax; y++) {
//                Case *aCase=  &p->plateau[x][y];
//                int oc = open_node_in_case(aCase);
//                if (oc != -1){
//                    Node *node = &open_node[oc];
//
//                    remplir_case(aCase,green);
//
//                    dessiner_parent(node);
//
//                }
//                int cc = closed_node_in_case(aCase);
//                if (cc != -1){
//                    Node *node = &closed_node[cc];
//                    remplir_case(aCase,red);
//                    dessiner_parent(node);
//
//                }
//
//            }
//        }
//        actualiser();
//        //pause
//        printf("wait\n");
//        attendre_clic();
//        printf("stopwait\n");

        int ni = get_cheapest_node();
        if (ni == -1) return 0;
        Node *n = &open_node[ni];
        n->empty = 1;
        closed_node[++closed_node_index] = *n;
        closed_node[closed_node_index].empty = 0;

        if ((n->node_case->x == p->pacman.case_pacman->x) && (n->node_case->y == p->pacman.case_pacman->y)) {
            return 1;
        }

        check_neighbours(n, p);
    }
}


int has_fantome(Case *target, Partie *p) {
    int f = 0;
    for (int i = 0; i < NBFANTOMES; ++i) {
        if (meme_case(target,p->fantomes[i].case_fantome))f++;
    }
    return f;
}

void check_neighbours(Node *parent, Partie *p) {

    for (int dir = DIR_HAUT; dir < DIR_GAUCHE + 1; ++dir) {
        Case *target = get_case_at(p, parent->node_case, dir);
        if (target == NULL || target->wall || closed_node_in_case(target) != -1) {
            continue;
        }

        if (open_node_in_case(target) == -1 || get_f_cost(open_node_in_case(target), parent, p) <
                                               open_node[open_node_in_case(target)].f_cost) {
            if (open_node_in_case(target) == -1) {
                Node *neighbour = &open_node[++open_node_index];
                neighbour->empty = 0;
                neighbour->node_case = target;
                neighbour->parent = parent;
                set_cost(parent, neighbour, p);
            } else {
                Node *neighbour = &open_node[open_node_in_case(target)];
                set_cost(parent, neighbour, p);
                neighbour->parent = parent;

            }
        }

    }
}

int get_f_cost(int nodeI, Node *parent, Partie *partie) {
    Node *node = &open_node[nodeI];
    return get_g_cost(parent) + get_h_cost(node, partie);
}

int get_cheapest_node() {
    int max_f_cost = -1;
    int index = -1;

    for (int i = 0; i < array_max; ++i) {
        if (open_node[i].empty) continue;
        if (max_f_cost == -1 || open_node[i].f_cost < max_f_cost ) {
            index = i;
            max_f_cost = open_node[i].f_cost;
        }
    }

    return index;
}

int closed_node_in_case(Case *c) {
    for (int i = 0; i < array_max; ++i) {
        if (!closed_node[i].empty && (closed_node[i].node_case->x == c->x && closed_node[i].node_case->y == c->y)) {
            return i;
        }
    }
    return -1;
}

int open_node_in_case(Case *c) {
    for (int i = 0; i < array_max; ++i) {
        if (!open_node[i].empty && (open_node[i].node_case->x == c->x && open_node[i].node_case->y == c->y)) {
            return i;
        }
    }
    return -1;
}

int get_g_cost(Node *parent) {
    return parent->g_cost + 1;
}

void set_cost(Node *n, Node *neightbour, Partie *p) {
    neightbour->g_cost = get_g_cost(n);

    neightbour->h_cost = get_h_cost(n, p);

    neightbour->f_cost = neightbour->h_cost + neightbour->g_cost;
}

int get_h_cost(const Node *n, const Partie *p) {
    return abs(p->pacman.case_pacman->x - n->node_case->x) + abs(p->pacman.case_pacman->y - n->node_case->y);
}
