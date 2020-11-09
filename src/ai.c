#include "./ai.h"

#include "gameplay.h"
#include "utils.h"
#include "pathfinding.h"

int DIR_AVAILABLE[4];

int fantome_aligned_case(Fantome *fantome);
int fantome_update_direction(Partie *partie, Fantome* f);
void fantome_move(Fantome* f);
int dir_to_pacman(Fantome* f);
void blinky_behaviour(Fantome *blinky, Partie *partie);

void (*fantome_behaviour[NBFANTOMES])(Fantome *fantome,Partie *partie) = {blinky_behaviour,blinky_behaviour,blinky_behaviour,blinky_behaviour};

int start_timer[NBFANTOMES] = {0,1000,2000,3000};

void fantome_event_manager(Partie *partie){

    for(int i = 0;i < NBFANTOMES;i++) {
        Fantome *f = &partie->fantomes[i];
        if (start_timer[i] <= 0){
            fantome_behaviour[i](f,partie);
        } else{
            //printf("%d : %d\n",i,start_timer[i]);
            start_timer[i]-= framerate;
        }

    }
}

void blinky_behaviour(Fantome *blinky, Partie *partie){
    if (fantome_aligned_case(blinky)) {

        blinky->direction = path_init(blinky->case_fantome, partie);
    }
    fantome_move(blinky);

    Pos p = blinky->position;

    int cx = p.x / PLATEAU_BLOCK_TAILLE;
    int cy = p.y / PLATEAU_BLOCK_TAILLE;


    blinky->case_fantome = &partie->plateau[cx][cy];
}

void fantome_move(Fantome* f){
    f->position.x += 1 * dir_to_vector(f->direction).x;
    f->position.y += 1* dir_to_vector(f->direction).y;
}

int fantome_aligned_case(Fantome *fantome){
    Pos p = get_case_center(fantome->case_fantome);
    return fantome->position.x == p.x && fantome->position.y == p.y;
}


