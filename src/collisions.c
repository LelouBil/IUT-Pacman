
#include "collisions.h"
#include "utils.h"

void collision_pac_gomme(Partie *partie);

int collisions_pac_fantome(Partie *partie);

int check_collisions(Partie *partie) {
    collision_pac_gomme(partie);
    collision_pac_gomme(partie);
    return collisions_pac_fantome(partie);
}

int collisions_pac_fantome(Partie *partie) {
    for (int i = 0; i < NBFANTOMES; ++i) {
        if (partie->fantomes[i].alive && meme_case(partie->pacman.case_pacman, partie->fantomes[i].case_fantome)) {
            if (partie->bonus_timer > 0) {
                partie->points += 100;
                partie->fantomes[i].position = get_case_center(partie->fantomes[i].case_fantome);
                partie->fantomes[i].alive = 0;
                return 0;
            }
            return 1;
        }
    }
    return 0;
}

void collision_pac_gomme(Partie *partie) {
    Pacman p = partie->pacman;
    Pos center = get_case_center(p.case_pacman);
    if (center.x != p.position.x || center.y != p.position.y) return;
    switch (p.case_pacman->gomme) {
        case GOMME_PAC:
            partie->points += 1;
            partie->gomme_restant--;
            p.case_pacman->gomme = GOMME_EMPTY;
            break;
        case GOMME_BONUS:
            partie->points += 50;
            partie->bonus_timer = BONUS_MAX_TIME;
            p.case_pacman->gomme = GOMME_EMPTY;
            break;
        case GOMME_EMPTY:
            break;
    }
}

