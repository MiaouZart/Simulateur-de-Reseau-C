#include <affichage.h>
#include <adress.h>
#include <trame.h>
#include <machine.h>
#include <bpdu.h>
#include <bridge.h>

void simulate_stp(graphe* g) {
    // Procédure qui simule le spanning tree protocol et affiche le résultat.
    printf("\n====== Simulation STP =====\n");
    getchar();
    size_t nb_switch = g->nbSwitch;
    printf("Nombre de switches: %ld\n", nb_switch);

    int iterations = 0;
    const int max_iterations = 100;
    bool changed=true;

    while (changed){
        changed = false;
        printf("\n--- Itération %d ---\n", iterations + 1);
        for (size_t i = 0; i < nb_switch; i++) {
            bridge* br = (bridge*)g->sommets[i]->machine->machine;
            send_bdpu(br);
        }

        for (size_t i = 0; i < nb_switch; i++) {
            
            bridge* br = (bridge*)g->sommets[i]->machine->machine;
           
            for (size_t j = 0; j < br->nb_ports; j++) {
                if (br->ports[j] && br->ports[j]->type_changed) {
                    changed = true;
                    br->ports[j]->type_changed = false; 
                }
            }
        }
        
        iterations++;
        if (iterations >= max_iterations) {
            printf("\nlimite d'itérations atteinte sans convergence\n");
            break;
        }
        
    } ;

    if (iterations == max_iterations) {
        printf("\nlimite d'itérations atteinte sans convergence\n");
    } else {
        printf("\nTopologie stable après %d itérations\n", iterations);
    }

    printf("\nÉtat final des ports:\n");
    for (size_t i = 0; i < nb_switch; i++) {
        bridge* br = (bridge*)g->sommets[i]->machine->machine;
        printf("Switch %ld (\033[33m%s\033[0m):\n", i, to_string_mac(&br->addr_mac, (char[20]){0}));
        for (size_t j = 0; j < br->nb_ports; j++) {
            if (br->ports[j] && br->ports[j]->port) {
                interface* inter = br->ports[j]->port;
                printf("  Port %ld [", j);
                
                switch (br->ports[j]->type) {
                    case RACINE: printf("\033[32mRACINE\033[0m]     "); break;
                    case DESIGNE: printf("\033[34mDESIGNE\033[0m]    "); break;
                    case NONDESIGNE: printf("\033[31mNON-DESIGNE\033[0m]"); break;
                    default: printf("INCONNU]    ");
                }
                
                printf(" -> Connecté à: ");
                
                if (inter->connected_to) {
                    machine* connected_machine = inter->connected_to->machine;
                    if (connected_machine->type == SWITCH) {
                        bridge* connected_br = (bridge*)connected_machine->machine;
                        printf("Switch  %p (MAC: \033[33m%s\033[0m)", 
                            (void*)connected_machine,
                            to_string_mac(&connected_br->addr_mac, (char[20]){0}));
                    } else {
                        printf("Station %p (MAC: %s)", 
                            (void*)connected_machine,
                            to_string_mac(&connected_machine->addr_mac, (char[20]){0}));
                    }
                } else {
                    printf("Non connecté");
                }
                
                printf("\n");
            }
        }
    }
}

void vider_buffer() {
    // Procédure qui vide le buffer de la console.
    int c;
    while ((c = getchar()) != '\n' && c != EOF); // consomme jusqu’à la fin de ligne
}

void demande_entier(int min, int max, int* entier) {
    // Procédure qui demande d'entrer un entier.
    int res;
    do {
        printf("\nEntrez un entier (%d à %d) : ", min, max);
        res = scanf("%d", entier);
        if (res != 1) {
            fprintf(stderr, "Erreur : entrée invalide.\n");
            vider_buffer();
        } else if (*entier < min || *entier > max) {
            printf("Erreur : valeur hors intervalle.\n");
        }
    } while (res != 1 || *entier < min || *entier > max);
}

void quitter(graphe* g, FILE* file, char* lignes[], int nb_lignes){
    // Procédure qui libère toute la mémoire allouée et ferme le fichier de configuration avant de stopper le programme.
    printf("\n===== Libération de la mémoire =====\n");
    desinit_graphe(g);
    close_file(file, lignes, nb_lignes);
}

void envoyer_trame(graphe* g){
    // Procédure qui envoie une trame (ping) entre 2 machines choisies par l'utilisateur.
    printf("\n===== Envoi de trame =====\n");
    int index[g->ordre];
    int nb = 0;
    for(int i =0;i<g->ordre;i++){
        if (g->sommets[i]->machine->type == STATION){
            printf("[%d] %s \n",nb,to_string_mac(&(g->sommets[i]->machine->addr_mac),(char[50]){0}));
            index[nb] = i;
            nb++;
        }
    }
    int index_machine1 = -1;
    int index_machine2 = -1;

    printf("Choisissez la machine qui enverra la trame : ");
    demande_entier(0, nb, &index_machine1);

    printf("Choisissez la destination de la trame : ");
    demande_entier(0, nb, &index_machine2);

    printf("La machine %d enverra une trame à la machine %d\n", index_machine1, index_machine2);

    printf("\n===== Envoi d'une trame entre machines =====\n");
    getchar();
    
    machine* station1 = g->sommets[index[index_machine1]]->machine;
    machine* station2 = g->sommets[index[index_machine2]]->machine;
    
    trame test_trame;
    size_t pong = 0;
    char msg_buffer[128];
    
    init_trame(&test_trame, station1->addr_mac, station2->addr_mac, &pong, PING);
    printf("De: %s\n", to_string_mac(&station1->addr_mac, (char[20]){0}));
    printf("À: %s\n", to_string_mac(&station2->addr_mac, (char[20]){0}));
    printf("Message: %s\n", to_string_message(&test_trame, msg_buffer));
    
    send_trame(station1, &test_trame, station1->interface);
    desinit_trame(&test_trame);
}

void afficher_table(graphe* g){
    // Procédure qui affiche la table de commmutation d'un switch choisi par l'utilisateur.
    printf("\n===== Affichage de la Table de commutation =====\n");
    if (g->ordre > 0) {
        int index[g->ordre];
        int nb = 0;
        for(int i =0;i<g->ordre;i++){
            if (g->sommets[i]->machine->type == SWITCH){
                printf("[%d] %s \n",nb,to_string_mac(&(g->sommets[i]->machine->addr_mac),(char[50]){0}));
                index[nb] = i;
                nb++;
            }
        }
        int index_switch = -1;
        printf("Choisissez le switch: ");
        demande_entier(0, nb, &index_switch);
        print_switch_table((bridge*)g->sommets[index[index_switch]]->machine->machine);
    }
}