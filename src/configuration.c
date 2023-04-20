/*
Grupo: 35
David Dantas 56331
Su Lishun 56375
Miguel Seabra 56344
*/
#include <configuration.h>

void get_configurations(char* config_filename, struct config *result)
{
    FILE* fc = fopen(config_filename, "r");
    if (fc == NULL){
        perror(config_filename);
        exit(-1);
    }
    
    char * line = NULL;
    char * line2 = NULL;
    char * line3 = NULL;
    size_t len = 0;

    get_value(&line, &len, fc);
    result->data.max_ops = to_int(line);
    get_value(&line, &len, fc);
    result->data.buffers_size = to_int(line);
    get_value(&line, &len, fc);
    result->data.n_restaurants = to_int(line);
    get_value(&line, &len, fc);
    result->data.n_drivers = to_int(line);
    get_value(&line, &len, fc);
    result->data.n_clients = to_int(line);
    get_value(&line, &len, fc);   
    result->log_filename = line;
    get_value(&line2, &len, fc);
    result->statistics_filename = line2;
    get_value(&line3, &len, fc);
    result->alarm_time = to_int(line3);

    fclose(fc);

}


void get_value(char** line, size_t* len, FILE* fc)
{
    if(getline(line, len, fc) == -1){
        printf("erro ao ler ficheiro de config\n");
        printf("Ficheiro de configuration errado, exemplo de ficheiro:\n10\n10\n1\n1\n1\nlog.txt\nstatic.txt\n100\n\n");

        exit(-1);
    }
}

int to_int(char* str)
{
    int res = atoi(str);

    return res;
}

