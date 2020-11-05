#include "./gameplay.h"
#include "utils.h"
#include "timings.h"
#include "display.h"

#define pacman_px_per_s (PLATEAU_BLOCK_TAILLE * PACMAN_SPEED)

#define pacman_px_per_frame (pacman_px_per_s / framerate)


int dir_from_key(int key) {
    switch (key) {
        case SDLK_UP:
            return DIR_HAUT;
        case SDLK_DOWN:
            return DIR_BAS;
        case SDLK_RIGHT:
            return DIR_DROITE;
        case SDLK_LEFT:
            return DIR_GAUCHE;
        default:
            return -1;
    }
}

Pos dir_to_vector(int key) {
    switch (key) {
        case DIR_HAUT:
            return (Pos) {0, -1};
        case DIR_BAS:
            return (Pos) {0, 1};
        case DIR_DROITE:
            return (Pos) {1, 0};
        case DIR_GAUCHE:
            return (Pos) {-1, 0};
        default:
            return (Pos) {0, 0};
    }
}

int get_perp(int dir) {
    switch (dir) {
        case DIR_HAUT:
        case DIR_BAS:
            return DIR_DROITE;

        case DIR_DROITE:
        case DIR_GAUCHE:
            return DIR_HAUT;
        default:
            return 0;
    }
}


int pacman_aligned_dir(Pacman *pacman, int dir) {
    Pos p = get_case_center(pacman->case_pacman);
    switch (dir) {
        case DIR_HAUT:
        case DIR_BAS:
            return pacman->position.x == p.x;
        case DIR_GAUCHE:
        case DIR_DROITE:
            return pacman->position.y == p.y;
        default:
            return 0;
    }
}


void pacman_do_move(Partie *partie, int dir);

void pacman_deplace(Partie *partie) {

    Pacman *pacman = &partie->pacman;

    for (int key = SDLK_UP; key < SDLK_LEFT + 1; ++key) {
        int dir = dir_from_key(key);
        if (touche_a_ete_pressee(key)) {
            Case *target = get_case_at(partie, pacman->case_pacman, dir);
            if ((target == NULL) || (pacman_aligned_dir(pacman, dir) && target->wall ==
                                                                        0)) //on autorise le changement de direction si pacman est aligné avec cette colonne/ligneet si il n'y a pas de mur a cet endroit
                pacman->direction = dir;
        }
    }

    if (!pacman->oob) {
        Case *target = get_case_at(partie, pacman->case_pacman, pacman->direction);

        if ((target == NULL || !target->wall) || !pacman_aligned_dir(pacman, get_perp(
                pacman->direction))) { // on continue d'avancer si pacman n'as pas touché de mur OU si il n'est pas encore aligné sur la direction dans laquelle il avance
            pacman_do_move(partie, pacman->direction);
        } else {

        }
    } else {
        pacman_do_move(partie, pacman->direction);
    }


    Pos p = partie->pacman.position;

    int cx = p.x / PLATEAU_BLOCK_TAILLE;
    int cy = p.y / PLATEAU_BLOCK_TAILLE;

    if (cx >= 0 && cx < partie->xmax && cy >= 0 && cy < partie->ymax) {
        pacman->oob = 0;
    } else {
        pacman->oob = 1;
    }


    int tunnel = 0;

    if (cx > partie->xmax) {
        cx = -1;
        tunnel = 1;
    } else if (cx < 0) {
        cx = partie->xmax;
        tunnel = 1;
    }

    if (cy > partie->ymax) {
        cy = -1;
        tunnel = 1;
    } else if (cy < 0) {
        cy = partie->ymax;
        tunnel = 1;
    }


    if (tunnel) {

        partie->pacman.position = (Pos) {cx * PLATEAU_BLOCK_TAILLE + PLATEAU_BLOCK_TAILLE / 2,
                                         cy * PLATEAU_BLOCK_TAILLE + PLATEAU_BLOCK_TAILLE / 2};
    }

    if (!pacman->oob) {
        partie->pacman.case_pacman = &partie->plateau[cx][cy];
    }
    dessiner_rect_cases(partie, cx, cy);
    dessiner_pacman(&partie->pacman);

}


void pacman_do_move(Partie *partie, int dir) {
    Pos vector = dir_to_vector(dir);
    vector.x *= pacman_px_per_frame;
    vector.y *= pacman_px_per_frame;
    partie->pacman.position.x += vector.x;
    partie->pacman.position.y += vector.y;
}


void pacman_evenement(Partie *arg_partie) {
    //appel de fonction pacman selon les evenements
    pacman_deplace(arg_partie);


}

void key_events(Partie *partie) {
    pacman_evenement(partie);
}

