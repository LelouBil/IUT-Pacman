#include "./ai.h"

#include "gameplay.h"
#include "utils.h"
#include "pathfinding.h"
#include "timings.h"

#define REGEN_TIME 3000

int DIR_AVAILABLE[4];

int fantome_aligned_case(Fantome *fantome);

int fantome_update_direction(Partie *partie, Fantome *f);

void fantome_move(Fantome *f);

int dir_to_pacman(Fantome *f);

void blinky_behaviour(Fantome *blinky, Partie *partie);

void inky_behaviour(Fantome *blinky, Partie *partie);

void pinky_behaviour(Fantome *blinky, Partie *partie);

void clyde_behaviour(Fantome *blinky, Partie *partie);

void (*fantome_behaviour[NBFANTOMES])(Fantome *fantome, Partie *partie) = {blinky_behaviour, pinky_behaviour,
                                                                           inky_behaviour, clyde_behaviour};

int base_timer[NBFANTOMES] = {0, 1000, 1000, 2000};

int regen_timer[NBFANTOMES] = {REGEN_TIME, REGEN_TIME, REGEN_TIME, REGEN_TIME};


int start_timer[NBFANTOMES];

void reset_timers() {
    for (int i = 0; i < NBFANTOMES; ++i) {
        start_timer[i] = base_timer[i];
        regen_timer[i] = REGEN_TIME;
    }
}

void fantome_event_manager(Partie *partie) {

    for (int i = 0; i < NBFANTOMES; i++) {
        Fantome *f = &partie->fantomes[i];
        if (start_timer[i] <= 0) {
            if (!f->oob) {
                if (!f->alive) {
                    if (meme_case(f->case_fantome, partie->spawn_fantome[i]) && fantome_aligned_case(f)) {
                        f->speed = 1;
                        f->sorti = 0;
                        if (regen_timer[i] <= 0) {
                            f->alive = 1;
                            regen_timer[i] = REGEN_TIME;
                        } else if (regen_timer[i] <= REGEN_TIME) {
                            regen_timer[i] -= framerate;
                        }
                        continue;
                    } else {
                        f->speed = 2;
                        if (fantome_aligned_case(f))
                            f->direction = path_init(f->case_fantome, partie, 0 /*obsolete*/ ,
                                                     partie->spawn_fantome[i], 0);
                    }
                } else {
                    if (partie->bonus_timer == BONUS_MAX_TIME) {
                        f->direction = get_oppos(f->direction);
                    } else {
                        if (f->case_fantome->porte) f->sorti = 1;
                        fantome_behaviour[i](f, partie);
                    }
                }
            }
            int good = 1;
            for (int j = 0; j < NBFANTOMES; ++j) {
                if (i != j && fantome_aligned_case(f)) {
                    if (meme_case(get_case_at(partie, f->case_fantome, f->direction),
                                  partie->fantomes[j].case_fantome)) {
                        if (partie->bonus_timer > 0)
                            good = entier_aleatoire(2);
                        else good = 0;
                    }
                    if (meme_case(f->case_fantome, partie->fantomes[j].case_fantome)) good = entier_aleatoire(2);
                }
            }
            if (good || !f->alive) {
                fantome_move(f);
            }
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

void blinky_behaviour(Fantome *blinky, Partie *partie) {
    if (fantome_aligned_case(blinky)) {
        if (partie->bonus_timer <= 0) {
            blinky->speed = 6 * (partie->level / 255) + 1;
            blinky->direction = path_init(blinky->case_fantome, partie, 0 /*obsolete*/, partie->pacman.case_pacman,
                                          blinky->sorti);
        } else {
            blinky->direction = path_panic(blinky->case_fantome, partie, blinky->direction);
        }

    }
}

void inky_behaviour(Fantome *blinky, Partie *partie) {
    if (fantome_aligned_case(blinky)) {
        if (partie->bonus_timer <= 0 && entier_aleatoire(10) != 0) {

            blinky->direction = path_init(blinky->case_fantome, partie, 0 /*obsolete*/, partie->pacman.case_pacman,
                                          blinky->sorti);
        } else {
            blinky->direction = path_panic(blinky->case_fantome, partie, blinky->direction);
        }

    }
}

void pinky_behaviour(Fantome *blinky, Partie *partie) {
    if (fantome_aligned_case(blinky)) {
        if (partie->bonus_timer <= 0) {
            Case *target = get_case_at(partie, partie->pacman.case_pacman, partie->pacman.direction);
            if (target == NULL || target->wall) {
                target = partie->pacman.case_pacman;
            }

            blinky->direction = path_init(blinky->case_fantome, partie, 0 /*obsolete*/, target,
                                          blinky->sorti);
        } else {
            blinky->direction = path_panic(blinky->case_fantome, partie, blinky->direction);
        }

    }
}

void clyde_behaviour(Fantome *blinky, Partie *partie) {
    if (fantome_aligned_case(blinky)) {
        if (partie->bonus_timer <= 0) {
            Case *target = partie->pacman.case_pacman;
            int distance = manhattan_distance_no_rollover(blinky->case_fantome, partie->pacman.case_pacman, partie);
            if (distance <= 8 || blinky->flag) {
                target = &partie->plateau[1][1];
                blinky->flag = 1;
                if (meme_case(blinky->case_fantome, target) == 1) {
                    blinky->flag = 0;
                }
            }
            blinky->direction = path_init(blinky->case_fantome, partie, 0 /*obsolete*/, target,
                                          blinky->sorti);
        } else {
            blinky->direction = path_panic(blinky->case_fantome, partie, blinky->direction);
        }

    }
}

void fantome_move(Fantome *f) {
    f->position.x += f->speed * dir_to_vector(f->direction).x;
    f->position.y += f->speed * dir_to_vector(f->direction).y;
}

int fantome_aligned_case(Fantome *fantome) {
    Pos p = get_case_center(fantome->case_fantome);
    return fantome->position.x == p.x && fantome->position.y == p.y;
}


