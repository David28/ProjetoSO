/*
Grupo: 35
David Dantas 56331
Su Lishun 56375
Miguel Seabra 56344
*/
#include <stdio.h>
#include <metime.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

/*
Cria um ficheiro log de nome filename, e devolve o ponteiro
*/
FILE* create_log_file(char* filename);
/*
Regista uma nova instrução junto com o seu tempo
*/
void log_instruction(FILE* file, char* instruction);

void log_instruction_s(FILE* file, char* instruction, int n);

/*
Fecha o ficheiro log
*/
void close_file(FILE* file);