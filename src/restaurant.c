/*
Grupo: 35
David Dantas 56331
Su Lishun 56375
Miguel Seabra 56344
*/
#include <restaurant.h>
#include <stdio.h>
#include <metime.h>
#include <unistd.h>
#include <sys/types.h>

int execute_restaurant(int rest_id, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems)
{
    int x = 0;
    int *op_counter = &x;
    
    struct operation op; //memoria em caso de ser preciso afetar id com -1
    op.id = -1;
    struct operation* op_ptr = &op;
    while (*data->terminate != 1)
    {
        restaurant_receive_operation(op_ptr, rest_id, buffers, data,sems);
        if (op_ptr->id != -1)
        {
            printf("Restaurante recebeu o pedido!\n");
            restaurant_process_operation(op_ptr, rest_id, data, op_counter, sems);

            restaurant_forward_operation(op_ptr, buffers, data, sems);
        }
        op_ptr = &op;
    }

    return *op_counter;
}


void restaurant_receive_operation(struct operation* op, int rest_id, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems)
{
    if(*data->terminate){
        return;
    }  
    consume_begin(sems->main_rest);
    read_main_rest_buffer(buffers->main_rest, rest_id, data->buffers_size, op);
    if (op->id == -1)
    {
        //Como não foi consumido nada o valor de sems->main_rest->full tem um valor a menos
        //assim temos de fazer post em full e no mutex
        produce_end(sems->main_rest);
    }else
    {
        consume_end(sems->main_rest);
    }

}



void restaurant_process_operation(struct operation* op, int rest_id, struct main_data* data, int* counter, struct semaphores* sems)
{

    op->receiving_rest = rest_id;
    op->status = 'R';
    *counter = *counter + 1;
    clock_time(op);
    semaphore_mutex_lock(sems->results_mutex);
    data->results[op->id] = *op;
    semaphore_mutex_unlock(sems->results_mutex);
}



void restaurant_forward_operation(struct operation* op, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems)
{
    produce_begin(sems->rest_driv);
    write_rest_driver_buffer(buffers->rest_driv, data->buffers_size, op);
    produce_end(sems->rest_driv);
}