/*
Grupo: 35
David Dantas 56331
Su Lishun 56375
Miguel Seabra 56344
*/
#include <driver.h>
#include <stdio.h>
#include <metime.h>

int execute_driver(int driver_id, struct communication_buffers *buffers, struct main_data *data,  struct semaphores* sems)
{
    int count = 0;
    int *op_counter = &count;
    struct operation op; 
    op.id = -1;
    while (*data->terminate != 1)
    {
        //memoria em caso de ser preciso afetar id com -1
        struct operation* op_ptr = &op;
        
        driver_receive_operation(op_ptr, buffers, data, sems);

   
    
        
        //leu algo que ainda nao tenha sido lido (garante menos problemas de sincronização)
        if (*data->terminate != 1 && op_ptr->id != -1 && data->results[op_ptr->id].status == 'R')
        {
            puts("Motorista recebeu o pedido!");
            driver_process_operation(op_ptr, driver_id, data, op_counter, sems);
            driver_send_answer(op_ptr, buffers, data, sems);
        }
    }

    return *op_counter;
}

void driver_receive_operation(struct operation *op, struct communication_buffers *buffers, struct main_data *data, struct semaphores* sems)
{
    if(*data->terminate)
        return;
    consume_begin(sems->rest_driv);
    read_rest_driver_buffer(buffers->rest_driv, data->buffers_size, op);
    consume_end(sems->rest_driv);
}


void driver_process_operation(struct operation *op, int driver_id, struct main_data *data, int *counter, struct semaphores* sems)
{
    op->receiving_driver = driver_id;
    op->status = 'D';
    *counter = *counter + 1;
    clock_time(op);

    semaphore_mutex_lock(sems->results_mutex);
    data->results[op->id] = *op;
    semaphore_mutex_unlock(sems->results_mutex);
}

void driver_send_answer(struct operation *op, struct communication_buffers *buffers, struct main_data *data, struct semaphores* sems)
{
    produce_begin(sems->driv_cli);
    write_driver_client_buffer(buffers->driv_cli, data->buffers_size, op);
    produce_end(sems->driv_cli);
}