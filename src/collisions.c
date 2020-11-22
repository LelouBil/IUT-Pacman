#include "collisions.h"
#include "utils.h"

void collision_pac_gomme(Partie *partie);

int collisions_pac_fantome(Partie *partie);

int check_collisions(Partie *partie) {
    //appelle a chaque instant pour verifier les collisions

    collision_pac_gomme(partie);
    collision_pac_gomme(partie);
    return collisions_pac_fantome(partie);
}

int collisions_pac_fantome(Partie *partie) {
    //verifie toutes les collisions avec pacman et fait les opperations appropries selon que l on soit en mode bonnus ou non
    //retourne 0 si non collision ou collision en mode bonus -> pacman reste en vie
    //retourne 1 si collision en mode normal -> pac man meurt
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
    Pos center = get_case_center(p.case_pacman); //recupere la position de centre de case de pacman
    if (center.x != p.position.x || center.y != p.position.y) return; //si pac man n'est pas au centre, sortir
    switch (p.case_pacman->gomme) { //switch type de pac gomme
        case GOMME_PAC: //pac gomme
            partie->points += 1;
            partie->gomme_restant--;
            p.case_pacman->gomme = GOMME_EMPTY; //vide la case
            break;
        case GOMME_BONUS: //super pac gomme
            partie->points += 50;
            partie->bonus_timer = BONUS_MAX_TIME; //activation du timer bonus
            p.case_pacman->gomme = GOMME_EMPTY;
            break;
        case GOMME_EMPTY: //vide
            break;
        default:
            break;
    }
}
