//
// Created by rubenubuntu on 11/03/24.
//
#include <mqueue.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include "mensaje.h"

int main(){
    mqd_t q_servidor;
    mqd_t q_cliente;
    struct mensaje mensaje;
    struct mq_attr attr;
    char queuename[100];

    attr.mq_maxmsg = 1;
    attr.mq_msgsize = sizeof(int);

    sprintf(queuename, "/Cola-%d", getpid());
    printf("Cola Cliente: %s\n", queuename);
    q_cliente = mq_open(queuename, O_CREAT|O_RDONLY, 0700, &attr);
    if (q_cliente == -1){
        perror("Error al abrir la cola del cliente");

    }
    q_servidor = mq_open("/SERVIDOR", O_WRONLY);
    if (q_servidor == -1){
        perror("Error al abrir la cola del servidor");
    }
    mq_close(q_servidor);
    mq_close(q_cliente);
    mq_unlink(queuename);
}