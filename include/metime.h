/*
Grupo: 35
David Dantas 56331
Su Lishun 56375
Miguel Seabra 56344
*/
#include <sys/time.h>
#include <memory.h>
#include <stdio.h>
/*
Marca o tempo no sítio correto da estrutura op, de acordo com o seu estado atual
*/
void clock_time(struct operation *op);


/*
Devolve o tempo atual formatado das seguintes formas de acordo com mode, 
a string é devolvida através de memoria dinamica alocada 
1: "2022-2-31 14:53:19.943"
2: "000.001"
*/
char *format_time_complex(struct timespec t, int mode);

/*
Imprime o tempo atual no formato 1
*/
char *format_now();