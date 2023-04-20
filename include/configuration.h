/*
Grupo: 35
David Dantas 56331
Su Lishun 56375
Miguel Seabra 56344
*/
#include <stdio.h>
#include <main.h>
#include <stdlib.h>


//estrutura para todos os dados recebidos na configuração
struct config 
{
    struct main_data data; //data principal
    char* log_filename;
    char* statistics_filename;
    int alarm_time;
};

/*
Lê o ficheiro de configuração e devolve todos os dados lidos
*/
void get_configurations(char* config_filename, struct config *result);

/*
Lê uma nova linha e coloca em line
*/
void get_value(char** line, size_t* len, FILE* fc);

/*
Converte uma string em int, e devolve -1 caso haja erro de leitura
*/
int to_int(char* str);

