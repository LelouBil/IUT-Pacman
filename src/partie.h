#pragma once


//region Defines

//region plateau
#define PLATEAU_BLOCK_TAILLE 32

#define MAX_SCORES 30
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
#define BONUS_MAX_TIME 5000 //temps max du mode super pac gomme
#define VIE_MAX 3
//endregion

//region STRUCTURES
// Structure Pos: permet de stocker un couple ligne/colonne
typedef struct {
    int x;
    int y;
} Pos;


// Une case
typedef struct {
    int x, y;
    int wall;
    type_gomme gomme; // 0 rien, 1 pac gomme, 2 bonus
    //int pas;
    int porte;
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
    int speed;
    int alive;
    int sorti;
} Fantome;

// Une partie
typedef struct {
    Case **plateau;
    int xmax;
    int ymax;
    Pacman pacman;
    int gomme_restant;
    int level;
    Fantome fantomes[NBFANTOMES];
    int max_gommes;
    int bonus_timer;
    Case *spawn_fantome[NBFANTOMES];
    int points;
    int vies;
} Partie;
//endregion



//region PROTOTYPES
// charge_plan : lit un fichier contenant un plateau de jeu et le charge en mémoire, dans le champ 'plateau' d'une Partie
Partie load_partie_template(char *fichier);


Partie clone_partie(Partie *p);
//endregion

