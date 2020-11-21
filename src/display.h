#pragma once

#include "../lib/libgraphique.h"
#include "partie.h"

//region colors
#define COLOR_WALL blue
#define COLOR_BG black
#define COLOR_PACMAN yellow
#define COLOR_GOMME yellow
#define COLOR_BONUS yellow

#define COLOR_SCORE_BG wheat


static const int COLOR_FANTOMES[NBFANTOMES] = {red, pink, cyan, orange};

//region Sizes
#define SIZE_GOMME_BONUS ((PLATEAU_BLOCK_TAILLE / 2.0) * 0.5)
#define SIZE_GOMME_PAC ((PLATEAU_BLOCK_TAILLE / 2.0) * 0.2)
#define SIZE_FANTOME ((PLATEAU_BLOCK_TAILLE / 2.0) * 0.8)
#define SIZE_PACMAN ((PLATEAU_BLOCK_TAILLE / 2.0) * 0.8)




//region Prototypes

void dessiner_plateau(const Partie *p);

void dessiner_case(const Case *p);


void dessiner_rect_cases(const Partie *p, int cx, int cy);

void dessiner_debug_paththfinding_case(Pos *c, int current_step, int max_step);

void remplir_case(const Case *c, int color);

void dessiner_entities(Partie *partie);

void dessiner_score(Partie *partie);

void dessiner_texte_niveau(Partie *partie);

void dessiner_rectangle_score(Partie *partie);

void dessiner_all_scores(char pString[MAX_SCORES][3], int score[MAX_SCORES], int nbscores);

void dessiner_vies(int vies, Couleur color);


void dessiner_texte_center(char *text, int pt, int bgcolor, int fgcolor);
//endregion

