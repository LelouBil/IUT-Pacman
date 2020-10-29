#include "utils.h"


Point get_point(const Case *c) {
    return (Point) {c->x * PLATEAU_BLOCK_TAILLE, c->y * PLATEAU_BLOCK_TAILLE};
}

Case *get_case_at(const Partie *partie, const Case *c, int direction) {
    switch (direction) {
        case DIR_GAUCHE:
            if (c->x - 1 < 0) return NULL;
            return &partie->plateau[c->x - 1][c->y];
        case DIR_DROITE:
            if (c->x + 1 > partie->xmax) return NULL;
            return &partie->plateau[c->x + 1][c->y];
        case DIR_HAUT:
            if (c->y - 1 < 0) return NULL;
            return &partie->plateau[c->x][c->y - 1];
        case DIR_BAS:
            if (c->y + 1 > partie->ymax) return NULL;
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