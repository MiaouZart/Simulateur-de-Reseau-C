#include "adress.h"
#include <stdio.h>
#include <stddef.h>
#include <string.h>


bool mac_equals(mac mac1, mac mac2){
    // Fonction qui retourne vrai si les deux addresses mac en paramètre sont identiques, retourne faux sinon.
    return mac1 == mac2;
}

bool is_mac_inferior_to(mac mac1, mac mac2){
    // Fonction qui retourne vrai si l'adresse mac 1 est strictement inférieure à l'addresse mac 2, retourne faux sinon.
    return mac1 < mac2;
}

char* to_string_mac(mac const* mac, char * buffer) {
    // Fonction qui convertit une addresse mac en chaine de caractères puis la renvoie.
    buffer[0] = '\0'; 
    for(size_t i = 6; i > 0; i--) {
        size_t byte_index = i - 1;
        uint8_t byte = ((uint8_t*)mac)[byte_index];
        if (i < 6) {
            sprintf(buffer + strlen(buffer), ":");
        }
        sprintf(buffer + strlen(buffer), "%02X", byte);
    }
    return buffer;
}

void afficher_mac(mac const* mac){
    // Procédure qui affiche une addresse mac à l'écran.
    char buffer[18];
    printf("MAC: %s\n", to_string_mac(mac, buffer));
}

mac string_to_mac(char* string) {
    // Fonction qui convertit une chaine de caractères en addresse mac et la renvoie.
    if (string == NULL) {
        return 0;
    }
    
    unsigned int o1, o2, o3, o4, o5, o6;
    if (sscanf(string, "%02x:%02x:%02x:%02x:%02x:%02x", 
               &o1, &o2, &o3, &o4, &o5, &o6) != 6) {
        return 0;
    }
    
    return ((mac)o1 << 40) | ((mac)o2 << 32) | ((mac)o3 << 24) | 
           ((mac)o4 << 16) | ((mac)o5 << 8) | (mac)o6;
}




ipv4 build_ipv4(uint8_t o1, uint8_t o2, uint8_t o3, uint8_t o4){
    // Fonction qui construit puis retorune une addresse ipv4 à partir de 4 octets.
    return (o1<<24) + (o2<<16) + (o3<<8) + o4; 
}

bool compare_ipv4( ipv4 const* ip1, ipv4 const* ip2){
    // Fonction retourne vrai si les deux addresses ipv4 en paramètre sont identiques, retourne faux sinon.
    return *ip1 == *ip2;
}

char* to_string_ipv4(ipv4 const* ip, char* buffer){
    // Fonction qui convertit une addresse ipv4 en chaine de caractères puis la renvoie.
    for(size_t i=4; i>0; i--){
        size_t nb_bits = (i-1)*8;
        uint32_t masque = 255 << nb_bits;
        uint8_t x = (*ip&masque) >> nb_bits;
        char* buff_temp = buffer;
        if (i==4){
            sprintf(buffer, "%u", x);
        }
        else{
            sprintf(buff_temp + strlen(buffer), ".%u", x);
        }
    }
    return buffer;
}

void afficher_ipv4(ipv4 const *ip){
    // Procédure qui affiche une addresse ipv4 à l'écran.
    char buffer[16];
    printf("IPV4: %s\n", to_string_ipv4(ip, buffer));
}

ipv4 string_to_ipv4(char* string) {
    // Fonction qui convertit une chaine de caractères en addresse ipv4 et la renvoie.
    if (string == NULL) {
        return 0;
    }
    
    unsigned int o1, o2, o3, o4;
    if (sscanf(string, "%u.%u.%u.%u", &o1, &o2, &o3, &o4) != 4) {
        return 0;
    }
    
    if (o1 > 255 || o2 > 255 || o3 > 255 || o4 > 255) {
        return 0;
    }
    
    return (o1 << 24) | (o2 << 16) | (o3 << 8) | o4;
}