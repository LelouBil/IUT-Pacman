#include "./pathfinding.h"



int path_neighbour(Node* n, Partie* p);
int path_seaker(Node* n, Partie* p);
void debud_draw_pathfinder();


void debug_draw_pathfinder() {
    int max_step = 0;


    for (int i = 0; i < 21; ++i) {
        for (int j = 0; j < 27; ++j) {
            Node* n = node_matrix[i][j];
            max_step = (max_step < n->step) ? n->step : max_step;
        }
    }

    for (int i = 0; i < 21; ++i) {
        for (int j = 0; j < 27; ++j) {
            Node* n = node_matrix[i][j];
            dessiner_debug_paththfinding_case(n->case_node, n->step, max_step);
        }
    }
    actualiser();
    attendre_clic();
}


int path_init(Case* pos, Partie* p){
    Case case_null;
    Node node_null = {&case_null,PATHFINDER_INFINITE,-1,NULL,NULL };
    for (int i = 0; i < 21; ++i) {
        for (int j = 0; j < 27; ++j) {
            node_matrix[i][j] = &node_null;
        }
    }

    Node node_start = {pos,0,0,NULL,NULL };

    int dir = path_seaker(&node_start,p);

    if(PATHFINDER_DEBUG_LEVEL == 1){
        debug_draw_pathfinder();
    }

    return dir;
}

int path_seaker(Node* n, Partie* p){
    if(!meme_case(n->case_node, p->pacman.case_pacman)){
        return path_neighbour(n,p);
    }

    if(PATHFINDER_DEBUG_LEVEL == 2){
        debug_draw_pathfinder();
    }

    return 0;
}

int path_neighbour(Node* n,Partie* p){
    //verifie les cases voisine et assigne a la case comme attribut 'next_node' la case par lequel le chemin est le plus proche

    if(!get_case_at(p,(n->case_node),DIR_HAUT)->wall) {
        Node* up = n;
        up->case_node = &p->plateau[n->case_node->x][n->case_node->y - 1 % p->ymax];

        if((node_matrix[up->case_node->x][up->case_node->y])->step < n->step+1) {
            path_seaker(up, p);
            n->dir = DIR_HAUT;
            return DIR_HAUT;
        }
    }

    if(!get_case_at(p,(n->case_node),DIR_DROITE)->wall) {
        Node* right = n;
        right->case_node = &p->plateau[n->case_node->x  + 1 % p->xmax][n->case_node->y];

        if((node_matrix[right->case_node->x][right->case_node->y])->step < n->step+1) {
            path_seaker(right, p);
            n->dir = DIR_DROITE;
            return DIR_DROITE;
        }
    }

    if(!get_case_at(p,(n->case_node),DIR_BAS)->wall) {
        Node* down = n;
        down->case_node = &p->plateau[n->case_node->x][n->case_node->y + 1 % p->ymax];

        if((node_matrix[down->case_node->x][down->case_node->y])->step < n->step+1) {
            path_seaker(down, p);
            n->next_node = down;
            n->dir = DIR_BAS;
            return DIR_BAS;
        }
    }

    if(!get_case_at(p,(n->case_node),DIR_GAUCHE)->wall) {
        Node* left = n;
        left->case_node = &p->plateau[n->case_node->x - 1 % p->xmax][n->case_node->y ];

        if((node_matrix[left->case_node->x][left->case_node->y])->step < n->step+1) {
            path_seaker(left, p);
            n->next_node = left;
            n->dir = DIR_GAUCHE;
            return DIR_GAUCHE;
        }
    }

    return -1;
}