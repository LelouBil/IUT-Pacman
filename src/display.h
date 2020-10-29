#pragma once

#include "../lib/libgraphique.h"
#include "./partie.h"

//region colors
#define COLOR_WALL blue
#define COLOR_BG black
#define COLOR_PACMAN yellow
#define COLOR_GOMME yellow
#define COLOR_BONUS yellow
static const int COLOR_FANTOMES[NBFANTOMES] = {pink};

//region Prototypes

void dessiner_plateau(const Partie *p);

void dessiner_case(const Case *p);

void dessiner_pacman(const Pacman *p);

void dessiner_fantome(const Fantome *f);

void dessiner_rect_cases(const Partie *p, int cx, int cy);
//endregion

