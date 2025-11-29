#include <stdio.h>
#include <stdlib.h>
#include <affichage.h>

int main(int argc, char *argv[]) {
    printf("\n===== Récupération de la configuration réseau =====\n");
    getchar();
    
    int MAX_LIGNES = 1024;
    FILE* file = open_file(argv[1]);
    if (!file) {
        perror("Erreur d'ouverture du fichier\n");
        printf("Pour lancer: make run fichier=<nom du fichier>\n");
        return EXIT_FAILURE;
    }

    char* lignes[MAX_LIGNES];
    int nb_lignes = read_file(file, lignes);
    graphe g;
    parse_file(&g, lignes);

    int choix = -1;
    while(choix != 0){
        printf("\n========== Menu ==========\nQue souhaitez-vous faire ?\n");
        printf("0 - Quitter\n");
        printf("1 - Envoyer une trame\n");
        printf("2 - Afficher la table de commutation\n");
        printf("3 - Simuler le protocole STP\n");
        demande_entier(0, 3, &choix);
        switch(choix){
            case 1:
                envoyer_trame(&g);
                getchar();
                break;
            case 2:
                afficher_table(&g);
                getchar();
                break;
            case 3:
                simulate_stp(&g);
                getchar();
                break;
            default:
                break;
        }
    }
    quitter(&g, file, lignes, nb_lignes);
}