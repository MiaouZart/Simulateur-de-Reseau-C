#pragma once
#include <stddef.h>

typedef struct interface {
    struct machine* machine;           
    struct interface* connected_to;
    size_t poids;
} interface;

void init_interface(interface *inter, struct machine* machine);
void desinit_inter(interface* inter);

void connect_two_interface(interface* a, interface* b, size_t poids);
void send_data(interface* sender, void* data);
void receive_data(interface *receiver, void* data);
