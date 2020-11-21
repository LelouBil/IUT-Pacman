#include "./display.h"

#include "utils.h"
#include "main.h"

//region Fonctions privées

extern int window_width, window_height;

void dessiner_fantome_bleu(const Fantome *fantome);

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

void dessiner_entities(Partie *partie) {

    dessiner_rect_cases(partie, partie->pacman.case_pacman->x, partie->pacman.case_pacman->y);
    for (int i = 0; i < NBFANTOMES; ++i) {
        const Fantome *fantome = &partie->fantomes[i];
        dessiner_rect_cases(partie, fantome->case_fantome->x, fantome->case_fantome->y);
    }


    dessiner_pacman(&partie->pacman);

    for (int i = 0; i < NBFANTOMES; ++i) {
        const Fantome *fantome = &partie->fantomes[i];
        if (partie->pacman.bonus_timer > 0) {
            dessiner_fantome_bleu(fantome);
        } else {
            dessiner_fantome(fantome);
        }
    }
}

void dessiner_fantome_bleu(const Fantome *fantome) {
    dessiner_fantome(fantome); //todo
}

void dessiner_texte_center(char *text, int pt, int bgcolor, int fgcolor) {

    int twidth, theight;
    Point point = taille_texte(text, pt);
    twidth = point.x;
    theight = point.y;

    Point center = (Point) {window_width / 2, window_height / 2};

    int padding = 5;
    Point tcoin = (Point) {center.x - (twidth / 2) - 2, center.y - (theight / 2) - padding};

    Point bcoin = (Point) {tcoin.x - padding, tcoin.y};

    dessiner_rectangle(bcoin, twidth + padding, theight + padding, bgcolor);
    afficher_texte(text, pt, tcoin, fgcolor);
}

void dessiner_score(Partie *partie) {

    char text[30];
    sprintf(text, "Points : %d", partie->max_gommes - partie->gomme_restant);
    int taille = 15;
    int padding = 5;
    Point ttext = taille_texte(text, taille);
    Point corner = {window_width - ttext.x - padding,
                    0 + padding};
    dessiner_rectangle(corner, ttext.x, ttext.y, COLOR_WALL);
    afficher_texte(text, taille, corner, yellow);
}

