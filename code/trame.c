#include <trame.h>
#include <stdio.h>
#include <stdlib.h>

void init_trame(trame * tra, mac source, mac dest, void* message, trame_type type){
    // Procédure qui initialise une trame.
    tra->dest = dest;
    tra->source = source;
    tra->type = type;

    switch(type){
        case PING:
            tra->message = malloc(sizeof(ping));
            if (tra->message == NULL) exit(EXIT_FAILURE);
            *((ping*)tra->message) = *((ping*)message);
            break;
        case BPDU:
            tra->message = malloc(sizeof(bpdu));
            if (tra->message == NULL) exit(EXIT_FAILURE);
            *((bpdu*)tra->message) = *((bpdu*)message);
            break;
    }
}


char* to_string_message(const trame* tra, char* buffer) {
    // Fonction qui convertit le contenu de la trame en chaine de caractères et la renvoie.
    switch (tra->type) {
        case PING: {
            sprintf(buffer, "Ping: %zu", *(ping*)tra->message);
            break;
        }
        case BPDU: {
            char str_bpdu[64];
            to_string_bpdu(((bpdu*)tra->message), str_bpdu);
            sprintf(buffer, "BPDU: %s", str_bpdu);
            break;
        }
    }
    return buffer;
}

void desinit_trame(trame* tra) {
    // Procédure qui libère la mémoire allouée par la trame.
    if (!tra) return;
    if (tra->message) {
        free(tra->message); 
        tra->message = NULL;
    }
}

void copy_trame(trame* dest, const trame* src) {
    // Procédure qui effectue une copie profonde de la trame source dans la trame destination.
    init_trame(dest, src->source, src->dest, src->message, src->type);
}
