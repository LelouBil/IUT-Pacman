#pragma once

#include "../lib/libgraphique.h"
#include "partie.h"
#include "display.h"
#include "gameplay.h"

//region Libgraphique util
Point get_point(const Case *c);

Case *get_case_at(const Partie *partie, const Case *c, int direction);

Pos get_case_center(const Case *c);

Point to_point(Pos p);

Pos dir_to_vector(int key);

int meme_case(Case* a, Case* b);
//endregion