#include "./ai.h"

#include "gameplay.h"
#include "utils.h"
#include "pathfinding.h"
#include "timings.h"

int DIR_AVAILABLE[4];

int fantome_aligned_case(Fantome *fantome);

int fantome_update_direction(Partie *partie, Fantome *f);

void fantome_move(Fantome *f);

int dir_to_pacman(Fantome *f);

void blinky_behaviour(Fantome *blinky, Partie *partie);

void (*fantome_behaviour[NBFANTOMES])(Fantome *fantome, Partie *partie) = {blinky_behaviour, blinky_behaviour,
                                                                           blinky_behaviour, blinky_behaviour};

int base_timer[NBFANTOMES] = {0, 1000, 2000, 3000};
int start_timer[NBFANTOMES];

void reset_timers() {
    for (int i = 0; i < NBFANTOMES; ++i) {
        start_timer[i] = base_timer[i];
    }
}

void fantome_event_manager(Partie *partie) {

    for (int i = 0; i < NBFANTOMES; i++) {
        Fantome *f = &partie->fantomes[i];
        if (start_timer[i] <= 0) {
            if (!f->oob)fantome_behaviour[i](f, partie);
            fantome_move(f);
        } else {
            //printf("%d : %d\n",i,start_timer[i]);
            start_timer[i] -= framerate;
        }

        Pos p = f->position;

        int cx = p.x / PLATEAU_BLOCK_TAILLE;
        int cy = p.y / PLATEAU_BLOCK_TAILLE;


        if (cx >= 0 && cx < partie->xmax && cy >= 0 && cy < partie->ymax) {
            f->oob = 0;
        } else {
            f->oob = 1;
        }


        int tunnel = 0;

        if (cx > partie->xmax) {
            cx = -1;
            tunnel = 1;
        } else if (cx < 0) {
            cx = partie->xmax;
            tunnel = 1;
        }

        if (cy > partie->ymax) {
            cy = -1;
            tunnel = 1;
        } else if (cy < 0) {
            cy = partie->ymax;
            tunnel = 1;
        }


        if (tunnel) {

            f->position = (Pos) {cx * PLATEAU_BLOCK_TAILLE + PLATEAU_BLOCK_TAILLE / 2,
                                 cy * PLATEAU_BLOCK_TAILLE + PLATEAU_BLOCK_TAILLE / 2};
        }

        if (!f->oob) {
            f->case_fantome = &partie->plateau[cx][cy];
        }
    }
}

void blinky_behaviour(Fantome *blinky, Partie *partie){
    if (fantome_aligned_case(blinky)) {
        if(partie->bonus_timer == 0) {
            blinky->direction = path_init(blinky->case_fantome, partie, 0 /*obsolete*/ );
        }
        else{
            blinky->direction = path_panic(blinky->case_fantome, partie, blinky->direction);
        }

    }
}

void fantome_move(Fantome* f){
    f->position.x += 1 * dir_to_vector(f->direction).x;
    f->position.y += 1* dir_to_vector(f->direction).y;
}

int fantome_aligned_case(Fantome *fantome){
    Pos p = get_case_center(fantome->case_fantome);
    return fantome->position.x == p.x && fantome->position.y == p.y;
}


