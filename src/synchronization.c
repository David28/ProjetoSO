/*
Grupo: 35
David Dantas 56331
Su Lishun 56375
Miguel Seabra 56344
*/
#include <synchronization.h>
#include <semaphore.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

sem_t *semaphore_create(char *name, int value)
{
    sem_t *sem = sem_open(name, O_CREAT, 0xFFFFFFFF, value);
    if (sem == SEM_FAILED)
    {
        perror("create");
        exit(6);
    }
    return sem;
}

void semaphore_destroy(char *name, sem_t *semaphore)
{
    if (sem_close(semaphore) == -1)
    {
        perror("sem");
    }
    if (sem_unlink(name) == -1)
    {
        perror(name);
    }
}

void produce_begin(struct prodcons *pc)
{
    semaphore_mutex_lock(pc->empty);
    semaphore_mutex_lock(pc->mutex);
}

void produce_end(struct prodcons *pc)
{
    semaphore_mutex_unlock(pc->mutex);
    semaphore_mutex_unlock(pc->full);
}

void consume_begin(struct prodcons *pc)
{
    semaphore_mutex_lock(pc->full); 
    semaphore_mutex_lock(pc->mutex);
} 

void consume_end(struct prodcons *pc)
{   
    semaphore_mutex_unlock(pc->mutex);
    semaphore_mutex_unlock(pc->empty);
}

void semaphore_mutex_lock(sem_t *sem)
{
    sem_wait(sem);
}

void semaphore_mutex_unlock(sem_t *sem)
{
    sem_post(sem);
}