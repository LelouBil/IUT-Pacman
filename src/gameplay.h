#pragma once

#include "partie.h"
#include "utils.h"
#include "timings.h"
#include "display.h"

#define PACMAN_SPEED 1.0 // 1 case par seconde

#define DIR_HAUT 0
#define DIR_DROITE 1
#define DIR_BAS 2
#define DIR_GAUCHE 3

static const int VITESSE_FANTOME[NBFANTOMES] = {4};


void key_events(Partie *partie);