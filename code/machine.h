#pragma once
#include "station.h"
#include "interface.h"  

typedef enum {
    STATION = 1,
    SWITCH = 2,
} type;

typedef struct machine {
    void* machine;
    interface* interface;
    type type;
    mac addr_mac;
} machine;

void init_machine(machine* machine, void* machine_pointer, type type, mac addr);
void desinit_machine(machine* machine);

void send_trame(machine* sender, trame *tr, interface* input_port);
void receive_trame(machine* receiver, trame* tr, interface* input_port);
bool is_it_for_me_question_mark(machine *mach, trame* t);
void connect_two_machine(machine* machine1, machine* machine2, size_t poids);
void assign_interface(machine** machine, interface** interface);
void add_interface(machine* machine , interface* inter);
