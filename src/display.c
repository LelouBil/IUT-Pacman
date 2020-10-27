//
// Created by Lelou on 27/10/2020.
//

#include "display.h"

#include "utils.h"

//region Fonctions privées

void remplir_case(const Case *c, int color) {
    dessiner_rectangle(get_point(c), PLATEAU_BLOCK_TAILLE, PLATEAU_BLOCK_TAILLE, color);
}

void dessiner_fantome(const Case *c) {
    //todo
}

void dessiner_pacman(const Case *c) {
    //Dessine pacman selon sa position
    dessiner_cercle(get_point(c), PLATEAU_BLOCK_TAILLE, yellow);
}

void dessiner_gomme_pac(const Case *c) {
    //todo
}

void dessiner_gomme_bonus(const Case *c) {
    //todo
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

        if (!c->type_entity) { // il y a une entité
            if (c->type_entity == ENTITY_PACMAN) {
                dessiner_pacman(c);
            } else {
                dessiner_fantome(c);
            }
        } else { // pas d'entité, on dessine les gommes si besoin
            switch (c->type_gomme) {
                case GOMME_PAC:
                    dessiner_gomme_pac(c);
                    break;
                case GOMME_BONUS:
                    dessiner_gomme_bonus(c);
                    break;
            }
        }
    }
}



