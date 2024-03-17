//
// Created by rubenubuntu on 28/02/24.
//
#include <mqueue.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include "mensaje.h"


int init(){
    mqd_t q_servidor;       /* cola de mensajes del proceso servidor */
    mqd_t q_cliente;        /* cola de mensajes para el proceso cliente */
    char queue_name[100];
    sprintf(queue_name, "/Cola-%d", getpid());
    struct mensaje mess;
    struct mq_attr attr;
    int res;

    attr.mq_maxmsg = 1;
    attr.mq_msgsize = sizeof(int);

    q_cliente = mq_open(queue_name, O_CREAT|O_RDONLY, 0700, &attr);
    if (q_cliente == -1){
        perror("Error al abrir la cola del cliente");
    }
    q_servidor = mq_open("/SERVIDOR", O_WRONLY);
    if (q_servidor == -1){
        perror("Error al abrir la cola del servidor");
    }


    // Inicializamos el mensaje
    mess.op = 0;
    strcpy(mess.cola_cliente, queue_name);
    for(int i = 0; i < 32; i++){mess.vector[i] = 0.0;}
    mess.n_elem = 0;
    strcpy(mess.valor_1, "");
    mess.clave = 0;

    if (mq_send(q_servidor, (const char *)&mess, sizeof(mess), 0) < 0){
        perror("mq_send");
        return -1;
    }


    if (mq_receive(q_cliente, (char *) &res, sizeof(int), 0) < 0){
        perror("mq_recv");
        return -1;
    }

    mq_close(q_servidor);
    mq_close(q_cliente);
    mq_unlink(queue_name);


    printf("Queue_Name = %s\n", queue_name);
    return res;
}

void exit_f(){
    mqd_t q_servidor;       /* cola de mensajes del proceso servidor */
    mqd_t q_cliente;        /* cola de mensajes para el proceso cliente */
    char queue_name[100];
    sprintf(queue_name, "/Cola-%d", getpid());
    struct mensaje mess;
    struct mq_attr attr;

    attr.mq_maxmsg = 1;
    attr.mq_msgsize = sizeof(int);

    q_cliente = mq_open(queue_name, O_CREAT|O_RDONLY, 0700, &attr);
    if (q_cliente == -1){
        perror("Error al abrir la cola del cliente");
    }
    q_servidor = mq_open("/SERVIDOR", O_WRONLY);
    if (q_servidor == -1){
        perror("Error al abrir la cola del servidor");
    }

    mess.op = -1;
    strcpy(mess.cola_cliente, queue_name);
    for(int i = 0; i < 32; i++){mess.vector[i] = 0.0;}

    mess.n_elem = 0;

    strcpy(mess.valor_1, "");


    mq_send(q_servidor, (const char *)&mess, sizeof(mess), 0);

    mq_close(q_servidor);
    mq_close(q_cliente);
    mq_unlink(queue_name);


    printf("Queue_Name = %s\n", queue_name);
}

int delete_key(int key){
    mqd_t q_servidor;       /* cola de mensajes del proceso servidor */
    mqd_t q_cliente;        /* cola de mensajes para el proceso cliente */
    char queue_name[100];
    sprintf(queue_name, "/Cola-%d", getpid());
    struct mensaje mess;
    struct mq_attr attr;
    int res;

    attr.mq_maxmsg = 1;
    attr.mq_msgsize = sizeof(int);

    q_cliente = mq_open(queue_name, O_CREAT|O_RDONLY, 0700, &attr);
    if (q_cliente == -1){
        perror("Error al abrir la cola del cliente");
    }
    q_servidor = mq_open("/SERVIDOR", O_WRONLY);
    if (q_servidor == -1){
        perror("Error al abrir la cola del servidor");
    }

// Inicializamos el mensaje
    mess.op = 1;
    strcpy(mess.cola_cliente, queue_name);
    for(int i = 0; i < 32; i++){mess.vector[i] = 0.0;}
    mess.n_elem = 0;
    strcpy(mess.valor_1, "");
    mess.clave = key;

    if (mq_send(q_servidor, (const char *)&mess, sizeof(mess), 0) < 0){
        perror("mq_send");
        return -1;
    }


    if (mq_receive(q_cliente, (char *) &res, sizeof(int), 0) < 0){
        perror("mq_recv");
        return -1;
    }

    mq_close(q_servidor);
    mq_close(q_cliente);
    mq_unlink(queue_name);


    printf("Queue_Name = %s\n", queue_name);
    return res;
}

int set_value(int key, char *value1, int N_value2, double *V_value2){
    printf("key = %d, value1 = %s, n_elem = %d ", key, value1, N_value2);
    for (int i = 0; i < N_value2; i++){
        printf("vector[%d] = %lf ", i, V_value2[i]);
    }
    printf("\n");


    mqd_t q_servidor;       /* cola de mensajes del proceso servidor */
    mqd_t q_cliente;        /* cola de mensajes para el proceso cliente */
    char queue_name[100];
    sprintf(queue_name, "/Cola-%d", getpid());
    struct mensaje mess;
    struct mq_attr attr;
    int res;

    attr.mq_maxmsg = 1;
    attr.mq_msgsize = sizeof(int);

    q_cliente = mq_open(queue_name, O_CREAT|O_RDONLY, 0700, &attr);
    if (q_cliente == -1){
        perror("Error al abrir la cola del cliente");
    }
    q_servidor = mq_open("/SERVIDOR", O_WRONLY);
    if (q_servidor == -1){
        perror("Error al abrir la cola del servidor");
    }


    // Inicializamos el mensaje
    mess.op = 2;
    strcpy(mess.cola_cliente, queue_name);
    for(int i = 0; i < N_value2; i++){mess.vector[i] = V_value2[i];}
    mess.n_elem = N_value2;
    strcpy(mess.valor_1, value1);
    mess.clave = key;

    if (mq_send(q_servidor, (const char *)&mess, sizeof(mess), 0) < 0){
        perror("mq_send");
        return -1;
    }


    if (mq_receive(q_cliente, (char *) &res, sizeof(int), 0) < 0){
        perror("mq_recv");
        return -1;
    }

    mq_close(q_servidor);
    mq_close(q_cliente);
    mq_unlink(queue_name);


    printf("Queue_Name = %s\n", queue_name);
    return res;}

int get_value(int key, char *value1, int *N_value2, double *V_value2) {
    mqd_t q_servidor;       /* cola de mensajes del proceso servidor */
    mqd_t q_cliente;        /* cola de mensajes para el proceso cliente */
    char queue_name[100];
    sprintf(queue_name, "/Cola-%d", getpid());
    struct mensaje mess;
    struct mq_attr attr;

    attr.mq_maxmsg = 1;
    attr.mq_msgsize = sizeof(struct mensaje);

    q_cliente = mq_open(queue_name, O_CREAT|O_RDONLY, 0700, &attr);
    if (q_cliente == -1){
        perror("Error al abrir la cola del cliente");
        return -1;
    }
    q_servidor = mq_open("/SERVIDOR", O_WRONLY);
    if (q_servidor == -1){
        perror("Error al abrir la cola del servidor");
        return -1;
    }

    // Inicializamos el mensaje
    mess.op = 3;
    strcpy(mess.cola_cliente, queue_name);
    for(int i = 0; i < 32; i++){mess.vector[i] = 0.0;}
    mess.n_elem = 0;
    strcpy(mess.valor_1, "");
    mess.clave = key;

    if (mq_send(q_servidor, (const char *)&mess, sizeof(mess), 0) < 0){
        perror("mq_send");
        return -1;
    }

    if (mq_receive(q_cliente, (char *) &mess, sizeof(struct mensaje), 0) < 0){
        perror("mq_recv");
        return -1;
    }


    
    int res = 0;
    strcpy(value1, mess.valor_1);
    *N_value2 = mess.n_elem;
    for(int i = 0; i < mess.n_elem; i++) {
        V_value2[i] = mess.vector[i];
    }
    res = mess.op;

    mq_close(q_servidor);
    mq_close(q_cliente);
    mq_unlink(queue_name);

    return res;
}

int modify_value(int key, char *value1, int N_value2, double *V_value2){
    printf("key = %d, value1 = %s, n_elem = %d ", key, value1, N_value2);
    for (int i = 0; i < N_value2; i++){
        printf("vector[%d] = %lf ", i, V_value2[i]);
    }
    printf("\n");


    mqd_t q_servidor;       /* cola de mensajes del proceso servidor */
    mqd_t q_cliente;        /* cola de mensajes para el proceso cliente */
    char queue_name[100];
    sprintf(queue_name, "/Cola-%d", getpid());
    struct mensaje mess;
    struct mq_attr attr;
    int res;

    attr.mq_maxmsg = 1;
    attr.mq_msgsize = sizeof(int);

    q_cliente = mq_open(queue_name, O_CREAT|O_RDONLY, 0700, &attr);
    if (q_cliente == -1){
        perror("Error al abrir la cola del cliente");
    }
    q_servidor = mq_open("/SERVIDOR", O_WRONLY);
    if (q_servidor == -1){
        perror("Error al abrir la cola del servidor");
    }


    // Inicializamos el mensaje
    mess.op = 4;
    strcpy(mess.cola_cliente, queue_name);
    for(int i = 0; i < N_value2; i++){mess.vector[i] = V_value2[i];}
    mess.n_elem = N_value2;
    strcpy(mess.valor_1, value1);
    mess.clave = key;

    if (mq_send(q_servidor, (const char *)&mess, sizeof(mess), 0) < 0){
        perror("mq_send");
        return -1;
    }


    if (mq_receive(q_cliente, (char *) &res, sizeof(int), 0) < 0){
        perror("mq_recv");
        return -1;
    }

    mq_close(q_servidor);
    mq_close(q_cliente);
    mq_unlink(queue_name);


    printf("Queue_Name = %s\n", queue_name);
    return res;
}

int exist(int key){
    mqd_t q_servidor;       /* cola de mensajes del proceso servidor */
    mqd_t q_cliente;        /* cola de mensajes para el proceso cliente */
    char queue_name[100];
    sprintf(queue_name, "/Cola-%d", getpid());
    struct mensaje mess;
    struct mq_attr attr;
    int res;

    attr.mq_maxmsg = 1;
    attr.mq_msgsize = sizeof(int);

    q_cliente = mq_open(queue_name, O_CREAT|O_RDONLY, 0700, &attr);
    if (q_cliente == -1){
        perror("Error al abrir la cola del cliente");
    }
    q_servidor = mq_open("/SERVIDOR", O_WRONLY);
    if (q_servidor == -1){
        perror("Error al abrir la cola del servidor");
    }


    // Inicializamos el mensaje
    mess.op = 5;
    strcpy(mess.cola_cliente, queue_name);
    for(int i = 0; i < 32; i++){mess.vector[i] = 0.0;}
    mess.n_elem = 0;
    strcpy(mess.valor_1, "");
    mess.clave = key;

    if (mq_send(q_servidor, (const char *)&mess, sizeof(mess), 0) < 0){
        perror("mq_send");
        return -1;
    }


    if (mq_receive(q_cliente, (char *) &res, sizeof(int), 0) < 0){
        perror("mq_recv");
        return -1;
    }

    mq_close(q_servidor);
    mq_close(q_cliente);
    mq_unlink(queue_name);


    printf("Queue_Name = %s\n", queue_name);
    return res;
}
