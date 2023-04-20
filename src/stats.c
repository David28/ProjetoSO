/*
Grupo: 35
David Dantas 56331
Su Lishun 56375
Miguel Seabra 56344
*/
#include <stats.h>


void out_statistics(struct main_data *data, char* filename)
{
    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
        perror(filename);
    fprintf(fp, "Process statistics:\n");
    print_process_statistics(fp, "Restaurant", "prepared", data->restaurant_stats, data->n_restaurants);
    print_process_statistics(fp, "Driver", "delivered" , data->driver_stats, data->n_drivers);
    print_process_statistics(fp, "Client", "received" , data->client_stats, data->n_clients);

    print_requests_statistics(fp, data->results, data->max_ops);
}

void print_process_statistics(FILE *file, char *process_name, char *process_action, int *stats, int max_ops)
{
    for (int i = 0; i < max_ops; i++)
    {
       fprintf(file, "    %s: %d %s %d requests!\n", process_name, i, process_action, stats[i]);
    }
}

void print_requests_statistics(FILE *file, struct operation *results, int max_ops)
{
    fprintf(file, "Request Statistics:\n");
    for (int i = 0; i < max_ops; i++)
    {
        struct operation op = results[i];
        if (op.status == '\0')
        {
            return; //verifica se op foi criado ou nao
        }
        if (op.status == 'C'){ //Request foi completado
            fprintf(file, "\n");
            char* str;  //ponteiro para a string com tempo formatado
            fprintf(file, "Request: %d\n", i);
            fprintf(file, "Status: %c\n", op.status);
            fprintf(file, "Restaurant id: %d\n", op.requested_rest);
            fprintf(file, "Driver id: %d\n", op.receiving_driver);
            fprintf(file, "Client id: %d\n", op.receiving_client);
            str = format_time_complex(op.start_time, 1);
            fprintf(file, "Created: %s\n", str);
            destroy_dynamic_memory(str);
            str = format_time_complex(op.rest_time, 1);
            fprintf(file, "Restaurant time: %s\n", str);
            destroy_dynamic_memory(str);
            str = format_time_complex(op.driver_time, 1);
            fprintf(file, "Driver time: %s\n", str);
            destroy_dynamic_memory(str);
            str = format_time_complex(op.client_end_time, 1);
            fprintf(file, "Client time (end): %s\n", str);
            destroy_dynamic_memory(str);
            struct timespec total_time;
            total_time.tv_sec = op.client_end_time.tv_sec - op.start_time.tv_sec; 
            total_time.tv_nsec = op.client_end_time.tv_nsec/1000000 - op.start_time.tv_nsec/1000000; 
            str = format_time_complex(total_time, 2);
            fprintf(file, "Total time: %s\n", str);
            destroy_dynamic_memory(str);
        }
        
    }
}