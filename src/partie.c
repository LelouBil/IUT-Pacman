#include "./partie.h"
#include "utils.h"
#include "utils.h"

Partie load_partie_template(char *fichier) {

    Partie p;
    FILE *f = fopen(fichier, "r");
    p.plateau = NULL;
    int res, y, x;
    int nbf;        // Nb de fantômes trouvés sur le plan
    int nbb;        // Nb de bonus trouvés sur le plan
    char ch;

    if (!f) {
        printf("Impossible d'ouvrir '%s'\n", fichier);
        exit(0);
    }

    // Lecture des dimensions du plan en en-tête
    res = fscanf(f, "%d %d\n", &p.ymax, &p.xmax);


    // Si on n'a pas pu lire deux entiers ou s'ils sont incorrects
    if (res != 2 || p.xmax < 2 || p.ymax < 2 || p.xmax > 800 || p.ymax > 600) {
        printf("Dimensions du tableau lues dans '%s' incorrectes\n", fichier);
        fclose(f);
        exit(0);
    }

    printf("Dimensions lues: %d x %d\n", p.ymax, p.xmax);

    // ALLOCATION DYNAMIQUE
    // Allocation du tableau de *xmax pointeurs sur lignes
    p.plateau = malloc(p.xmax * sizeof(Case *));
    if (p.plateau == NULL) {
        printf("Allocation dynamique impossible\n");
        fclose(f);
        exit(0);
    }

    // Allocation des tableaux de *ymax Case
    for (y = 0; y != p.xmax; y++) {
        p.plateau[y] = malloc(p.ymax * sizeof(Case));
        if (p.plateau[y] == NULL) {
            printf("Allocation dynamique impossible\n");
            fclose(f);
            exit(0);
        }
    }

    //p.fantomes = malloc(NBFANTOMES * sizeof(Fantome));

    p.gomme_restant = 0;
    p.bonus_timer = 0;
    p.max_gommes = 0;
    p.level = 0;
    p.vies = 0;
    p.points = 0;

    // LECTURE DES LIGNES DU PLAN
    y = 0;
    res = 0;
    nbf = 0;
    nbb = 0;
    while (res != EOF) // Lecture de chaque ligne
    {
        x = 0;
        while (1) {
            res = fscanf(f, "%c", &ch);
            if (res == EOF)
                break;

            if (x > p.xmax) // Si trop de colonnes...
            {
                printf("Ligne %d colonne %d: trop de colonnes\n", y, x);
                fclose(f);
                exit(0);
            }

            if (x == p.xmax) // Si fin de ligne supposée...
            {
                if (ch == '\n') // Si fin de ligne réelle, on quitte la boucle
                {
                    break;
                } else // Sinon trop de caractères
                {
                    printf("Ligne %d: trop de caractères\n", y);
                    fclose(f);
                    exit(0);
                }
            }

            p.plateau[x][y].x = x;
            p.plateau[x][y].y = y;
            p.plateau[x][y].wall = 0;
            p.plateau[x][y].gomme = 0;


            switch (ch) {
                case '.':
                    p.plateau[x][y].gomme = 1;
                    p.gomme_restant++;
                    p.max_gommes++;
                    break;
                case '*':
                    p.plateau[x][y].wall = 1;
                    break;
                case 'P':
                    p.pacman.case_pacman = &p.plateau[x][y];
                    p.pacman.position = get_case_center(p.pacman.case_pacman);
                    p.pacman.direction = DIR_HAUT;
                    p.pacman.oob = 0;
                    break;
                case 'F':

                    if (nbf > NBFANTOMES) {
                        printf("Ligne %d:  un fantôme de trop!\n", y);
                        fclose(f);
                        exit(0);
                    }
                    p.fantomes[nbf].case_fantome = &p.plateau[x][y];
                    p.fantomes[nbf].direction = 0;
                    p.fantomes[nbf].position = get_case_center(&p.plateau[x][y]);
                    p.fantomes[nbf].type = nbf;
                    p.fantomes[nbf].oob = 0;
                    p.fantomes[nbf].alive = 1;
                    p.spawn_fantome[nbf] = p.fantomes[nbf].case_fantome;
                    p.fantomes[nbf].speed = 1;
                    p.fantomes[nbf].flag = 0;
                    nbf++;

                    break;
                case 'B':
                    p.plateau[x][y].gomme = GOMME_BONUS;
                    nbb++;
                    break;
                case ' ':
                    break;
                default:
                    if (ch == '\n') // Si c'est un saut de ligne
                        printf("Ligne %d: trop peu de caractères\n", y);
                    else
                        printf("Ligne %d: caractère '%c' incorrect\n", y, ch);
                    fclose(f);
                    exit(0);
            }


            x++; // caractère suivant
        }
        y++; // ligne suivante
    }

    fclose(f); // Fermeture du flux de lecture du fichier

    // Si à la lecture de EOF on n'est pas sur la *V+1 ème ligne...
    if (y != p.ymax + 1) {
        printf("Ligne %d: nb de lignes incorrect\n", y);
        exit(0);
    }

    // Si nb de fantômes incorrect...
    if (nbf != NBFANTOMES) {
        printf("Nb de fantômes incorrect sur le plan\n");
        fclose(f);
        exit(0);
    }


    return p;
}


Partie clone_partie(Partie *p) {
    Partie partie_data;

    partie_data.xmax = p->xmax;
    partie_data.ymax = p->ymax;
    partie_data.gomme_restant = p->gomme_restant;
    partie_data.max_gommes = p->max_gommes;

    partie_data.level = p->level;
    partie_data.points = p->points;
    partie_data.bonus_timer = p->bonus_timer;

    partie_data.plateau = malloc(partie_data.xmax * sizeof(Case *));


    for (int x = 0; x < partie_data.xmax; ++x) {
        partie_data.plateau[x] = malloc(partie_data.ymax * sizeof(Case));
        for (int y = 0; y < partie_data.ymax; ++y) {
            partie_data.plateau[x][y] = p->plateau[x][y];
        }
    }


    for (int i = 0; i < NBFANTOMES; ++i) {
        partie_data.fantomes[i] = p->fantomes[i];
        //on répare les pointeurs
        partie_data.fantomes[i].case_fantome = &(partie_data.plateau[p->fantomes[i].case_fantome->x][p->fantomes[i].case_fantome->y]);
        partie_data.spawn_fantome[i] = &(partie_data.plateau[p->spawn_fantome[i]->x][p->spawn_fantome[i]->y]);

    }

    partie_data.pacman = p->pacman;
    partie_data.vies = p->vies;
    partie_data.pacman.direction = DIR_HAUT;
    //reparation du pointeur
    partie_data.pacman.case_pacman = &partie_data.plateau[p->pacman.case_pacman->x][p->pacman.case_pacman->y];

    return partie_data;
}