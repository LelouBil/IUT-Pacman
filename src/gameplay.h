#pragma once


#include "partie.h"

#define PACMAN_SPEED 1.0 // 1 case par seconde


static const int VITESSE_FANTOME[NBFANTOMES] = {4};


void pacman_deplace(Partie *partie);