#include "parser.h"                                                             
#include <string.h>                                                             
#include <stdlib.h>  

#include <station.h>
#include <bridge.h>
#include <interface.h>
                                                                                
int MAX_LIGNE_SIZE = 1024;                                                           
int MAX_LIGNE = 1024;                                                          
                                                                                
FILE* open_file(char * nameFile){     
    // Fonction qui retourne le fichier choisi ouvert en mode lecture.                                    
    return fopen(nameFile,"r");                                                 
}                                                                    
                                                                                
int read_file(FILE* file, char* lignes[]) { 
    /* Fonction qui lit le fichier tout en remplissant le tableau de chaines de caractères représentant le texte du fichier,
    puis retourne le nombre de lignes. */                                    
    if (file == NULL) {                                                         
        return -1;                                                              
    }                                                                           
                                                                                
    char buffer[MAX_LIGNE_SIZE];                                                     
    int count = 0;                                                              
                                                                                
    while (fgets(buffer, MAX_LIGNE_SIZE, file) != NULL && count < MAX_LIGNE) {      
        buffer[strcspn(buffer, "\n")] = '\0';                                   
                                                                                
        lignes[count] = malloc(strlen(buffer) + 1);                             
        if (lignes[count] == NULL) {                                            
            return -1;                                                          
        }                                                                       
        strcpy(lignes[count], buffer);                                          
        count++;                                                                
    }                                                                  
                                                                                
    return count;                                                               
}     



int close_file(FILE* file, char* lignes[], int nb_lignes) {
    // Fonction qui ferme le fichier et libère la mémoire allouée, puis renvoie 0 si tout s'est bien passé.
    if (file != NULL) {
        fclose(file);
    }
    
    if (lignes != NULL) {
        for (int i = 0; i < nb_lignes; i++) {
            free(lignes[i]); 

        }
    }
    
    return 0;
}                                                                    
                                                                                
void parse_file(graphe *g, char* ligne[]) {
    // Procédure qui traduit le contenu du fichier et remplit le graphe.
    size_t nb_machine = 0;
    size_t nb_connexion = 0;
    sscanf(ligne[0], "%zu %zu", &nb_machine, &nb_connexion);
    printf("Nombre de machines : %zu \nNombre de connexions : %zu\n", nb_machine, nb_connexion);
    init_graphe(g,nb_machine);
    for(size_t i = 1; i <= nb_machine; i++) {
        size_t type;
        char buffer[MAX_LIGNE_SIZE];
        sscanf(ligne[i], "%zu;%[^\n]", &type, buffer);
        machine * mach = malloc(sizeof(machine));
        
        if(type == STATION) {
            char macAddr[18];  
            char ipAddr[16];  
            sscanf(buffer, "%[^;];%15s", macAddr, ipAddr);
            station* sta = malloc(sizeof(station)) ;
            mac mac = string_to_mac(macAddr);
            ipv4 ip = string_to_ipv4(ipAddr);
            init_station(sta,ip);
            init_machine(mach,(void*)sta,STATION,mac);
            g->nbStation++;
        }
        else if(type == SWITCH) {
            char macAddr[18];
            size_t nb_port, priorite;
            sscanf(buffer, "%[^;];%zu;%zu", macAddr, &nb_port, &priorite);
            bridge  * bd = malloc(sizeof(bridge));
            mac mac = string_to_mac(macAddr);
            init_bridge(bd,nb_port,priorite, mac);
            init_machine(mach,(void*)bd,SWITCH,mac);
            g->nbSwitch ++;
        }

        sommet * som = malloc(sizeof(sommet));
        init_sommet(som,i-1,mach);
        add_sommet(g,som,i-1);
    }


    for (size_t i = nb_machine + 1; i < nb_machine + 1 + nb_connexion; i++) {
        size_t poids, indexmachine1, indexmachine2;
        sscanf(ligne[i], "%zu;%zu;%zu", &indexmachine1, &indexmachine2,&poids);
        //printf("%ld , %ld , %ld \n" , poids ,indexmachine1,indexmachine2);
        connect_two_machine(g->sommets[indexmachine1]->machine, g->sommets[indexmachine2]->machine, poids);
    }

}                                                           
                                                                                
                                                                                




