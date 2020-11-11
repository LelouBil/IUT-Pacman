#include "./display.h"

#include "utils.h"

//region Fonctions privées

void remplir_case(const Case *c, int color) {
    dessiner_rectangle(get_point(c), PLATEAU_BLOCK_TAILLE, PLATEAU_BLOCK_TAILLE, color);
}

void dessiner_fantome(const Fantome *f) {
    dessiner_disque(to_point(f->position), SIZE_FANTOME, COLOR_FANTOMES[f->type]);
}

void dessiner_pacman(const Pacman *p) {
    //Dessine pacman selon sa position
    dessiner_disque(to_point(p->position), SIZE_PACMAN, yellow);
}

void dessiner_gomme_pac(const Case *c) {
    dessiner_disque(to_point(get_case_center(c)), SIZE_GOMME_PAC, COLOR_GOMME);
}

void dessiner_gomme_bonus(const Case *c) {
    dessiner_disque(to_point(get_case_center(c)), SIZE_GOMME_BONUS, COLOR_GOMME);
}

//endregion

void dessiner_plateau(const Partie *p) {

    dessiner_rectangle((Point) {0, 0}, p->xmax * PLATEAU_BLOCK_TAILLE, p->ymax * PLATEAU_BLOCK_TAILLE, black);

    //Parcoure tout le tableau du plateau et dessine un rectangle pour chaque character '*'

    for (int x = 0; x < p->xmax; x++) {
        for (int y = 0; y < p->ymax; y++) {
            dessiner_case(&p->plateau[x][y]);
        }
    }


}


void dessiner_case(const Case *c) {
    if (c->wall) {
        remplir_case(c, COLOR_WALL);
    } else {

        // On vide la case
        remplir_case(c, COLOR_BG);


        switch (c->gomme) {
            case GOMME_PAC:
                dessiner_gomme_pac(c);
                break;
            case GOMME_BONUS:
                dessiner_gomme_bonus(c);
                break;
        }
    }
}

void dessiner_rect_cases(const Partie *p, int cx, int cy) {
    for (int x = cx - 1; x < cx + 2; ++x) {
        for (int y = cy - 1; y < cy + 2; ++y) {
            if (x >= 0 && x < p->xmax && y >= 0 && y < p->ymax) {
                dessiner_case(&p->plateau[x][y]);
            }
        }
    }
}

void dessiner_entities(const Partie *partie){

    dessiner_rect_cases(partie,partie->pacman.case_pacman->x,partie->pacman.case_pacman->y);
    for (int i = 0; i < NBFANTOMES; ++i) {
        const Fantome *fantome = &partie->fantomes[i];
        dessiner_rect_cases(partie,fantome->case_fantome->x,fantome->case_fantome->y);
    }


    dessiner_pacman(&partie->pacman);


    for (int i = 0; i < NBFANTOMES; ++i) {
        const Fantome *fantome = &partie->fantomes[i];

        dessiner_fantome(fantome);
    }
}
