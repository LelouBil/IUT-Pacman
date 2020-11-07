#include "./ai.h"

#include "gameplay.h"
#include "utils.h"
#include "../lib/libgraphique.h"
#include "math.h"


int DIR_AVAILABLE[4];

int fantome_aligned_case(Fantome *fantome);
int fantome_update_direction(Partie *partie, Fantome* f);
void fantome_move(Fantome* f,int index);
int dir_to_pacman(Fantome* f);

void fantome_event_manager(Partie *partie){

    for(int i = 0;i < NBFANTOMES;i++) {
        Fantome f = partie->fantomes[i];
        if (fantome_aligned_case(&f)) {

            //f.direction = fantome_update_direction(partie,&f);
        }
        fantome_move(&f, i);

        Pos p = f.position;

        int cx = p.x / PLATEAU_BLOCK_TAILLE;
        int cy = p.y / PLATEAU_BLOCK_TAILLE;

        f.case_fantome->x = cx;
        f.case_fantome->y = cy;

        dessiner_rect_cases(partie, cx, cy);
    }
}

void fantome_move(Fantome* f,int index){
    f->position.x += 4 * dir_to_vector(f->direction).x;
    f->position.y += 4 * dir_to_vector(f->direction).y;
}

int fantome_aligned_case(Fantome *fantome){
    Pos p = get_case_center(fantome->case_fantome);
    switch (fantome->direction) {
        case DIR_HAUT:
        case DIR_BAS:
            return fantome->position.x == p.x;
        case DIR_GAUCHE:
        case DIR_DROITE:
            return fantome->position.y == p.y;
        default:
            return 0;
    }
}


