/*
Grupo: 35
David Dantas 56331
Su Lishun 56375
Miguel Seabra 56344
*/
#include <stdio.h>
#include <memory.h>
#include <metime.h>
#include <main.h>

/*
Imprime para um FILE as estatísticas de cada restaurante, condutor e cliente
*/
void out_statistics(struct main_data *data, char* filename);

/*
Imprime as estatísticas de um processo no seguinte formato "   process_name: process_id process_action stats requested!"
*/
void print_process_statistics(FILE *file, char *process_name, char *process_action, int *stats, int max_ops);

/*
Para cada request imprime as suas estatísticas para FILE
*/
void print_requests_statistics(FILE *file, struct operation *results, int max_ops);