/*
Grupo: 35
David Dantas 56331
Su Lishun 56375
Miguel Seabra 56344
*/
#include <log.h>

FILE* create_log_file(char* filename)
{
    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
        perror(filename);
    return fp;
}

void log_instruction(FILE* file, char* instruction)
{
    log_instruction_s(file, instruction, -1);
}

void log_instruction_s(FILE* file, char* instruction, int n){
    char str[30];
    char* now;
    if (n == -1)
    {
        now = format_now();
        sprintf(str, "%s%s\n",now,instruction);
    }else{
        now = format_now();
       sprintf(str, "%s%s%d\n",now, instruction, n); 
    }
    destroy_dynamic_memory(now);
    fputs(str, file);
}


void close_file(FILE* file)
{
    fclose(file);
}