#include "./main.h"
#include "display.h"
#include "timings.h"
#include "gameplay.h"
#include "collisions.h"
#include "pathfinding.h"
#include "ai.h"

int window_width;
int window_height;

int game_loop();

int check_game_end(Partie *partie);


void levels_loop();

Partie load_game_data(char *const plan_file) {
    printf("Chargement du plan...\n");
    Partie partie = charge_plan(plan_file);

    //region debug
#if DEBUG == 1

    printf("Affichage du plan...\n");
    for (int y = 0; y != partie.ymax; y++) { // ligne par ligne vu que terminal
        for (int x = 0; x != partie.xmax; x++)
            printf("%c", partie.plateau[x][y].wall ? '*' : ' ');
        printf("\n");
    }
#endif
//endregion

    return partie;
}

Partie init_graphique(const Partie *partie) {
    printf("Ouverture fenetre\n");

    window_width = (*partie).xmax * PLATEAU_BLOCK_TAILLE;
    window_height = (*partie).ymax * PLATEAU_BLOCK_TAILLE;
    ouvrir_fenetre(window_width, window_height);

    return (*partie);
}

int pathfinding_debug = 0;
int main(int argc, char **argv) {

    // sans ca le deboguage sous windows n'affiche les printf qu'apres la fin du programme
#if DEBUG == 1
    setbuf(stdout, 0);
#endif


    levels_loop();


    fermer_fenetre();
    return 0;
}

void levels_loop() {

    int level = 0;
    int exitcode = GAME_WIN;
    char leveltext[50];

    Partie p = load_game_data("data/test.txt");

    init_graphique(&p);

    while (exitcode == GAME_WIN) {

        Partie partie = load_game_data("data/test.txt");

        partie.level = ++level;
        sprintf(leveltext, "Niveau %d", level);

        dessiner_plateau(&partie);

        reset_timers();


        dessiner_texte_center(leveltext, 20, gray, blue);
        actualiser();

        attendre_touche();

        dessiner_plateau(&partie);

        dessiner_entities(&partie);

        exitcode = game_loop(&partie);
    }

    if (exitcode == GAME_LOST) {
        dessiner_texte_center("Perdu !", 20, red, black);
        actualiser();
        attendre_touche();
    }

}

int game_loop(Partie *partie) {
    while (1) {


        traiter_evenements();

        if (touche_a_ete_pressee(SDLK_ESCAPE)) {
            return GAME_QUIT;
        }

        if (touche_a_ete_pressee(SDLK_SPACE)) {
            pathfinding_debug = !pathfinding_debug;
        }

        key_events(partie);

        fantome_event_manager(partie);

        if (check_collisions(partie)) {
            return GAME_LOST;
        }

        if (check_game_end(partie)) {
            return GAME_WIN;
        }

        dessiner_entities(partie);

        //printf("Pacman case %d, %d\n", partie->pacman.case_pacman->x, partie->pacman.case_pacman->y);

        actualiser();

        attente(framerate);

        reinitialiser_evenements();

    }
}


int check_game_end(Partie *partie) {
    return partie->gomme_restant == 0;
}


