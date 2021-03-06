#include "./main.h"
#include "display.h"
#include "timings.h"
#include "gameplay.h"
#include "collisions.h"
#include "pathfinding.h"
#include "ai.h"

//display.c en a besoins
int window_width;
int window_height;

int plateau_width;
int plateau_height;

int game_loop();

int check_game_end(Partie *partie);


void levels_loop();


void save_score();

void init_graphique(int xmax, int ymax) {
    printf("Ouverture fenetre\n");

    plateau_width = xmax * PLATEAU_BLOCK_TAILLE;
    plateau_height = ymax * PLATEAU_BLOCK_TAILLE;

    //pour les elements de l'interface
    window_width = plateau_width + 200;
    window_height = plateau_height + 50;

    ouvrir_fenetre(window_width, window_height);

}


char pseudos[MAX_SCORES][3];

int nbscores;

int scores[MAX_SCORES];

int pathfinding_debug = 0;

void load_scores() {

    FILE *scoref = fopen(SCORE_FILE, "r");
    if (scoref == NULL) {
        nbscores = 0;
        return;
    }

    const int maxlen = 500;

    char buffer[maxlen];

    while (fgets(buffer, maxlen - 1, scoref)) {
        sscanf(buffer, "%c%c%c %d", &pseudos[nbscores][0], &pseudos[nbscores][1], &pseudos[nbscores][2],
               &scores[nbscores]);
        nbscores++;
    }

    fclose(scoref);
}

int main(int argc, char **argv) {

    // sans ca le deboguage sous windows n'affiche les printf qu'apres la fin du programme
#if DEBUG == 1
    setbuf(stdout, 0);
#endif

    load_scores();

    levels_loop();

    return 0;
}


void levels_loop() {


    int level = 0;
    int score = 0;
    int vies = MAX_VIES;

    int exitcode = GAME_WIN;

    char leveltext[50];

    Partie partieTemplate = load_partie_template("data/test.txt");

    init_graphique(partieTemplate.xmax, partieTemplate.ymax);

    dessiner_rectangle_score(&partieTemplate);

    dessiner_all_scores(pseudos, scores, nbscores);

    Partie partie;


    while (vies > 0 && exitcode != GAME_QUIT && level < 256) {


        if (exitcode == GAME_WIN) {
            partie = clone_partie(&partieTemplate);
        } else {
            // si on perd une vie on réinitialise toute la partie sauf les gommes
            // on reprend les gommes mangées et non mangées de la partie d'avant.

            Partie newPartie = clone_partie(&partieTemplate);

            for (int x = 0; x < newPartie.xmax; ++x) {
                for (int y = 0; y < newPartie.ymax; ++y) {
                    newPartie.plateau[x][y].gomme = partie.plateau[x][y].gomme;
                }
            }
            newPartie.gomme_restant = partie.gomme_restant;
            partie = newPartie;
        }

        //printf("Pacman pos %d %d\n", partie.pacman.position.x, partie.pacman.position.y);

        partie.level = ++level;
        partie.points = score;
        partie.vies = vies;

        //partie.gomme_restant = 20;

        sprintf(leveltext, "Niveau %d", level);

        dessiner_vies(MAX_VIES, black); //en noir pour vider

        dessiner_vies(vies, yellow);


        dessiner_plateau(&partie);
        dessiner_texte_niveau(&partie);

        reset_timers();

        dessiner_texte_center(leveltext, 20, gray, blue);

        actualiser();

        while (attendre_touche() != SDLK_RETURN);

        dessiner_plateau(&partie);

        dessiner_entities(&partie);

        exitcode = game_loop(&partie); //boucle principale

        score = partie.points;

        if (exitcode == GAME_LOST) {

            vies--;
            char texte[50];
            sprintf(texte, "Perdu ! Il vous reste %d vies ", vies);

            dessiner_texte_center(texte, 20, red, black);

            actualiser();
            while (attendre_touche() != SDLK_RETURN);
        } else if (exitcode == GAME_WIN) {
            score += 500;
        }

    }

    //si on a plus de vies ou quitté

    if (exitcode == GAME_LOST) {
        dessiner_texte_center("Perdu !", 20, red, black);
        actualiser();
    }

    save_score(score);

}

int game_loop(Partie *partie) {
    while (1) {

        traiter_evenements();

        if (touche_a_ete_pressee(SDLK_ESCAPE) || _test_arret()) {

            dessiner_texte_center("Etes vous sur de quitter ? (O/N)", 20, black, blue);
            actualiser();

            int t;
            do {
                t = attendre_touche();
            } while (t != SDLK_o && t != SDLK_n);

            if (t == SDLK_o) {
                return GAME_QUIT;
            }
            dessiner_plateau(partie);
        }

//        if (touche_a_ete_pressee(SDLK_SPACE)) {
//            pathfinding_debug = !pathfinding_debug;
//        }

        pacman_deplace(partie);

        if (check_collisions(partie)) { //s'occupe des pac gommes aussi
            return GAME_LOST;
        }

        if (check_game_end(partie)) {
            return GAME_WIN;
        }

        fantome_event_manager(partie); //collision avant fantomes


        dessiner_entities(partie);

        //si pacman ou un fantome est dans un tunnel ou au bors de l'ecran, on redessine les scores par dessus
        for (int i = 0; i < NBFANTOMES; ++i) {
            if (partie->pacman.oob || partie->fantomes[i].oob //hors de l'ecran
                || partie->pacman.case_pacman->x == partie->xmax - 1
                || partie->pacman.case_pacman->y == partie->ymax - 1 //pacman a gauche ou a droite
                || partie->fantomes[i].case_fantome->x == partie->xmax - 1
                || partie->fantomes[i].case_fantome->y == partie->ymax - 1) { //fantome a gauche ou a droite
                dessiner_rectangle_score(partie);
                dessiner_all_scores(pseudos, scores, nbscores);
                break; // pas besoins de le faire plusieurs fois donc au premier trouvé on break.
            }
        }

        dessiner_score(partie);

        //printf("Pacman case %d, %d\n", partie->pacman.case_pacman->x, partie->pacman.case_pacman->y);

        if (partie->bonus_timer > 0) {
            partie->bonus_timer -= framerate;
        }

        actualiser();

        attente(framerate);

        reinitialiser_evenements();

    }
}


// pour le tri
void swap(int *xp, int *yp) {
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}


void save_score(int score) {


    // si on a déja le nombre max de scores et qu'on a pas battu le plus bas, on sauvegarde pas
    if (nbscores > 0 && nbscores >= MAX_SCORES && score <= scores[nbscores - 1]) return;

    dessiner_texte_center("     Regardez le terminal     ", 20, gray, blue);
    actualiser();

    printf("Veuillez entrer votre pseudo (3 caracteres) :");

    char pseudo[3];
    scanf("%c%c%c", &pseudo[0], &pseudo[1], &pseudo[2]);
    for (int i = 0; i < 3; ++i) {
        if (!isalnum(pseudo[i])) { // on filtre pour les caracteres speciaux, genre tab ou \n
            pseudo[i] = '*';
        }
    }

    fermer_fenetre();
    printf("Votre pseudo est [%c%c%c]\n", pseudo[0], pseudo[1], pseudo[2]);

    FILE *score_file = fopen(SCORE_FILE, "w");

    if (score_file == NULL) {
        printf("Erreur lors de l'ouverture du fichier %s", SCORE_FILE);
        return;
    }

    //normalement on a toujours la place si ici
    if (nbscores < MAX_SCORES) {
        scores[nbscores] = score;
        pseudos[nbscores][0] = pseudo[0];
        pseudos[nbscores][1] = pseudo[1];
        pseudos[nbscores][2] = pseudo[2];
        nbscores++;
    }


    //tri a bulle, on swap juste les scores et les pseudos en meme temps

    char temp[3];

    for (int i = 0; i < nbscores; i++)

        for (int j = 0; j < nbscores - i; j++)
            if (scores[j] < scores[j + 1]) {
                swap(&scores[j], &scores[j + 1]);
                temp[0] = pseudos[j][0];
                temp[1] = pseudos[j][1];
                temp[2] = pseudos[j][2];

                pseudos[j][0] = pseudos[j + 1][0];
                pseudos[j][1] = pseudos[j + 1][1];
                pseudos[j][2] = pseudos[j + 1][2];

                pseudos[j + 1][0] = temp[0];
                pseudos[j + 1][1] = temp[1];
                pseudos[j + 1][2] = temp[2];
            }


    for (int i = 0; i < nbscores; ++i) {
        fprintf(score_file, "%c%c%c %d\n", pseudos[i][0], pseudos[i][1], pseudos[i][2], scores[i]);
    }


    fclose(score_file);
}


int check_game_end(Partie *partie) {
    return partie->gomme_restant == 0;
}


