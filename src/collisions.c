
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
    return 0;
}

void collision_pac_gomme(Partie *partie) {
    Pacman p = partie->pacman;
    Pos center = get_case_center(p.case_pacman);
    if (center.x != p.position.x || center.y != p.position.y) return;
    switch (p.case_pacman->gomme) {
        case GOMME_PAC:
            partie->gomme_restant--;
            p.case_pacman->gomme = GOMME_EMPTY;
            break;
        case GOMME_BONUS:
            //todo p.bonus_timer = BONUS_MAX_TIME;
            p.case_pacman->gomme = GOMME_EMPTY;
            break;
    }
}
