#include "./main.h"
#include "display.h"
#include "timings.h"
#include "gameplay.h"
#include "collisions.h"
#include "pathfinding.h"
#include "ai.h"


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


    fermer_fenetre();
    return 0;
}


void levels_loop() {

    int maxvie = 3;

    int level = 0;
    int score = 0;
    int vies = maxvie;

    int exitcode = GAME_WIN;
    char leveltext[50];

    Partie partieTemplate = load_partie_template("data/test.txt");


    init_graphique(partieTemplate.xmax, partieTemplate.ymax);

    dessiner_rectangle_score(&partieTemplate);

    dessiner_all_scores(pseudos, scores, nbscores);


    while (vies > 0 && exitcode != GAME_QUIT) {

        Partie partie = clone_partie(&partieTemplate);

        printf("Pacman pos %d %d\n", partie.pacman.position.x, partie.pacman.position.y);

        partie.level = ++level;
        partie.points = score;
        partie.vies = vies;
        partie.gomme_restant = 20;
        sprintf(leveltext, "Niveau %d", level);


        dessiner_vies(maxvie, black);
        dessiner_vies(vies, yellow);
        dessiner_plateau(&partie);
        dessiner_texte_niveau(&partie);

        reset_timers();


        dessiner_texte_center(leveltext, 20, gray, blue);
        actualiser();

        while (attendre_touche() != SDLK_RETURN);

        dessiner_plateau(&partie);

        dessiner_entities(&partie);

        exitcode = game_loop(&partie);

        score = partie.points;
        if (exitcode == GAME_LOST) {

            vies--;
            char texte[50];
            sprintf(texte, "Perdu ! Il vous reste %d vies ", vies);
            dessiner_texte_center(texte, 20, red, black);
            actualiser();
            while (attendre_touche() != SDLK_RETURN);
        }
    }

    if (exitcode == GAME_LOST) {
        dessiner_texte_center("Perdu !", 20, red, black);
        actualiser();
        save_score(score);
    }

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
                save_score(partie->points);
                return GAME_QUIT;
            }
            dessiner_plateau(partie);
        }

//        if (touche_a_ete_pressee(SDLK_SPACE)) {
//            pathfinding_debug = !pathfinding_debug;
//        }

        key_events(partie);

        if (check_collisions(partie)) {
            return GAME_LOST;
        }

        if (check_game_end(partie)) {
            return GAME_WIN;
        }

        fantome_event_manager(partie); //collision avant fantome pour changement dir


        dessiner_entities(partie);
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

void swap(int *xp, int *yp) {
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}


void save_score(int score) {

    if (nbscores > 0 && nbscores >= MAX_SCORES && score <= scores[nbscores - 1]) return; //todo si pas place

    dessiner_texte_center("     Regardez le terminal     ", 20, gray, blue);
    actualiser();
    printf("Veuillez entrer votre pseudo (3 caracteres) :");
    char pseudo[3];
    scanf("%c%c%c", &pseudo[0], &pseudo[1], &pseudo[2]);
    fermer_fenetre();
    printf("Votre pseudo est [%s]\n", pseudo);

    FILE *score_file = fopen(SCORE_FILE, "w");
    if (score_file == NULL) {
        printf("Erreur lors de l'ouverture du fichier %s", SCORE_FILE);
        return;
    }
    if (nbscores < MAX_SCORES) {
        scores[nbscores] = score;
        pseudos[nbscores][0] = pseudo[0];
        pseudos[nbscores][1] = pseudo[1];
        pseudos[nbscores][2] = pseudo[2];
        nbscores++;
    }

    //todo trier score

    char temp[3];

    for (int i = 0; i < nbscores; i++)

        // Last i elements are already in place
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


