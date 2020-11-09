#pragma once

#include "partie.h"
#include "utils.h"
#include "display.h"


#define PATHFINDER_DEBUG_LEVEL 1
#define array_max 100



int path_init(Case *start, Partie* p);

int get_case_color(const Case *c);