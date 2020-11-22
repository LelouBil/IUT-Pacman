#include "./pathfinding.h"
#include <signal.h>
#include "../lib/libgraphique.h"
#include "utils.h"
#include "display.h"

/* pathfinding avec algorithme A*
 *
 * 
 * il consiste a calculers plusieurs poids pour chaque noeuds voisin attegnable
 *
 * Les poids sont les suivants:
 * score h : distance manhattan entre le noeud et la destination 
 * scord g : distance la plus courte en suivant des noeuds de debut au noeud courant
 * score f : somme des scores g et h
 *
 * le pathfiniding A* preferera les noeud avec le plus faible score f
 *
 * lorsqu un chemin est trouve on peut le retracer grace a la propriete parent de noeud
 */

//cas de sortie possible du path finder
typedef enum {
    PATH_FOUND,
    PATH_DISTANCE_REACHED,
    PATH_MAX_ITER_REACHED,
    PATH_NOT_FOUND
} pathcode;

// noeuds du path finding
typedef struct Node {
    int empty;
    Case *node_case;
    int f_cost; //sum of h_cost and g_cost
    int h_cost; //dist from finish (static straight line)
    int g_cost; //dist from start (following neighbours, this is dynamic)
    struct Node *parent;
} Node;



int get_cheapest_node();

pathcode path_finder(Partie *p, int avoid, Case *goal);

int get_g_cost(Node *parent, Partie *partie, Node *neightbour);

void check_neighbours(Node *parent, Partie *p, int avoid, Case *goal);

int closed_node_in_case(Case *c);

int open_node_in_case(Case *c);

void set_cost(Node *node, Node *parent, Partie *p, Case *goal);

int get_f_cost(int nodeI, Node *parent, Partie *partie, Case *goal);


int open_node_index = 0;
int closed_node_index = 0;


Node open_node[array_max]; //liste des noeuds ouverts
Node closed_node[array_max]; //liste des noeuds fermes


direction path_init(Case *start, Partie *p, int avoid, Case *goal) {
    //initialisation des outils de path finding

    //cas ou le fantome est sur la meme case que le fantome
    if (meme_case(start, goal)) {
        return -1;
    }

    //remplir les deux tableaux de noeuds vide
    for (int i = 0; i < array_max; ++i) {
        open_node[i] = (Node) {1};
        closed_node[i] = (Node) {1};
    }

    //definition du premier noeud
    Node *start_node = &open_node[0];
    start_node->node_case = start;
    start_node->empty = 0;

    start_node->h_cost = manhattan_distance(start_node->node_case, goal, p);
    start_node->g_cost = 0;
    start_node->f_cost = start_node->h_cost;
    start_node->parent = NULL;

    //reinitialisation des deux indexes
    closed_node_index = 0;
    open_node_index = 0;


    pathcode path = path_finder(p, avoid, goal);

    //cas ou le chemin n'a pas ete trouve
    Node cur = closed_node[closed_node_index];
    if (path == PATH_NOT_FOUND) {
        return -1;
    }
    int index, max_f_cost;

    switch (path) {
        case PATH_NOT_FOUND:
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
            //recherche la case voisine de la case de depart (n a pas de parent)
            while (cur.parent != NULL && !meme_case(cur.parent->node_case, start)) {
                cur = *cur.parent;
            }
    }

    //retourne la direction de la case de depart a la premiere case
    return dir_from_to(start, cur.node_case, p);
}

direction path_panic(Case *c, Partie *partie, int original_dir) {

    static direction dir_list[4] = {-1, -1, -1, -1};

    int dir_i = 0;

    for (direction i = 0; i < 4; ++i) {
        if (i != get_oppos(original_dir) && !get_case_at(partie, c, i)->wall) {
            dir_list[dir_i] = i;
            dir_i++;
        }
    }

    int random = entier_aleatoire(dir_i);

    return dir_list[random];
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

pathcode path_finder(Partie *p, int avoid, Case *goal) {
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
            if (manhattan_distance(n->node_case, goal, p) >= FLEE_MAX_PATH_DISTANCE) {
                return PATH_DISTANCE_REACHED;
            } else if (iter > FLEE_MAX_PATH_ITER) {
                return PATH_MAX_ITER_REACHED;
            }
        } else {
            if (manhattan_distance(n->node_case, goal, p) == 0) {
                return PATH_FOUND;
            }
        }

        check_neighbours(n, p, avoid, goal);
        iter++;
    }
}


void check_neighbours(Node *parent, Partie *p, int avoid, Case *goal) {

    for (direction dir = DIR_HAUT; dir < DIR_GAUCHE + 1; ++dir) {
        Case *target = get_case_at(p, parent->node_case, dir);
        if (target == NULL || target->wall ||
            closed_node_in_case(target) != -1) {
            continue;
        }

        if (open_node_in_case(target) == -1 || get_f_cost(open_node_in_case(target), parent, p, goal) <
                                               open_node[open_node_in_case(target)].f_cost) {
            if (open_node_in_case(target) == -1) {
                Node *neighbour = &open_node[++open_node_index];
                neighbour->empty = 0;
                neighbour->node_case = target;
                neighbour->parent = parent;
                set_cost(neighbour, parent, p, goal);
            } else {
                Node *neighbour = &open_node[open_node_in_case(target)];
                set_cost(neighbour, parent, p, goal);
                neighbour->parent = parent;

            }
        }

    }
}

int get_f_cost(int nodeI, Node *parent, Partie *partie, Case *goal) {
    //retourne le scoe f (somme des scores g et h)
    Node *node = &open_node[nodeI];
    return get_g_cost(parent, partie, node) + manhattan_distance(node->node_case, goal, partie);
}

int get_cheapest_node() {
    //recherche du voisin ayant le score F le plus petit
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
    //retourne si le noeud de la case est ferme
    for (int i = 0; i < closed_node_index + 1; ++i) {
        if (!closed_node[i].empty && (closed_node[i].node_case->x == c->x && closed_node[i].node_case->y == c->y)) {
            return i;
        }
    }
    return -1;
}

int open_node_in_case(Case *c) {
    //retourne si le noeud de la case est ouvert
    for (int i = 0; i < open_node_index + 1; ++i) {
        if (!open_node[i].empty && (open_node[i].node_case->x == c->x && open_node[i].node_case->y == c->y)) {
            return i;
        }
    }
    return -1;
}

int get_g_cost(Node *parent, Partie *partie, Node *neightbour) {
    //retourne le cout G d un noeud
    return parent->g_cost + 1;
}

void set_cost(Node *node, Node *parent, Partie *p, Case *goal) {
    //attribut les couts H, G et F d un noeud
    node->g_cost = get_g_cost(parent, p, node);

    node->h_cost = manhattan_distance(node->node_case, goal, p);
    //node->h_cost *= -1;

    node->f_cost = node->h_cost + node->g_cost;
}

int min(int a, int b) {
    return a < b ? a : b;
}


// https://stackoverflow.com/questions/23580083/manhattan-distance-for-a-2d-toroid

int manhattan_distance(const Case *aCase, const Case *b, const Partie *p) {
    //calcule la distance manhattane tout en considerand le plan comme un torroid
    int walk_mhdx = abs(aCase->x - b->x);
    int walk_mhdy = abs(aCase->y - b->y);

    int wrap_mhdx = p->xmax - walk_mhdx;
    int wrap_mhdy = p->ymax - walk_mhdy;


    int dx = min(walk_mhdx, wrap_mhdx);
    int dy = min(walk_mhdy, wrap_mhdy);

    return dx + dy;
}

int manhattan_distance_no_rollover(const Case *aCase, const Case *b, const Partie *p) {
    int walk_mhdx = abs(aCase->x - b->x);
    int walk_mhdy = abs(aCase->y - b->y);
    return walk_mhdx + walk_mhdy;
}
