#pragma once

#include "partie.h"


#define array_max 100


#define FLEE_MAX_PATH_ITER 150

#define FLEE_MAX_PATH_DISTANCE 20

int path_init(Case *start, Partie *p, int avoid);

int path_panic(Case *start, Partie *p, int original_dir);

int get_case_color(const Case *c);