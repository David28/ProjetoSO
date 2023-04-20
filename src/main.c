/*
Grupo: 35
David Dantas 56331
Su Lishun 56375
Miguel Seabra 56344
*/
#include <main.h>
#include <main-private.h>
#include <string.h>
#include <unistd.h>
#include <log.h>
#include <mesignal.h>
#include <metime.h>
#include <stats.h>
#include <time.h>
#include <semaphore.h>
#include <configuration.h>

FILE *log_file;
char *statistics_filename;



int main(int argc, char *argv[])
{
    // init data structures
    struct main_data *data = create_dynamic_memory(sizeof(struct
                                                          main_data));
    struct communication_buffers *buffers =
        create_dynamic_memory(sizeof(struct communication_buffers));

    buffers->main_rest = create_dynamic_memory(sizeof(struct
                                                      rnd_access_buffer));
    buffers->rest_driv = create_dynamic_memory(sizeof(struct
                                                      circular_buffer));
    buffers->driv_cli = create_dynamic_memory(sizeof(struct
                                                     rnd_access_buffer));
    // init semaphore data structure
    struct semaphores *sems = create_dynamic_memory(sizeof(struct
                                                           semaphores));
    sems->main_rest = create_dynamic_memory(sizeof(struct prodcons));
    sems->rest_driv = create_dynamic_memory(sizeof(struct prodcons));
    sems->driv_cli = create_dynamic_memory(sizeof(struct prodcons));

    // execute main code
    main_args(argc, argv, data);
    create_dynamic_memory_buffers(data);
    create_shared_memory_buffers(data, buffers);
    create_semaphores(data, sems);
    launch_processes(buffers, data, sems);
    user_interaction(buffers, data, sems);
    // release memory before terminating
    destroy_dynamic_memory(data);
    destroy_dynamic_memory(buffers->main_rest);
    destroy_dynamic_memory(buffers->rest_driv);
    destroy_dynamic_memory(buffers->driv_cli);
    destroy_dynamic_memory(buffers);
    destroy_dynamic_memory(sems->main_rest);
    destroy_dynamic_memory(sems->rest_driv);
    destroy_dynamic_memory(sems->driv_cli);
    destroy_dynamic_memory(sems);
}

void main_args(int argc, char *argv[], struct main_data *data)
{
    if (argc < 2)
    {
        printf("Uso: magnaeats config_filename   Exemplo: magnaeats config.txt\n\n");
        exit(-1);
    }

    struct config conf;
    get_configurations(argv[1], &conf);
    *data = conf.data;
    log_file = create_log_file(conf.log_filename);

    statistics_filename = conf.statistics_filename;
    create_alarm(conf.alarm_time);

    if (data->max_ops <= 0 || data->buffers_size <= 0 || data->n_restaurants <= 0 || data->n_drivers <= 0 || data->n_clients <= 0)
    {
        printf("Ficheiro de configuration errado, exemplo de ficheiro:\n10\n10\n1\n1\n1\nlog.txt\nstatic.txt\n100\n\n");
        exit(-1);
    }
}

// Gestão de Memória

void create_dynamic_memory_buffers(struct main_data *data)
{
    int restaurants_size = data->n_restaurants * sizeof(int *);
    int drivers_size = data->n_drivers * sizeof(int *);
    int clients_size = data->n_clients * sizeof(int *);

    data->restaurant_pids = create_dynamic_memory(restaurants_size);
    data->driver_pids = create_dynamic_memory(drivers_size);
    data->client_pids = create_dynamic_memory(clients_size);

    data->restaurant_stats = create_dynamic_memory(restaurants_size);
    data->driver_stats = create_dynamic_memory(drivers_size);
    data->client_stats = create_dynamic_memory(clients_size);
}

void create_shared_memory_buffers(struct main_data *data, struct communication_buffers *buffers)
{
    int *int_pointer;
    struct operation *op_pointer;

    op_pointer = create_shared_memory(STR_SHM_RESULTS, data->max_ops * sizeof(struct operation));
    data->results = op_pointer;
    int_pointer = create_shared_memory(STR_SHM_TERMINATE, sizeof(int));
    data->terminate = int_pointer;

    struct pointers *ptrs_ptr;
    // pointers
    int_pointer = create_shared_memory(STR_SHM_MAIN_REST_PTR, (sizeof(int)) * (data->buffers_size));
    buffers->main_rest->ptrs = int_pointer;
    struct pointers *struc_pointers = create_shared_memory(STR_SHM_REST_DRIVER_PTR, sizeof(struct pointers));
    buffers->rest_driv->ptrs = struc_pointers;
    int_pointer = create_shared_memory(STR_SHM_DRIVER_CLIENT_PTR, (sizeof(int)) * (data->buffers_size));
    buffers->driv_cli->ptrs = int_pointer;

    // buffers
    int size_op_buffers = sizeof(struct operation) * data->buffers_size;

    op_pointer = create_shared_memory(STR_SHM_MAIN_REST_BUFFER, size_op_buffers);
    buffers->main_rest->buffer = op_pointer;
    op_pointer = create_shared_memory(STR_SHM_REST_DRIVER_BUFFER, size_op_buffers);
    buffers->rest_driv->buffer = op_pointer;
    op_pointer = create_shared_memory(STR_SHM_DRIVER_CLIENT_BUFFER, size_op_buffers);
    buffers->driv_cli->buffer = op_pointer;
}

void destroy_memory_buffers(struct main_data *data, struct communication_buffers *buffers)
{
    // memoria dinâmica
    destroy_dynamic_memory(data->restaurant_pids);
    destroy_dynamic_memory(data->driver_pids);
    destroy_dynamic_memory(data->client_pids);

    destroy_dynamic_memory(data->restaurant_stats);
    destroy_dynamic_memory(data->driver_stats);
    destroy_dynamic_memory(data->client_stats);
    // libertar memória dos pointer para os pratos
    for (int i = 0; i < data->max_ops; i++)
    {
        // se foi criado esse pedido libertar memoria
        if (data->results[i].status)
            destroy_dynamic_memory((data->results + i)->requested_dish);
    }

    // memoria partilhada
    int size_op_buffers = sizeof(struct operation) * data->buffers_size;
    int size_ptr_buffers = sizeof(int) * data->buffers_size;
    destroy_shared_memory(STR_SHM_DRIVER_CLIENT_PTR, buffers->driv_cli->ptrs, size_ptr_buffers);
    destroy_shared_memory(STR_SHM_DRIVER_CLIENT_BUFFER, buffers->driv_cli->buffer, size_op_buffers);
    destroy_shared_memory(STR_SHM_REST_DRIVER_PTR, buffers->rest_driv->ptrs, sizeof(struct pointers));
    destroy_shared_memory(STR_SHM_REST_DRIVER_BUFFER, buffers->rest_driv->buffer, size_op_buffers);
    destroy_shared_memory(STR_SHM_MAIN_REST_PTR, buffers->main_rest->ptrs, size_ptr_buffers);
    destroy_shared_memory(STR_SHM_MAIN_REST_BUFFER, buffers->main_rest->buffer, size_op_buffers);

    destroy_shared_memory(STR_SHM_RESULTS, data->results, sizeof(*data->results));
    destroy_shared_memory(STR_SHM_TERMINATE, data->terminate, sizeof(int *));
}

// Gestão de processos

void launch_processes(struct communication_buffers *buffers, struct main_data *data, struct semaphores *sems)
{

    for (int i = 0; i < data->n_restaurants; i++)
    {
        data->restaurant_pids[i] = launch_restaurant(i, buffers, data, sems);
        if (data->restaurant_pids[i] == -1)
        {
            perror("Criacao de restaurante falhou\n");
            stop_execution(data, buffers, sems);
        }
    }

    for (int i = 0; i < data->n_drivers; i++)
    {
        data->driver_pids[i] = launch_driver(i, buffers, data, sems);
        if (data->driver_pids[i] == -1)
        {
            perror("Criacao de motorista falhou\n");
            stop_execution(data, buffers, sems);
        }
    }
    for (int i = 0; i < data->n_clients; i++)
    {
        data->client_pids[i] = launch_client(i, buffers, data, sems);
        if (data->client_pids[i] == -1)
        {
            perror("Criacao de cliente falhou\n");
            stop_execution(data, buffers, sems);
        }
    }
}
void stop_execution(struct main_data *data, struct communication_buffers *buffers, struct semaphores *sems)
{

    *data->terminate = 1;

    wakeup_processes(data, sems);
    wait_processes(data);

    write_statistics(data);
    destroy_semaphores(sems);
    destroy_memory_buffers(data, buffers);
    close_file(log_file);
}

void wait_processes(struct main_data *data)
{
    // esperar pelos clientes
    for (int i = 0; i < data->n_clients; i++)
    {
        if (data->client_pids[i] != -1)
            data->client_stats[i] = wait_process(data->client_pids[i]);
    }
    // esperar pelos condutores
    for (int i = 0; i < data->n_drivers; i++)
    {
        if (data->driver_pids[i] != -1)
            data->driver_stats[i] = wait_process(data->driver_pids[i]);
    }
    // esperar pelos restaurantes
    for (int i = 0; i < data->n_restaurants; i++)
    {
        if (data->restaurant_pids[i] != -1)
            data->restaurant_stats[i] = wait_process(data->restaurant_pids[i]);
    }
}

// Interação com o utilizador

void user_interaction(struct communication_buffers *buffers, struct main_data *data, struct semaphores *sems)
{
    int x = 0;
    int *op_counter = &x;

    // perparar ctrl+c e variaveis globais
    create_stop(data, buffers, sems);
    //
    char *help = "Ações disponíveis:\n\trequest client restaurant dish - criar um novo pedido\n\tstatus id - consultar o estado de um pedido\n\tstop - termina a execução do magnaeats.\n\thelp - imprime informação sobre as ações disponíveis.\n";
    printf("%s", help);
    g_terminate = 0;
    while (!g_terminate)
    {
        printf("Itroduza uma ação:\n");
        char str[5] = "\0";
        scanf("%4s", str);
        if (!strcmp(str, "op"))
        {
            log_instruction(log_file, " request ");
            create_request(op_counter, buffers, data, sems);
        }
        else if (!strcmp(str, "status"))
        {
            read_status(data, sems);
        }
        else if (!strcmp(str, "stop"))
        {
            log_instruction(log_file, " stop ");
            g_terminate = 1;
        }
        else if (!strcmp(str, "help"))
        {
            printf("%s", help);
            log_instruction(log_file, " help ");
        }
        else if (str[0] != '\0' && str != "request:" && g_terminate != 1)
        {
            printf("Ação não reconhecida, insira 'help' para assistência.\n");
        }
    }
    stop_execution(data, buffers, sems);
}

void create_request(int *op_counter, struct communication_buffers *buffers, struct main_data *data, struct semaphores *sems)
{

    if (*op_counter < data->max_ops)
    {
        struct operation new_op =
            {.id = *op_counter,
             .status = 'I',
             .requesting_client = -1, // caso a leitura corra mal (Ex. request a sd salad)
             .requested_rest = -1,    // caso a leitura corra mal
             .requested_dish = calloc(20, sizeof(char))};

        scanf("%d %d", &new_op.requesting_client, &new_op.requested_rest); // caso a leitura falhe continuam como -1
        scanf(" %20s", new_op.requested_dish);
        clock_time(&new_op);

        semaphore_mutex_lock(sems->results_mutex);
        data->results[*op_counter] = new_op;
        semaphore_mutex_unlock(sems->results_mutex);

        produce_begin(sems->main_rest);
        write_main_rest_buffer(buffers->main_rest, data->buffers_size, (data->results + *op_counter));
        produce_end(sems->main_rest);

        printf("Pedido de id #%d criado.\n", *op_counter);
        *op_counter = *op_counter + 1;
        usleep(100000);
    }
    else
    {
        printf("O número máximo de pedidos foi alcançado!\n");
        scanf("%*s %*s %*s"); // limpar tokens
    }
}

void read_status(struct main_data *data, struct semaphores *sems)
{
    int id;
    scanf("%d", &id);

    if (id < 0 || id >= data->max_ops)
    {
        printf("id de pedido fornecido é inválido!\n");
        return;
    }
    log_instruction_s(log_file, " status ", id);
    struct operation *requested_op = &data->results[id];
    printf("%c\n", requested_op->status);
    if (requested_op->status != 0)
    {
        printf("Pedido %d com estado %c requisitado pelo cliente %d ao restaurante %d com o prato %s",
               id, requested_op->status, requested_op->requesting_client, requested_op->requested_rest,
               requested_op->requested_dish);
        statusDetails(requested_op);
    }
    else
        printf("Pedido %d ainda não é válido!\n", id);
}

void write_statistics(struct main_data *data)
{
    out_statistics(data, statistics_filename);
    printf("\nTerminando o MAGNAEATS! Imprimindo estatísticas: \n");
    // cada restaurante
    for (int i = 0; i < data->n_restaurants; i++)
    {
        printf("Restaurante %d preparou %d pedidos!\n", i, data->restaurant_stats[i]);
    }
    // cada motorista
    for (int i = 0; i < data->n_drivers; i++)
    {
        printf("Motorista %d entregou %d pedidos!\n", i, data->driver_stats[i]);
    }
    // cada cliente
    for (int i = 0; i < data->n_clients; i++)
    {
        printf("Cliente %d recebeu %d pedidos!\n", i, data->client_stats[i]);
    }
}

void statusDetails(struct operation *op)
{
    switch (op->status)
    {
    case 'I':
        printf(", ainda não foi recebido no restaurante!\n");
        break;
    case 'R':
        printf(", foi tratado pelo restaurante %d, mas ainda não foi entregue ao motorista!\n", op->receiving_rest);
        break;
    case 'D':
        printf(", foi tratado pelo restaurante %d, encaminhado pelo motorista %d, ainda não foi entregue ao cliente!\n",
               op->receiving_rest, op->receiving_driver);
        break;
    case 'C':
        printf(", foi tratado pelo restaurante %d, encaminhado pelo motorista %d, e enviado ao cliente %d\n",
               op->receiving_rest, op->receiving_driver, op->receiving_client);
        break;
    }
}

// semaforos

void create_semaphores(struct main_data *data, struct semaphores *sems)
{

    sems->main_rest->full = semaphore_create(STR_SEM_MAIN_REST_FULL, 0);
    sems->main_rest->empty = semaphore_create(STR_SEM_MAIN_REST_EMPTY, data->buffers_size);
    sems->main_rest->mutex = semaphore_create(STR_SEM_MAIN_REST_MUTEX, 1);

    sems->rest_driv->full = semaphore_create(STR_SEM_REST_DRIV_FULL, 0);
    sems->rest_driv->empty = semaphore_create(STR_SEM_REST_DRIV_EMPTY, data->buffers_size);
    sems->rest_driv->mutex = semaphore_create(STR_SEM_REST_DRIV_MUTEX, 1);

    sems->driv_cli->full = semaphore_create(STR_SEM_DRIV_CLI_FULL, 0);
    sems->driv_cli->empty = semaphore_create(STR_SEM_DRIV_CLI_EMPTY, data->buffers_size);
    sems->driv_cli->mutex = semaphore_create(STR_SEM_DRIV_CLI_MUTEX, 1);

    sems->results_mutex = semaphore_create(STR_SEM_RESULTS_MUTEX, 1);
}

void wakeup_processes(struct main_data *data, struct semaphores *sems)
{
    for (size_t i = 0; i < data->n_restaurants; i++)
    {
        produce_end(sems->main_rest);
    }
    for (size_t i = 0; i < data->n_drivers; i++)
    {
        produce_end(sems->rest_driv);
    }
    for (size_t i = 0; i < data->n_clients; i++)
    {
        produce_end(sems->driv_cli);
    }
}

void destroy_semaphores(struct semaphores *sems)
{
    semaphore_destroy(STR_SEM_MAIN_REST_FULL, sems->main_rest->full);
    semaphore_destroy(STR_SEM_MAIN_REST_EMPTY, sems->main_rest->empty);
    semaphore_destroy(STR_SEM_MAIN_REST_MUTEX, sems->main_rest->mutex);

    semaphore_destroy(STR_SEM_REST_DRIV_FULL, sems->rest_driv->full);
    semaphore_destroy(STR_SEM_REST_DRIV_EMPTY, sems->rest_driv->empty);
    semaphore_destroy(STR_SEM_REST_DRIV_MUTEX, sems->rest_driv->mutex);

    semaphore_destroy(STR_SEM_DRIV_CLI_FULL, sems->driv_cli->full);
    semaphore_destroy(STR_SEM_DRIV_CLI_EMPTY, sems->driv_cli->empty);
    semaphore_destroy(STR_SEM_DRIV_CLI_MUTEX, sems->driv_cli->mutex);

    semaphore_destroy(STR_SEM_RESULTS_MUTEX, sems->results_mutex);
}