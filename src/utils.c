#include "utils.h"


Point get_point(const Case *c) {
    return (Point) {c->x * PLATEAU_BLOCK_TAILLE, c->y * PLATEAU_BLOCK_TAILLE};
}