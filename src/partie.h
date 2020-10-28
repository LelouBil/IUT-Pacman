#pragma once


#include <stdio.h>
#include <stdlib.h>

//region Defines

//region plateau
#define PLATEAU_BLOCK_TAILLE 32
//endregion

//region Entities Types
#define ENTITY_EMPTY 0
#define ENTITY_PACMAN 1
#define ENTITY_BLINKY 2
#define ENTITY_PINKY 3
#define ENTITY_INKY 4
#define ENTITY_CLYDE 5
//endregion

//region Gomme types
#define GOMME_EMPTY 0
#define GOMME_PAC 1
#define GOMME_BONUS 2
//endregion

//region jeu
#define NBFANTOMES  1 // nombres de fantômes dans les plateaux chargés
#define VIE_MAX 3
//endregion

//region STRUCTURES
// Structure Pos: permet de stocker un couple ligne/colonne
typedef struct {
    int x;
    int y;
} Pos;

// Structure Char_Partie:  permet de stocker les paramètres d'une partie en char
typedef struct {
    char **plateau; // le tableau de caractères contenant le plateau de jeu

    int L; // le nb de lignes du plateau de jeu
    int C; // le nb de colonne du plateau de jeu

    Pos pacman; // la position de Pacman dans le plateau

    Pos fantomes[NBFANTOMES]; // les positions de chaque fantôme

    int nbbonus; // le nombre de bonus restants à manger

} Char_Partie;


// Une case
typedef struct {
    int x, y;
    int wall;
    int type_gomme; // 0 rien, 1 pac gomme, 2 bonus
    int type_entity; // 0 rien, 1 pacman, 2 3 4 5 fantomes
} Case;

// pacman
typedef struct {
	Case *case_pacman;
	Pos position; //pos EN PIXEL
} Pacman;
//endregion

// Une partie
typedef struct {
    Case **plateau;
    int xmax;
    int ymax;
    Pacman pacman;
    Case *cases_fantomes[NBFANTOMES];

    int gomme_restant;
} Partie;
//endregion

//region PROTOTYPES
// charge_plan : lit un fichier contenant un plateau de jeu et le charge en mémoire, dans le champ 'plateau' d'une Partie
Partie charge_plan(char *fichier);

// convert_partie : convertit Char_Partie Partie
Partie convert_partie(Char_Partie charPartie);
//endregion

