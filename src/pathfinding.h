#pragma once

#include "partie.h"


#define array_max 100


#define FLEE_MAX_PATH_ITER 150

#define FLEE_MAX_PATH_DISTANCE 20

direction path_init(Case *start, Partie *p, int avoid, Case *goal);

int manhattan_distance(const Case *aCase, const Case *b, const Partie *p);

direction path_panic(Case *start, Partie *p, int original_dir);

int get_case_color(const Case *c);

int manhattan_distance_no_rollover(const Case *aCase, const Case *b, const Partie *p);