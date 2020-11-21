#include "./display.h"

#include "utils.h"
#include "main.h"

//region Fonctions privées

extern int window_width, window_height;
extern int plateau_width, plateau_height;

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

    if (c->porte) {
        remplir_case(c, COLOR_BG);
        dessiner_rectangle(get_point(c), PLATEAU_BLOCK_TAILLE, PLATEAU_BLOCK_TAILLE / 3, COLOR_WALL);
    } else if (c->wall) {
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
            case GOMME_EMPTY:
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

void dessiner_fantome_dead(const Fantome *fantome) {
    dessiner_disque(to_point(fantome->position), SIZE_FANTOME, white);
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
        if (fantome->alive) {
            if (partie->bonus_timer > 0) {
                dessiner_fantome_bleu(fantome);
            } else {
                dessiner_fantome(fantome);
            }
        } else {
            dessiner_fantome_dead(fantome);
        }
    }
}

void dessiner_fantome_bleu(const Fantome *fantome) {
    dessiner_disque(to_point(fantome->position), SIZE_FANTOME, bleumarine);
}

const int vie_radius = 10;

const int vie_padding = 5;

void dessiner_vies(int vies, Couleur color) {

    int ycent = plateau_height + ((window_height - plateau_height) / 2);

    Point center = {((window_height - plateau_height) / 2), ycent};

    //Point center = {corner.x + vie_radius,corner.y + vie_radius};

    for (int i = 0; i < vies; ++i) {
        dessiner_disque(center, vie_radius, color);

        center.x += vie_radius * 2 + vie_padding;
    }

}

void dessiner_texte_niveau(Partie *partie) {
    char texte[30];
    sprintf(texte, "Niveau %d", partie->level);

    int padding = 5;

    Point taille = taille_texte(texte, 15);

    Point corner = {window_width - taille.x - padding, plateau_height + padding};

    dessiner_rectangle((Point) {window_width - 30, plateau_height + 5}, 30, window_height - plateau_height, black);

    afficher_texte(texte, 15, corner, blue);
}

void dessiner_all_scores(char pString[MAX_SCORES][3], int score[MAX_SCORES], int nbscores) {
    int paddingy = 50;
    int paddingx = 20;

    Point corner = {plateau_width + paddingx,
                    0 + paddingy};

    for (int i = 0; i < nbscores; ++i) {
        char text[20];

        sprintf(text, "[%c%c%c] %d", pString[i][0], pString[i][1], pString[i][2], score[i]);

        corner.y += 20;

        afficher_texte(text, 15, corner, black);
    }


}


void dessiner_texte_center(char *text, int pt, int bgcolor, int fgcolor) {

    int twidth, theight;
    Point point = taille_texte(text, pt);
    twidth = point.x;
    theight = point.y;

    Point center = (Point) {plateau_width / 2, plateau_height / 2};

    int padding = 5;
    Point tcoin = (Point) {center.x - (twidth / 2) - 2, center.y - (theight / 2) - padding};

    Point bcoin = (Point) {tcoin.x - padding, tcoin.y};

    dessiner_rectangle(bcoin, twidth + padding, theight + padding, bgcolor);
    afficher_texte(text, pt, tcoin, fgcolor);
}


void dessiner_rectangle_score(Partie *partie) {

    int padding = 5;

    Point coin = {plateau_width + padding, padding};

    dessiner_rectangle(coin, (window_width - plateau_width - padding * 2), plateau_height - padding, COLOR_SCORE_BG);

}

void dessiner_score(Partie *partie) {

    char text[30];
    sprintf(text, "Points : %d", partie->points);
    int taille = 15;
    int paddingx = 5;
    int paddingy = 15;
    Point ttext = taille_texte(text, taille);

    int scoremid = plateau_width + ((window_width - plateau_width) / 2);

    Point corner = {scoremid - (ttext.x / 2) - paddingx,
                    0 + paddingy};
    dessiner_rectangle(corner, ttext.x, ttext.y, COLOR_SCORE_BG);
    afficher_texte(text, taille, corner, black);
}



