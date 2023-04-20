/*
Grupo: 35
David Dantas 56331
Su Lishun 56375
Miguel Seabra 56344
*/
#include <metime.h>


void clock_time(struct operation *op)
{
    if (op->status == 'I')
        clock_gettime(CLOCK_REALTIME, &op->start_time);
    else if (op->status == 'R')
        clock_gettime(CLOCK_REALTIME, &op->rest_time);
    else if (op->status == 'D')
        clock_gettime(CLOCK_REALTIME, &op->driver_time);
    else if (op->status == 'C')
        clock_gettime(CLOCK_REALTIME, &op->client_end_time);
}



char *format_time_complex(struct timespec t, int mode){

    struct tm nowTime;
    
    char* current = create_dynamic_memory((sizeof (char)) * 14);
    //tempo do t com milisegundo
     if(mode == 1){
        localtime_r(&t.tv_sec, &nowTime);

        sprintf(current, "%04d-%02d-%02d %02d:%02d:%02d.%03ld", nowTime.tm_year + 1900, nowTime.tm_mon, nowTime.tm_mday,
            nowTime.tm_hour, nowTime.tm_min, nowTime.tm_sec, t.tv_nsec/1000000);        
    }else if(mode == 2){ //intervalo de tempo 
        sprintf(current, "%02ld.%03ld", t.tv_sec, t.tv_nsec);  
    }
    
    

    return current;
}

char* format_now()
{
    struct timespec time;
    clock_gettime(CLOCK_REALTIME, &time); // Get relative to 1970 The number of seconds to the present
    return format_time_complex(time, 1);
}