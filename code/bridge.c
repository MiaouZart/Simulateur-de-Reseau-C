#include "bridge.h"
#include "trame.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void init_bridge(bridge* bd, size_t nb_ports, size_t priorite, mac addr_mac) {
    // Procédure qui initialise un switch (bridge).
    bd->nb_ports = nb_ports;
    bd->priorite = priorite;
    bd->addr_mac = addr_mac;

    bd->bpdu = (bpdu*)malloc(sizeof(bpdu));
    if (!bd->bpdu) return;
    init_bpdu(bd->bpdu, priorite, addr_mac);

    bd->table = (com*)malloc(sizeof(com) * nb_ports);
    bd->table_length = 0;
    bd->max_table_length = nb_ports;

    bd->ports = (port**)calloc(nb_ports, sizeof(port*));
    bd->root_index = -1;

    printf("Switch: MAC=%s, Ports=%zu, Priorite=%zu\n", 
           to_string_mac(&addr_mac, (char[20]){0}), nb_ports, priorite);
}


void desinit_bridge(bridge* bd) {
    // Procédure qui libère la mémoire allouée par le switch.
    if (!bd) return;

    for (size_t i = 0; i < bd->nb_ports; i++) {
        if (bd->ports[i]) {
            if (bd->ports[i]->port) {
                desinit_inter(bd->ports[i]->port);
                free(bd->ports[i]->port);
            }

            if (bd->ports[i]->best_received && bd->ports[i]->best_received != bd->bpdu) {
                desinit_bpdu(bd->ports[i]->best_received);
                free(bd->ports[i]->best_received);
            }

            free(bd->ports[i]);
        }
    }

    free(bd->ports);
    desinit_bpdu(bd->bpdu);
    free(bd->bpdu);
    free(bd->table);

    bd->bpdu = NULL;
    bd->ports = NULL;
    bd->table = NULL;
    bd->nb_ports = 0;
    bd->root_index = -1;
    bd->table_length = 0;
    bd->max_table_length = 0;
}


int check_if_in_com_table(bridge* bd, mac addr) {
    /* Fonction qui retourne l'index du switch ayant l'addresse mac 'addr' dans la table de commutation du switch 'bd', 
    retourne -1 si on ne trouve pas de correspondance.
    */
    if (!bd || !bd->table) return -1;

    for (size_t i = 0; i < bd->table_length; i++) {
        if (memcmp(&bd->table[i].addr_mac, &addr, sizeof(mac)) == 0) {
            return i;
        }
    }
    return -1;
}

void add_to_com_table(bridge* bd, mac addr, interface* inter) {
    // Procédure qui ajoute l'addresse mac dans la table de commutation du switch.
    if (!bd || !inter) return;
    if (check_if_in_com_table(bd, addr) != -1) return;

    if (bd->table_length == bd->max_table_length) {
        size_t new_size = bd->max_table_length * 2;
        com* new_table = (com*)realloc(bd->table, sizeof(com) * new_size);
        if (!new_table) return;

        bd->table = new_table;
        bd->max_table_length = new_size;
    }

    size_t interface_index = (size_t)-1;
    for (size_t i = 0; i < bd->nb_ports; i++) {
        if (bd->ports[i]->port == inter) {
            interface_index = i;
            break;
        }
    }

    if (interface_index == (size_t)-1) return;

    memcpy(&bd->table[bd->table_length].addr_mac, &addr, sizeof(mac));
    bd->table[bd->table_length].index_port = interface_index;
    bd->table_length++;
}

interface* bridge_get_free_interface(bridge* bd, machine* mach) {
    /* Fonction qui crée puis retourne une interface sur le premier port vide du switch que l'on trouve,
    retourne NULL si tous les ports sont occupés. */
    if (!bd) return NULL;

    for (size_t i = 0; i < bd->nb_ports; i++) {
        if (bd->ports[i] == NULL) {
            interface* new_inter = malloc(sizeof(interface));
            if (!new_inter) return NULL;

            init_interface(new_inter, mach);

            bd->ports[i] = malloc(sizeof(port));
            if (!bd->ports[i]) {
                free(new_inter);
                return NULL;
            }

            bd->ports[i]->port = new_inter;
            bd->ports[i]->type = DESIGNE;
            bd->ports[i]->type_changed = false;
            bd->ports[i]->best_received = malloc(sizeof(bpdu));
            if (bd->ports[i]->best_received)
                copy_bpdu(bd->bpdu, bd->ports[i]->best_received);

            return new_inter;
        }
    }

    return NULL;
}



void print_switch_table(const bridge* bd) {
    // Procédure qui affiche la table de commutation du switch à l'écran.
    if (!bd || !bd->table) {
        printf("Table de commutation vide ou bridge invalide.\n");
        return;
    }

    printf("┌───────────────────────────────────────────┐\n");
    printf("│       Table de commutation du bridge      │\n");
    printf("├─────────────────────┬─────────────────────┤\n");
    printf("│     Adresse MAC     │   Port d'interface  │\n");
    printf("├─────────────────────┼─────────────────────┤\n");

    if (bd->table_length == 0) {
        printf("│         Vide        │          -          │\n");
    } else {
        for (size_t i = 0; i < bd->table_length; i++) {
            printf("│  %s  ",to_string_mac(&bd->table[i].addr_mac, (char[20]){0}));
            printf("│ %10zu          │\n", bd->table[i].index_port);
        }
    }

    printf("└─────────────────────┴─────────────────────┘\n");
    printf("Total entrées: %zu / %zu\n", bd->table_length, bd->max_table_length);
}



int retrieve_port(bridge* bd, interface* inter) {
    // Fonction qui retourne l'index du port qui est associé à l'interface, retourne -1 si on ne trouve pas de correspondance.
    if (!bd || !inter) return -1;

    for (size_t i = 0; i < bd->nb_ports; i++) {
        if (bd->ports[i] && bd->ports[i]->port == inter) {
            return (int)i;
        }
    }

    return -1;
}



void process_trame(bridge *br, trame* tr, interface* input_port) {
    // Procédure qui gère le traitement d'une trame par le switch.
    int input_index = retrieve_port(br, input_port);
    if (input_index == -1) return;

    if (tr->type == PING &&br->ports[input_index]->type !=NONDESIGNE ) {
        int index = check_if_in_com_table(br, tr->dest);
        if (index != -1 ) {
            com* entry = &br->table[index];
            interface* out_inter = br->ports[entry->index_port]->port;
            if (out_inter != input_port) {
                trame reply;
                copy_trame(&reply, tr);
                send_data(out_inter, &reply);
                desinit_trame(&reply); 
            }
        } else {
            for (size_t i = 0; i < br->nb_ports; i++) {
                if (br->ports[i] ){
                if( br->ports[i]->port != input_port) {
                    
                if(br->ports[i]->type != NONDESIGNE) {
                    trame reply;
                    copy_trame(&reply, tr);
                    send_data(br->ports[i]->port, &reply);
                    desinit_trame(&reply); 
                }
            }}
        }
        }
    }
    else if (tr->type == BPDU) {
        bpdu* received = (bpdu*)tr->message;
        port* pt = br->ports[input_index];
        if (!pt) return;
        bpdu current_adjusted;
        copy_bpdu(br->bpdu, &current_adjusted);
        
        if (br->root_index != -1) {
            port* root_port = br->ports[br->root_index];
            current_adjusted.cost -= root_port->port->poids;
        }
        if (is_bpdu_better(received, &current_adjusted)) {
            desinit_bpdu(br->bpdu);
            copy_bpdu(received, br->bpdu);
            br->bpdu->cost += pt->port->poids;
            br->root_index = input_index;
        }
        if (pt->best_received) {
            desinit_bpdu(pt->best_received);
            copy_bpdu(received, pt->best_received);
        } else {
            pt->best_received = malloc(sizeof(bpdu));
            copy_bpdu(received, pt->best_received);
        }
         desinit_bpdu(&current_adjusted);
        recalculate_ports(br);
        desinit_trame(tr);
    }
}


void recalculate_ports(bridge* br) {
    // Procédure qui met à jour le statut des ports du switch.
    if (!br || !br->bpdu) return;

    for (size_t i = 0; i < br->nb_ports; i++) {
        port* pt = br->ports[i];
        if (!pt || !pt->port) continue;

        if ((int)i == br->root_index) {
            if (pt->type != RACINE) {
                pt->type = RACINE;
                pt->type_changed = true;
            }
            continue;
        }

        bpdu simulated;
        copy_bpdu(br->bpdu, &simulated);
        simulated.cost += pt->port->poids;
        simulated.transmitting_id = br->addr_mac;

        if (pt->best_received) {
            bpdu adjusted_received = *(pt->best_received);
            adjusted_received.cost += pt->port->poids;

            if (is_bpdu_better(&adjusted_received, &simulated)) {
                if (pt->type != NONDESIGNE) {
                    pt->type = NONDESIGNE;
                    pt->type_changed = true;
                    printf("je change \n");
                }
            } else {
                if (pt->type != DESIGNE) {
                    pt->type = DESIGNE;
                    pt->type_changed = true;
                    printf("je change \n");
                }
            }
        } else {
            if (pt->type != DESIGNE) {
                pt->type = DESIGNE;
            }
        }
        desinit_bpdu(&simulated);
    }
}


void send_bdpu(bridge* br) {
    // Procédure qui gère l'envoie d'un bpdu par tous les ports valides du switch.
    if (!br || !br->ports || !br->bpdu) return;

    for (size_t i = 0; i < br->nb_ports; i++) {
        port* pt = br->ports[i];
        if (!pt || pt->type == NONDESIGNE || !pt->port) continue;

        bpdu out_bpdu;
        copy_bpdu(br->bpdu, &out_bpdu);
        out_bpdu.transmitting_id = br->addr_mac;
        trame tr;
        init_trame(&tr, br->addr_mac,  br->addr_mac, &out_bpdu, BPDU);
        send_data(pt->port, &tr);
        desinit_trame(&tr);
        desinit_bpdu(&out_bpdu);
    }
}