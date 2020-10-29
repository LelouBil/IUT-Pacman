#include "./partie.h"
#include "utils.h"
#include "utils.h"

Partie charge_plan(char *fichier) {
    Char_Partie p;
    FILE *f = fopen(fichier, "r");
    p.plateau = NULL;
    int res, l, c;
    int nbf;        // Nb de fantômes trouvés sur le plan
    int nbb;        // Nb de bonus trouvés sur le plan
    char ch;

    if (!f) {
        printf("Impossible d'ouvrir '%s'\n", fichier);
        exit(0);
    }

    // Lecture des dimensions du plan en en-tête
    res = fscanf(f, "%d %d\n", &p.L, &p.C);


    // Si on n'a pas pu lire deux entiers ou s'ils sont incorrects
    if (res != 2 || p.C < 2 || p.L < 2 || p.C > 800 || p.L > 600) {
        printf("Dimensions du tableau lues dans '%s' incorrectes\n", fichier);
        fclose(f);
        exit(0);
    }

    printf("Dimensions lues: %d x %d\n", p.L, p.C);

    // ALLOCATION DYNAMIQUE
    // Allocation du tableau de *L pointeurs sur lignes
    p.plateau = (char **) malloc(p.L * sizeof(char *));
    if (p.plateau == NULL) {
        printf("Allocation dynamique impossible\n");
        fclose(f);
        exit(0);
    }

    // Allocation des tableaux de *C caractères
    for (l = 0; l != p.L; l++) {
        p.plateau[l] = (char *) malloc(p.C * sizeof(char));
        if (p.plateau[l] == NULL) {
            printf("Allocation dynamique impossible\n");
            fclose(f);
            exit(0);
        }
    }


    // LECTURE DES LIGNES DU PLAN
    l = 0;
    res = 0;
    nbf = 0;
    nbb = 0;
    while (res != EOF) // Lecture de chaque ligne
    {
        c = 0;
        while (1) {
            res = fscanf(f, "%c", &ch);
            if (res == EOF)
                break;

            if (c > p.C) // Si trop de colonnes...
            {
                printf("Ligne %d colonne %d: trop de colonnes\n", l, c);
                fclose(f);
                exit(0);
            }

            if (c == p.C) // Si fin de ligne supposée...
            {
                if (ch == '\n') // Si fin de ligne réelle, on quitte la boucle
                {
                    break;
                } else // Sinon trop de caractères
                {
                    printf("Ligne %d: trop de caractères\n", l);
                    fclose(f);
                    exit(0);
                }
            }

            // ...sinon, nous ne sommes pas à la fin de la ligne.
            // Si on lit un caractère interdit...

            if (ch != '.' && ch != ' ' && ch != '*' && ch != 'P' && ch != 'F' && ch != 'B' && ch != 'C') {
                if (ch == '\n') // Si c'est un saut de ligne
                    printf("Ligne %d: trop peu de caractères\n", l);
                else
                    printf("Ligne %d: caractère '%c' incorrect\n", l, ch);
                fclose(f);
                exit(0);
            }

            if (ch == 'P') {
                p.pacman.x = l;
                p.pacman.y = c;
            } else if (ch == 'F') {
                if (nbf > NBFANTOMES) {
                    printf("Ligne %d:  un fantôme de trop!\n", l);
                    fclose(f);
                    exit(0);
                }
                p.fantomes[nbf].x = l;
                p.fantomes[nbf].y = c;
                nbf++;
            } else if (ch == 'B')
                nbb++;

            p.plateau[l][c] = ch; // Ecriture dans le plan

            c++; // caractère suivant
        }
        l++; // ligne suivante
    }

    fclose(f); // Fermeture du flux de lecture du fichier

    // Si à la lecture de EOF on n'est pas sur la *V+1 ème ligne...
    if (l != p.L + 1) {
        printf("Ligne %d: nb de lignes incorrect\n", l);
        exit(0);
    }

    // Si nb de fantômes incorrect...
    if (nbf != NBFANTOMES) {
        printf("Nb de fantômes incorrect sur le plan\n");
        fclose(f);
        exit(0);
    }

    // Si pas de bonus....
    if (nbb == 0) {
        printf("Aucun bonus sur le plan!\n");
        fclose(f);
        exit(0);
    }
    p.nbbonus = nbb;

    return convert_partie(p);
}

Partie convert_partie(Char_Partie charPartie) {
    Partie partie_data;

    partie_data.xmax = charPartie.C;
    partie_data.ymax = charPartie.L;

    partie_data.plateau = calloc(partie_data.xmax, sizeof(Case *));



    //x et y inversé parce que on pase d'un tableau Colonnes par Lignes a Lignes par colonnes

    for (int x = 0; x < partie_data.xmax; ++x) {
        partie_data.plateau[x] = calloc(partie_data.ymax, sizeof(Case));
        for (int y = 0; y < partie_data.ymax; ++y) {
            partie_data.plateau[x][y] = (Case) {x, y, charPartie.plateau[y][x] == '*', 0};
        }
    }


    int fantome_name = FANTOME_BLINKY;

    for (int i = 0; i < NBFANTOMES; ++i) {
        Pos fantome_pos = charPartie.fantomes[i];
        Case *c = &partie_data.plateau[fantome_pos.x][fantome_pos.y];
        partie_data.fantomes[i] = (Fantome) {c, 0, get_case_center(c), fantome_name};
        fantome_name++;
    }

    partie_data.pacman.case_pacman = &partie_data.plateau[charPartie.pacman.y][charPartie.pacman.x];
    partie_data.pacman.position = get_case_center(partie_data.pacman.case_pacman);

    //libération de l'ancienne struct partie

    for (int i = 0; i < charPartie.C; ++i) {
        free(charPartie.plateau[i]);
    }
    free(charPartie.plateau);

    return partie_data;
}
