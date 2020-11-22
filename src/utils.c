#include "timings.h"
#include "./gameplay.h"
#include "utils.h"
#include "collisions.h"
#include "display.h"


direction get_oppos(direction direction);

Point get_point(const Case *c) {
    return (Point) {c->x * PLATEAU_BLOCK_TAILLE, c->y * PLATEAU_BLOCK_TAILLE};
}

Case *get_case_at(const Partie *partie, const Case *c, direction direction) {
    //retourne le contenu d'une case a partir d une autre case et d une direction
    switch (direction) {
        case DIR_GAUCHE:
            if (c->x - 1 < 0) return &partie->plateau[partie->xmax - 1][c->y]; //cas ou dehors du plateau
            return &partie->plateau[c->x - 1][c->y]; //sinon
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
    //retourne une pos de coordonees centre de la case
    return (Pos) {c->x * PLATEAU_BLOCK_TAILLE + PLATEAU_BLOCK_TAILLE / 2,
                  c->y * PLATEAU_BLOCK_TAILLE + PLATEAU_BLOCK_TAILLE / 2};
}

Point to_point(Pos p) {
    //retourne un point à partir des coordonnees d un pos
    return (Point) {p.x, p.y};
}

Pos dir_to_vector(int key) {
    //retourn un pos vecteur d une direction
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
    //verifie si deux case ont la memme position
    if (a == NULL || b == NULL) return 0;
    return (a->y == b->y && a->x == b->x);
}

direction dir_from_to(Case *a, Case *b, Partie *p) {
    //retourne une direction entre deux cases

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

direction get_oppos(direction direction) {
    //retourne oppose d une direction
    switch (direction) {
        case DIR_HAUT:
            return DIR_BAS;
        case DIR_DROITE:
            return DIR_GAUCHE;
        case DIR_BAS:
            return DIR_HAUT;
        case DIR_GAUCHE:
            return DIR_DROITE;
    }
}

int dir_from_key(int key) {
    switch (key) {
        case SDLK_UP:
            return DIR_HAUT;
        case SDLK_DOWN:
            return DIR_BAS;
        case SDLK_RIGHT:
            return DIR_DROITE;
        case SDLK_LEFT:
            return DIR_GAUCHE;
        default:
            return -1;
    }
}

int get_perpendicular(direction dir) {
    switch (dir) {
        case DIR_HAUT:
        case DIR_BAS:
            return DIR_DROITE;

        case DIR_DROITE:
        case DIR_GAUCHE:
            return DIR_HAUT;
        default:
            return 0;
    }
}