#pragma once


//region Defines

//region plateau
#define PLATEAU_BLOCK_TAILLE 32
//endregion

//region Fantome Types
typedef enum {
    FANTOME_BLINKY, FANTOME_PINKY, FANTOME_INKY, FANTOME_CLYDE
} type_fantome;
//endregion

//region Gomme types

typedef enum {
    GOMME_EMPTY,
    GOMME_PAC,
    GOMME_BONUS
} type_gomme;
//endregion

typedef enum {
    DIR_HAUT,
    DIR_DROITE,
    DIR_BAS,
    DIR_GAUCHE
} direction;

//region jeu
#define NBFANTOMES 4 // nombres de fantômes dans les plateaux chargés
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
    type_gomme gomme; // 0 rien, 1 pac gomme, 2 bonus
    //int pas;
} Case;

// pacman
typedef struct {
    Case *case_pacman;
    direction direction;
    Pos position; //pos EN PIXEL
    int oob;
} Pacman;
//endregion

typedef struct {
    Case *case_fantome;
    direction direction;
    Pos position;
    type_fantome type;
    int oob;
} Fantome;

// Une partie
typedef struct {
    Case **plateau;
    int xmax;
    int ymax;
    Pacman pacman;
    Fantome fantomes[NBFANTOMES];
    int gomme_restant;
} Partie;
//endregion



//region PROTOTYPES
// charge_plan : lit un fichier contenant un plateau de jeu et le charge en mémoire, dans le champ 'plateau' d'une Partie
Partie charge_plan(char *fichier);

// convert_partie : convertit Char_Partie Partie
Partie convert_partie(Char_Partie charPartie);
//endregion

