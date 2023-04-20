/*
Grupo: 35
David Dantas 56331
Su Lishun 56375
Miguel Seabra 56344
*/
#include <process.h>
#include <client.h>
#include <restaurant.h>
#include <driver.h>
#include <stdlib.h>
#include <wait.h>
#include <unistd.h>
#include <mesignal.h>

int launch_restaurant(int restaurant_id, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems)
{
    pid_t pid = fork();
    int op_counter;
    
    if(pid == 0)
    {
        ignoreCTRLC();
        op_counter = execute_restaurant(restaurant_id, buffers, data, sems);
        exit(op_counter);
    }else
    {
        return pid;
    }
}


int launch_driver(int driver_id, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems)
{
    int pid = fork();
    int op_counter;
    if(pid == 0)
    {
        ignoreCTRLC();
        op_counter = execute_driver(driver_id, buffers, data, sems);
        exit(op_counter);
    }else
    {
        return pid;
    }
}


int launch_client(int client_id, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems)
{
    int pid = fork();
    int op_counter;
    if(pid == 0)
    {
        ignoreCTRLC();
        op_counter = execute_client(client_id, buffers, data, sems);
        exit(op_counter);
    }else
    {
        return pid;
    }
}

int wait_process(int process_id)
{
    int code;
    waitpid(process_id, &code, 0);
    if(WIFEXITED(code))
        return WEXITSTATUS(code);
    else
        return -1;    
}