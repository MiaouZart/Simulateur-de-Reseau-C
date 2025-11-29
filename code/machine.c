#include <machine.h>
#include "bridge.h"
#include <stdlib.h>
#include <stdio.h>

void init_machine(machine* machine, void* machine_pointer, type type, mac addr) {
    // Procédure qui initialise une machine.
    machine->machine = machine_pointer;
    machine->type = type;
    machine->addr_mac = addr;
    if (type == SWITCH) {
        char macprintbuffer[1024];
        bridge *br = (bridge *)(machine->machine);
        printf("Switch: MAC=%s, Ports=%ld, Priorite=%ld\n",
               to_string_mac(&addr, macprintbuffer),
               br->nb_ports,
               br->priorite);
        machine->interface = malloc(sizeof(interface));
    } else {
        char bufferMAC[512];
        char bufferIP[256];
        station *sta = (station *)(machine->machine);
        printf("Station: MAC=%s, IP=%s\n",
               to_string_mac(&addr, bufferMAC),
               to_string_ipv4(&sta->addr_ip, bufferIP));
        machine->interface = malloc(sizeof(interface));
        if (!machine->interface) {
            exit(EXIT_FAILURE);
        }
    }
    init_interface(machine->interface, machine);
}

void desinit_machine(machine* machine) {
    // Procédure qui libère la mémoire allouée par une machine.
    char macprintbuffer[1024];
    printf(" desinit de : %s\n", to_string_mac(&machine->addr_mac, macprintbuffer));

    if(machine->type == SWITCH) {
        desinit_bridge((bridge*)machine->machine);
    } else {
        desinit_station((station*)machine->machine);
    }

    machine->addr_mac = -1;

    desinit_inter(machine->interface);
    free(machine->interface);
    machine->interface = NULL;

    free(machine->machine);
}

void send_trame(machine* sender, trame *tr, interface* input_port) {
    // Procédure qui gère l'envoie d'une trame.
    if (sender->type == STATION) {
        trame reply;
        copy_trame(&reply, tr); 
        send_data(sender->interface, &reply);
        desinit_trame(&reply);
    } else if (sender->type == SWITCH) {
        bridge *br = (bridge*)sender->machine;
        process_trame(br,tr, input_port);
       
    }

}

void receive_trame(machine* receiver, trame* tr, interface* input_port) {
    // Procédure qui gère la réception d'une trame.
    char mac_buffer_self[20]; 
    char mac_buffer_src[20];
    char mac_buffer_dest[20];
    char trame_buffer[64];

    if (receiver->type == STATION) {
        if (is_it_for_me_question_mark(receiver, tr)) {
            printf("Station %s: reçu trame de %s:\n",
                   to_string_mac(&receiver->addr_mac, mac_buffer_self),
                   to_string_mac(&tr->source, mac_buffer_src));
            printf("    Destination = %s\n", to_string_mac(&tr->dest, mac_buffer_dest));
            printf("    Message = %s\n\n", to_string_message(tr, trame_buffer));

            if (tr->type == PING && *((ping*)tr->message) == 0) {
                trame reply;
                size_t message = 1;
                init_trame(&reply, receiver->addr_mac, tr->source, &message, PING);
                send_trame(receiver, &reply, receiver->interface);
                desinit_trame(&reply); 
            }
        }
    } else if (receiver->type == SWITCH) {
        bridge *br = (bridge*)receiver->machine;
        printf("Switch %s: reçu trame de %s vers %s\n",
               to_string_mac(&receiver->addr_mac, mac_buffer_self),
               to_string_mac(&tr->source, mac_buffer_src),
               to_string_mac(&tr->dest, mac_buffer_dest));

        if (input_port) {
            add_to_com_table(br, tr->source, input_port);
        } else {
            printf(" Avertissement: port d'entrée inconnu\n");
        }

        if (!mac_equals(receiver->addr_mac, tr->dest)) { 
            send_trame(receiver, tr, input_port); 
        }
    }

}

bool is_it_for_me_question_mark(machine *mach, trame* t) {
    // Fonction qui retourne vrai si la destination de la trame est la machine, retourne faux sinon.
    return mac_equals(mach->addr_mac, t->dest);
}

void connect_two_machine(machine* machine1, machine* machine2, size_t poids) {
    // Procédure qui connecte 2 machines en liant leurs interfaces.
    interface *intf1 = NULL, *intf2 = NULL;
    assign_interface(&machine1, &intf1);
    assign_interface(&machine2, &intf2);

    if (intf1 && intf2) {
        connect_two_interface(intf1, intf2, poids);
    } else {
        fprintf(stderr, "Erreur: interface non disponible\n");
    }
}

void assign_interface(machine** machine, interface** interface) {
    // Procédure qui assigne une interface à la machine.
    if ((*machine)->type == SWITCH) {
        bridge* br = (bridge*)(*machine)->machine;
        *interface = bridge_get_free_interface(br, *machine);
    } else {
        *interface = (*machine)->interface;
    }
}
