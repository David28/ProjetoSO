/*
Grupo: 35
David Dantas 56331
Su Lishun 56375
Miguel Seabra 56344
*/
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <unistd.h>


void* create_shared_memory(char* name, int size)
{
    int shm_fd;
 
    void* ptr;
 
    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    if(shm_fd == -1)
    {
        perror("Erro ao criar memoria\n");
        exit(-1);
    }

    int trunc = ftruncate(shm_fd, size);
    if(trunc == -1)
    {
        perror("Erro ao criar memoria\n");
        exit(-1);
    }

    /* mapear com permissões de leitura e escrita */
    ptr = mmap(0,size, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);
    if(ptr == MAP_FAILED)
    {
        perror("Erro ao criar memoria\n");
        exit(-1);
    }
    bzero(ptr, size);

    return ptr;
}



void* create_dynamic_memory(int size)
{
    void *ptr = malloc(size);
    bzero(ptr, size);
    return ptr;
}


void destroy_shared_memory(char* name, void* ptr, int size)
{
    int ret;
    ret = munmap(ptr, size);
    if (ret == -1)
    {
        perror("Erro ao destruir a memoria partilhada.\n");
        exit(-1);
    }    


    ret = shm_unlink(name);
    if (ret == -1)
    {
        perror("Erro ao destruir a memoria partilhada.\n");
        exit(-1);
    }   
}



void destroy_dynamic_memory(void* ptr)
{
    free(ptr);
}

int getFirstEqualRest(struct rnd_access_buffer* buffer, int id, int size)
{
    for (int i = 0; i < size; i++)
    {
        if((buffer->ptrs[i] == 1) && (buffer->buffer[i].requested_rest == id)){
            return i;
        }
            
        
    }   
    return -1;
}

int getFirstEqualClient(struct rnd_access_buffer* buffer, int id, int size)
{
    for (int i = 0; i < size; i++)
    {
        if((buffer->ptrs[i] == 1) && (buffer->buffer[i].requesting_client == id))
            return i;
    }
    return -1;
}

int getAvailable(int* arr, int size)
{
    for (int i = 0; i < size; i++)
    {
        if(arr[i] == 0)
            return i;
    }
    return -1;
}

void write_main_rest_buffer(struct rnd_access_buffer* buffer, int buffer_size, struct operation* op)
{
    int in = getAvailable(buffer->ptrs, buffer_size);
    //Se existir posição livre
    if(in != -1)
    {
        buffer->buffer[in] = *op;
        buffer->ptrs[in] = 1;
    }

}



void write_rest_driver_buffer(struct circular_buffer* buffer, int buffer_size, struct operation* op)
{
    if((buffer->ptrs->in + 1)%buffer_size != buffer->ptrs->out)
    {
        buffer->buffer[buffer->ptrs->in] = *op;
        buffer->ptrs->in = (buffer->ptrs->in + 1)%buffer_size;       
    }

}



void write_driver_client_buffer(struct rnd_access_buffer* buffer, int buffer_size, struct operation* op)
{
    int in = getAvailable(buffer->ptrs,buffer_size);
    //Se existir posição livre
    if(in != -1)
    {
        buffer->buffer[in] = *op;
        buffer->ptrs[in] = 1;
    }
}



void read_main_rest_buffer(struct rnd_access_buffer* buffer, int rest_id, int buffer_size, struct operation* op)
{
    int out = getFirstEqualRest(buffer, rest_id, buffer_size);
    //Se existir posição ocupada
    if(out != -1)
    {
        *op = buffer->buffer[out];
        buffer->ptrs[out] = 0;
    }else{
        op->id = -1;
    }
}



void read_rest_driver_buffer(struct circular_buffer* buffer, int buffer_size, struct operation* op)
{

    //se o buffer estiver vazio ou já foi lida a posicção (necessario devido a n especificar o driver)
    if(buffer->ptrs->in == buffer->ptrs->out)
    {
        op->id = -1;
    }else 
    {
        *op = buffer->buffer[buffer->ptrs->out];
        buffer->ptrs->out = (buffer->ptrs->out + 1)%buffer_size;
    }

}



void read_driver_client_buffer(struct rnd_access_buffer* buffer, int client_id, int buffer_size, struct operation* op)
{
    int out = getFirstEqualClient(buffer, client_id, buffer_size);
    //Se existir posição ocupada
    if(out != -1)
    {
        *op = buffer->buffer[out];
        buffer->ptrs[out] = 0;
    }else
    {
        op->id = -1;
    }
}