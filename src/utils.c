#include "display.h"



Point get_point(const Case *c) {
    return (Point) {c->x * PLATEAU_BLOCK_TAILLE, c->y * PLATEAU_BLOCK_TAILLE};
}

Case *get_case_at(const Partie *partie, const Case *c, direction direction) {
    switch (direction) {
        case DIR_GAUCHE:
            if (c->x - 1 < 0) return &partie->plateau[partie->xmax - 1][c->y];
            return &partie->plateau[c->x - 1][c->y];
        case DIR_DROITE:
            if (c->x + 1 >= partie->xmax) return &partie->plateau[0][c->y];
            return &partie->plateau[c->x + 1][c->y];
        case DIR_HAUT:
            if (c->y - 1 < 0) return &partie->plateau[c->x][partie->ymax - 1];
            return &partie->plateau[c->x][c->y - 1];
        case DIR_BAS:
            if (c->y + 1 >= partie->ymax) return &partie->plateau[c->x][0];
            return &partie->plateau[c->x][c->y + 1];
        default:
            return NULL;
    }
}


Pos get_case_center(const Case *c) {
    return (Pos) {c->x * PLATEAU_BLOCK_TAILLE + PLATEAU_BLOCK_TAILLE / 2,
                  c->y * PLATEAU_BLOCK_TAILLE + PLATEAU_BLOCK_TAILLE / 2};
}

Point to_point(Pos p) {
    return (Point) {p.x, p.y};
}

Pos dir_to_vector(int key) {
    switch (key) {
        case DIR_HAUT:
            return (Pos) {0, -1};
        case DIR_BAS:
            return (Pos) {0, 1};
        case DIR_DROITE:
            return (Pos) {1, 0};
        case DIR_GAUCHE:
            return (Pos) {-1, 0};
        default:
            return (Pos) {0, 0};
    }
}

int meme_case(Case *a, Case *b) {
    return (a->y == b->y && a->x == b->x);
}

int dir_from_to(Case *a, Case *b, Partie *p) {
    int x = a->x - b->x;
    int y = a->y - b->y;

    if (x == 0 && y == 0) return -1;
    if (x == 1 && y == 0) return DIR_GAUCHE;
    if (x == -1 && y == 0) return DIR_DROITE;
    if (x == 0 && y == 1) return DIR_HAUT;
    if (x == 0 && y == -1) return DIR_BAS;

    if (p == NULL) return -2;
    //tunnels
    if (a->x == 0 && b->x == p->xmax - 1) return DIR_GAUCHE;
    if (a->x == p->xmax - 1 && b->x == 0) return DIR_DROITE;

    if (a->y == 0 && b->y == p->ymax - 1) return DIR_BAS;
    if (a->y == p->ymax - 1 && b->x == 0) return DIR_HAUT;

    return -2; //ne devrais jamais arriver
}