/*
Grupo: 35
David Dantas 56331
Su Lishun 56375
Miguel Seabra 56344
*/
#include <mesignal.h>

struct main_data *g_data;
struct communication_buffers *g_buffers;
struct semaphores *g_sems;
int g_terminate = 0;

void print_status()
{
    printf("\n");
    for (int i = 0; g_data->results[i].status != '\0' && i < g_data->max_ops; i++)
    {
        struct operation op = g_data->results[i];
        if (op.status == 'C')
        {
            printf("request:%d status:%c start: %ld server:%d server_time: %ld proxy:%d proxy_time: %ld client:%d client_time: %ld\n",
                   op.id, op.status, op.start_time.tv_sec, op.requested_rest, op.rest_time.tv_sec, op.receiving_driver, op.driver_time.tv_sec, op.receiving_client, op.client_end_time.tv_sec);
        }
        else
        {
            printf("request:%d status:%c\n", op.id, op.status);
        }
    }
    printf("\n");
}

void create_signal(int signum, void *function)
{
    struct sigaction sa;
    sa.sa_handler = function;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if (sigaction(signum, &sa, NULL) == -1)
    {
        perror("Signal");
        exit(-1);
    }
}

void ctrlC()
{
    fclose(stdin);
    log_instruction(log_file, " ctrlC");
    g_terminate = 1;
}

void create_stop(struct main_data* data, struct communication_buffers* buffers, struct semaphores* sems)
{
    g_data = data;
    g_buffers = buffers;
    g_sems = sems;
    create_signal(SIGINT, ctrlC);
}

void create_alarm(int alarm_time)
{
    create_signal(SIGALRM, print_status);
    struct itimerval val;
    val.it_interval.tv_sec = alarm_time;
    val.it_interval.tv_usec = 0;
    val.it_value.tv_sec = alarm_time;
    val.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL, &val, 0);
}

void ignoreCTRLC()
{
    create_signal(SIGINT, SIG_IGN);
}