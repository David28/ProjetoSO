/*
Grupo: 35
David Dantas 56331
Su Lishun 56375
Miguel Seabra 56344
*/

#include <client.h>
#include <stdio.h>
#include <metime.h>

int execute_client(int client_id, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems)
{
    int count = 0;
    int *op_counter = &count;
    struct operation op; //memoria em caso de ser preciso afetar id com -1
    struct operation* op_ptr = &op;
    while(*data->terminate != 1)
    {
        
        client_get_operation(op_ptr, client_id, buffers, data, sems);
        if(op_ptr->id != -1){
            printf("Cliente recebeu o pedido!\n");
            client_process_operation(op_ptr, client_id, data, op_counter, sems);
        }
        op_ptr = &op;
    }
    return *op_counter;
}



void client_get_operation(struct operation* op, int client_id, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems)
{
    if(*data->terminate)
        return;
    consume_begin(sems->driv_cli);
    read_driver_client_buffer(buffers->driv_cli, client_id, data->buffers_size, op);
    if (op->id == -1)
    {
        //Como não foi consumido nada o valor de sems->driv_cli->full tem um valor a menos
        //assim temos de fazer post em full e no mutex
        produce_end(sems->driv_cli);
    }else
    {
        consume_end(sems->driv_cli);
    }
}



void client_process_operation(struct operation* op, int client_id, struct main_data* data, int* counter, struct semaphores* sems)
{
    op->receiving_client = client_id;
    op->status = 'C';
    *counter = *counter + 1;
    clock_time(op);
    semaphore_mutex_lock(sems->results_mutex);
    data->results[op->id] = *op;
    semaphore_mutex_unlock(sems->results_mutex);
}