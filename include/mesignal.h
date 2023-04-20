/*
Grupo: 35
David Dantas 56331
Su Lishun 56375
Miguel Seabra 56344
*/
#include <main.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>
#include <log.h>
// Variaveis globais partilhadas
extern struct main_data *g_data;
extern struct communication_buffers *g_buffers;
extern struct semaphores *g_sems;
extern FILE* log_file;
extern int g_terminate;


/*
    Cria um novo sinal através de sigaction
*/
void create_signal(int signum, void *function);

/*
    Cria um timer que chama a função print_status cada vez que passa alarm_time
*/
void create_alarm(int alarm_time);

/*
    Prepara um sinal para a acão ctrl+c, e define as variáveis globais
*/
void create_stop(struct main_data* data, struct communication_buffers* buffers, struct semaphores* sems);

/*
Termina a execução do magnaeats de forma correta
*/
void ctrlC();

/*
Ignora ctrlC, usado nos processos filhos
*/
void ignoreCTRLC();

/*
Imprime o status de todas as operações até agora recebidas
*/
void print_status();