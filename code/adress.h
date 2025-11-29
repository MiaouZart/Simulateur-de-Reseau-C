#pragma once
#include <stdint.h> 
#include <stdbool.h>


typedef uint64_t mac;
bool mac_equals(mac mac1, mac mac2);
bool is_mac_inferior_to(mac mac1, mac mac2);
char* to_string_mac(mac const * mac,char * buffer);
void afficher_mac( mac const* mac);
mac string_to_mac(char* string);


typedef uint32_t ipv4;
ipv4 build_ipv4(uint8_t o1, uint8_t o2, uint8_t o3, uint8_t o4);
bool compare_ipv4( ipv4 const* ip1, ipv4 const* ip2);
char* to_string_ipv4(ipv4 const * ip,char * buffer);
void afficher_ipv4(ipv4 const  *ip);
ipv4 string_to_ipv4(char* string);


