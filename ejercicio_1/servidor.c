//
// Created by rubenubuntu on 28/02/24.
//

#include "mqueue.h"
#include "stdio.h"
#include "pthread.h"
#include "stdbool.h"
#include "string.h"
#include "mensaje.h"

pthread_mutex_t m;
pthread_cond_t c;


void main(){
    mqd_t cola_servior;
    mqd_t cola_cliente;
    cola_servior = mq_open("/SERVER", O_CREAT|O_WRONLY);
    printf("Cola Servidor: %d\n", cola_servior);
    mq_close(cola_servior);
    mq_unlink("/SERVER");

}