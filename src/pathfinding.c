#include "./pathfinding.h"
#include <signal.h>
#include "../lib/libgraphique.h"
#include "utils.h"
#include "display.h"

typedef enum {
    PATH_FOUND,
    PATH_DISTANCE_REACHED,
    PATH_MAX_ITER_REACHED,
    NO_PATH_FOUND
} pathcode;

typedef struct Node {
    int empty;
    Case *node_case;
    int f_cost; //sum of h_cost and g_cost
    int h_cost; //dist from finish (static straight line)
    int g_cost; //dist from start (following neighbours, this is dynamic)
    struct Node *parent;
} Node;



int get_cheapest_node();

int manhattan_distance(const Case *aCase, const Case *b, const Partie *p);

pathcode path_finder(Partie *p, int avoid);

int get_g_cost(Node *parent, Partie *partie, Node *neightbour);

void check_neighbours(Node *parent, Partie *p, int avoid);

int closed_node_in_case(Case *c);

int open_node_in_case(Case *c);

void set_cost(Node *node, Node *parent, Partie *p, int i);

int get_f_cost(int nodeI, Node *parent, Partie *partie);


int open_node_index = 0;
int closed_node_index = 0;


Node open_node[array_max];
Node closed_node[array_max];


int path_init(Case *start, Partie *p, int avoid) {
    if (meme_case(start, p->pacman.case_pacman)) {
        return -1;
    }
    for (int i = 0; i < array_max; ++i) {
        open_node[i] = (Node) {1};
        closed_node[i] = (Node) {1};
    }

    Node *start_node = &open_node[0];
    start_node->node_case = start;
    start_node->empty = 0;

    start_node->h_cost = manhattan_distance(start_node->node_case, p->pacman.case_pacman, p);
    start_node->g_cost = 0;
    start_node->f_cost = start_node->h_cost;
    start_node->parent = NULL;


    closed_node_index = 0;
    open_node_index = 0;


    pathcode path = path_finder(p, avoid);

    Node cur = closed_node[closed_node_index];
    if (path == NO_PATH_FOUND) {
        return -1;
    }
    int index, max_f_cost;

    switch (path) {
        case NO_PATH_FOUND:
            return -1;
        case PATH_MAX_ITER_REACHED:
            max_f_cost = cur.f_cost;
            index = -1;

            for (int i = 0; i < closed_node_index + 1; ++i) {
                if (closed_node[i].empty) continue;
                if (closed_node[i].f_cost <= max_f_cost) {
                    index = i;
                    max_f_cost = closed_node[i].f_cost;
                }
            }

            cur = closed_node[index];
            break;
        case PATH_DISTANCE_REACHED:
        case PATH_FOUND:
            while (cur.parent != NULL && !meme_case(cur.parent->node_case, start)) {
                cur = *cur.parent;
            }
    }


    return dir_from_to(start, cur.node_case, p);
}

int path_panic(Case* c, Partie *partie, int original_dir){
    int for_offset = entier_aleatoire(4);

    for (int i = for_offset; i < for_offset + 4; ++i) {
        if(!get_case_at(partie,c,i%4)->wall){
            if(original_dir != (i + 2)%4) {
                return i % 4;
            }
        }
    }
    return -1; //should never happen
}

void dessiner_parent(Node *node, Partie *p) {
    char fcost[3];
    sprintf(fcost, "%d", node->h_cost);
    afficher_texte(fcost, 10, to_point(get_case_center(node->node_case)), black);
    if (node->parent != NULL) {
        int dir = dir_from_to(node->parent->node_case, node->node_case, p);

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

pathcode path_finder(Partie *p, int avoid) {
    extern int pathfinding_debug;
    int iter = 0;
    while (1) {

        if (pathfinding_debug == 1) {
            dessiner_plateau(p);
            dessiner_entities(p);
            for (int x = 0; x < p->xmax; x++) {
                for (int y = 0; y < p->ymax; y++) {
                    Case *aCase = &p->plateau[x][y];
                    int oc = open_node_in_case(aCase);
                    if (oc != -1) {
                        Node *node = &open_node[oc];

                        remplir_case(aCase, green);

                        dessiner_parent(node, p);

                    }
                    int cc = closed_node_in_case(aCase);
                    if (cc != -1) {
                        Node *node = &closed_node[cc];
                        remplir_case(aCase, red);
                        dessiner_parent(node, p);

                    }

                }
            }
            actualiser();
            //pause
            printf("wait\n");
            attendre_clic();
            printf("stopwait\n");
        }
        int ni = get_cheapest_node();
        if (ni == -1) return 0;
        Node *n = &open_node[ni];
        n->empty = 1;
        closed_node[++closed_node_index] = *n;
        closed_node[closed_node_index].empty = 0;

        if (avoid) {
            if (manhattan_distance(n->node_case, p->pacman.case_pacman, p) >= FLEE_MAX_PATH_DISTANCE) {
                return PATH_DISTANCE_REACHED;
            } else if (iter > FLEE_MAX_PATH_ITER) {
                return PATH_MAX_ITER_REACHED;
            }
        } else {
            if (manhattan_distance(n->node_case, p->pacman.case_pacman, p) == 0) {
                return PATH_FOUND;
            }
        }

        check_neighbours(n, p, avoid);
        iter++;
    }
}


int has_fantome(Case *target, Partie *p) {
    int f = 0;
    for (int i = 0; i < NBFANTOMES; ++i) {
        if (meme_case(target, p->fantomes[i].case_fantome))f++;
    }
    return f;
}

void check_neighbours(Node *parent, Partie *p, int avoid) {

    for (direction dir = DIR_HAUT; dir < DIR_GAUCHE + 1; ++dir) {
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
                set_cost(neighbour, parent, p, avoid);
            } else {
                Node *neighbour = &open_node[open_node_in_case(target)];
                set_cost(neighbour, parent, p, avoid);
                neighbour->parent = parent;

            }
        }

    }
}

int get_f_cost(int nodeI, Node *parent, Partie *partie) {
    Node *node = &open_node[nodeI];
    return get_g_cost(parent, partie, node) + manhattan_distance(node->node_case, partie->pacman.case_pacman, partie);
}

int get_cheapest_node() {
    int max_f_cost = open_node[open_node_index].f_cost;
    int index = -1;

    for (int i = 0; i < open_node_index + 1; ++i) {
        if (open_node[i].empty) continue;
        if (open_node[i].f_cost <= max_f_cost) {
            index = i;
            max_f_cost = open_node[i].f_cost;
        }
    }

    return index;
}

int closed_node_in_case(Case *c) {
    for (int i = 0; i < closed_node_index + 1; ++i) {
        if (!closed_node[i].empty && (closed_node[i].node_case->x == c->x && closed_node[i].node_case->y == c->y)) {
            return i;
        }
    }
    return -1;
}

int open_node_in_case(Case *c) {
    for (int i = 0; i < open_node_index + 1; ++i) {
        if (!open_node[i].empty && (open_node[i].node_case->x == c->x && open_node[i].node_case->y == c->y)) {
            return i;
        }
    }
    return -1;
}

int get_g_cost(Node *parent, Partie *partie, Node *neightbour) {
    return parent->g_cost + 1;
}

void set_cost(Node *node, Node *parent, Partie *p, int i) {
    node->g_cost = get_g_cost(parent, p, node);

    node->h_cost = manhattan_distance(node->node_case, p->pacman.case_pacman, p);
    if (i) node->h_cost *= -1;

    node->f_cost = node->h_cost + node->g_cost;
}

int min(int a, int b) {
    return a < b ? a : b;
}


// https://stackoverflow.com/questions/23580083/manhattan-distance-for-a-2d-toroid

int manhattan_distance(const Case *aCase, const Case *b, const Partie *p) {
    int walk_mhdx = abs(aCase->x - b->x);
    int walk_mhdy = abs(aCase->y - b->y);

    int wrap_mhdx = p->xmax - walk_mhdx;
    int wrap_mhdy = p->ymax - walk_mhdy;


    int dx = min(walk_mhdx, wrap_mhdx);
    int dy = min(walk_mhdy, wrap_mhdy);

    return dx + dy;
}
