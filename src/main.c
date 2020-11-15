#include "./main.h"
#include "display.h"
#include "timings.h"
#include "gameplay.h"
#include "collisions.h"
#include "pathfinding.h"
#include "ai.h"


void game_loop();

void reinit_partie();

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

    ouvrir_fenetre((*partie).xmax * PLATEAU_BLOCK_TAILLE, (*partie).ymax * PLATEAU_BLOCK_TAILLE);

    dessiner_plateau(partie); // a faire qu'une seule fois

    dessiner_entities(partie);

    return (*partie);
}

int pathfinding_debug = 0;
Partie mainpartie;
int main(int argc, char **argv) {

    // sans ca le deboguage sous windows n'affiche les printf qu'apres la fin du programme
#if DEBUG == 1
    setbuf(stdout, 0);
#endif


    reinit_partie();

    game_loop(&mainpartie);


    fermer_fenetre();
    return 0;
}

void game_loop(Partie *partie) {
    int EXIT_FLAG = 0;
    partie->gomme_restant = 5;
    while (!EXIT_FLAG) {


        traiter_evenements();

        if (touche_a_ete_pressee(SDLK_ESCAPE)) {
            EXIT_FLAG = 1;
        }

        if (touche_a_ete_pressee(SDLK_SPACE)) {
            pathfinding_debug = !pathfinding_debug;
        }

        key_events(partie);

        fantome_event_manager(partie);

        check_collisions(partie);

        dessiner_entities(partie);

        if (partie->gomme_restant == 0) {


            reinit_partie();
        }

        //printf("Pacman case %d, %d\n", partie->pacman.case_pacman->x, partie->pacman.case_pacman->y);

        actualiser();

        attente(framerate);

        reinitialiser_evenements();

    }
}

void reinit_partie() {
    mainpartie = load_game_data("data/test.txt");

    mainpartie = init_graphique(&mainpartie);
    reset_timers();
}


