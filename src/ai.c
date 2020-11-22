#include "./ai.h"

#include "utils.h"
#include "pathfinding.h"
#include "timings.h"

#define REGEN_TIME 3000




int base_timer[NBFANTOMES] = {0, 1000, 1000, 2000};

int regen_timer[NBFANTOMES] = {REGEN_TIME, REGEN_TIME, REGEN_TIME, REGEN_TIME};

int start_timer[NBFANTOMES];

void reset_timers() {
    for (int i = 0; i < NBFANTOMES; ++i) {
        start_timer[i] = base_timer[i];
        regen_timer[i] = REGEN_TIME;
    }
}

int is_fantome_aligned_case(Fantome *fantome) {
    Pos p = get_case_center(fantome->case_fantome);
    return fantome->position.x == p.x && fantome->position.y == p.y;
}


void blinky_behaviour(Fantome *blinky, Partie *partie) {
    if (is_fantome_aligned_case(blinky)) {
        if (partie->bonus_timer <= 0) {
            blinky->speed = 6 * (partie->level / 255) + 1; // on augemente la vitesse petit a petit par niveau
            blinky->direction = path_init(blinky->case_fantome, partie, 0 /*obsolete*/, partie->pacman.case_pacman);
        } else {
            blinky->direction = path_panic(blinky->case_fantome, partie, blinky->direction);
        }

    }
}

void inky_behaviour(Fantome *blinky, Partie *partie) {
    if (is_fantome_aligned_case(blinky)) {
        if (partie->bonus_timer <= 0 && entier_aleatoire(10) !=
                                        0) { // chance aléatoire de prendre une direction au hasard, c'est a dire utiliser path_panic

            blinky->direction = path_init(blinky->case_fantome, partie, 0 /*obsolete*/, partie->pacman.case_pacman);
        } else {
            blinky->direction = path_panic(blinky->case_fantome, partie, blinky->direction);
        }

    }
}

void pinky_behaviour(Fantome *blinky, Partie *partie) {
    if (is_fantome_aligned_case(blinky)) {
        if (partie->bonus_timer <= 0) {
            Case *target = get_case_at(partie, partie->pacman.case_pacman,
                                       partie->pacman.direction); // vise case devant pacman
            if (target == NULL || target->wall) {
                target = partie->pacman.case_pacman; // si mur ou hors du plateau vise pacman
            }

            blinky->direction = path_init(blinky->case_fantome, partie, 0 /*obsolete*/, target);
        } else {
            blinky->direction = path_panic(blinky->case_fantome, partie, blinky->direction);
        }

    }
}

void clyde_behaviour(Fantome *blinky, Partie *partie) {
    if (is_fantome_aligned_case(blinky)) {
        if (partie->bonus_timer <= 0) {

            Case *target = partie->pacman.case_pacman; // vise pacman

            int distance = manhattan_distance_no_rollover(blinky->case_fantome, partie->pacman.case_pacman, partie);
            if (distance <= 8 ||
                blinky->flag) { // sauf si a moins de 8 de distance OU qu'on essaie deja d'aller en haut a gauche
                target = &partie->plateau[1][1]; // on vise en haut a gauche a la place
                blinky->flag = 1;
                if (meme_case(blinky->case_fantome, target) == 1) {
                    blinky->flag = 0; // on est arrivé donc on reset
                }
            }
            blinky->direction = path_init(blinky->case_fantome, partie, 0 /*obsolete*/, target);

        } else {
            blinky->direction = path_panic(blinky->case_fantome, partie, blinky->direction);
        }

    }
}


void fantome_move(Fantome *f) {
    f->position.x += f->speed * dir_to_vector(f->direction).x;
    f->position.y += f->speed * dir_to_vector(f->direction).y;
}

//tableau de pointeurs vers des fonctions, ca evite un switch
void (*fantome_behaviour[NBFANTOMES])(Fantome *fantome, Partie *partie) = {blinky_behaviour, pinky_behaviour,
                                                                           inky_behaviour, clyde_behaviour};

void fantome_event_manager(Partie *partie) {

    for (int i = 0; i < NBFANTOMES; i++) {
        Fantome *f = &partie->fantomes[i];
        if (start_timer[i] <= 0) { // si le fantome peut partir

            if (!f->oob) {

                if (!f->alive) { // si mort
                    if (meme_case(f->case_fantome, partie->spawn_fantome[i]) &&
                        is_fantome_aligned_case(f)) { // si mort et retourné a son point d'apparition
                        f->speed = 1;
                        if (regen_timer[i] <= 0) { // si fini de regenerer, il revient en vie
                            f->alive = 1;
                            regen_timer[i] = REGEN_TIME;
                        } else if (regen_timer[i] <= REGEN_TIME) { // sinon on continue de régénerer
                            regen_timer[i] -= framerate;
                        }
                        continue;
                    } else {
                        //si mort et pas au point d'apparition, on y va a vitesse 2
                        f->speed = 2;
                        if (is_fantome_aligned_case(f)) // changement de direction uniquement quand aligné sur une case
                            f->direction = path_init(f->case_fantome, partie, 0 /*obsolete*/ ,
                                                     partie->spawn_fantome[i]);
                    }

                } else { // si en vie
                    if (partie->bonus_timer == BONUS_MAX_TIME) {
                        f->direction = get_oppos(
                                f->direction); // si on vient de manger un bonus, les fantomes changent de direction instantanément. 
                    } else {
                        fantome_behaviour[i](f, partie); // sinon on fait l'ia du fantome normale
                    }
                }
            }
            // partie qui evite que les fantomes soient l'un sur l'autre
            int good = 1;

            for (int j = 0; j < NBFANTOMES; ++j) {
                if (i != j &&
                    is_fantome_aligned_case(f)) { // si on est aligné sur une case et qu'on regarde pas le meme fantome
                    if (meme_case(get_case_at(partie, f->case_fantome, f->direction),
                                  partie->fantomes[j].case_fantome)) {
                        // et qu'un autre fantome est sur la case ou on veut aller
                        if (partie->bonus_timer <= 0) // et pas en mode bonus
                            good = 0; // on attend
                        else good = entier_aleatoire(2); // sinon on a une chance sur 2 d'attendre
                        // car ils vont jamais bouger pendant le bonus si ils veulent aller dans des directions opposés
                        // ce qui ne peut pas arriver sans le bonus 
                    }

                    if (meme_case(f->case_fantome, partie->fantomes[j].case_fantome)) good = entier_aleatoire(2);
                    // si sur la meme case une chance sur 2 d'attendre, sinon ils vont tout les deux attendre et etre bloqués
                }
            }

            if (good || !f->alive) { // si on doit pas attendre, ou que le fantome est mort on bouge
                fantome_move(f);
            }
        } else {

            // on baisse le timer si ce fantome peut pas encore commencé
            start_timer[i] -= framerate;
        }

        // voir pacman_deplace dans gameplay.c, c'est la meme chose

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

