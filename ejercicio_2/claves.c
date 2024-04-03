//
// Created by rubenubuntu on 28/02/24.
//
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "mensaje.h"
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>


#define PORT 4200

int init(){
    int sd;
    struct sockaddr_in server_addr;
    int res;
    char received[256];
    char to_send[256];

    strcpy(to_send, "0");
    printf("n_op a enviar %s", to_send);
    sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sd < 0){
        perror("Error in socket");
        exit(-1);
    }
    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // Use localhost (127.0.0.1)
    server_addr.sin_port = htons(PORT);
    int err = connect(sd, (struct sockaddr *) &server_addr,  sizeof(server_addr)); // Se concta a sd el server_addr
    if (err == -1){
        printf("Error en connect\n");
        return -1;
    }
    err = send(sd, (char*) to_send, strlen(to_send) + 1, 0); // Envia el mensaje a sd
    if(err == -1){
        printf("Error al enviar\n");
    }
    recv(sd, received, 256, 0);
    if (sscanf(received, "%d", &res) != 1){
        printf("Error al recibir respuesta");
        res = -1;
    }

        return res;
}

int set_value(int key, char *value1, int N_value2, double *V_value2){
    int res;
    int sd;
    struct sockaddr_in server_addr;
    char received[256];
    char to_send[256];

    strcpy(to_send, "2");
    printf("n_op a enviar %s", to_send);

    sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sd < 0){
        perror("Error in socket");
        exit(-1);
    }
    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // Use localhost (127.0.0.1)
    server_addr.sin_port = htons(PORT);
    int err = connect(sd, (struct sockaddr *) &server_addr,  sizeof(server_addr)); // Se concta a sd el server_addr
    if (err == -1){
        printf("Error en connect\n");
        return -1;
    }

    err = send(sd, (char*) to_send, strlen(to_send) + 1, 0); // Envia el mensaje a sd
    if(err == -1){
        printf("Error al enviar n_op\n");
    }

    strcpy(to_send, value1);
    err = send(sd, (char*) to_send, strlen(to_send) + 1, 0); // Envia el mensaje a sd
    if(err == -1){
        printf("Error al enviar value 1\n");
    }
    sprintf(to_send, "%d", N_value2);
    err = send(sd, (char*) to_send, strlen(to_send) + 1, 0); // Envia el mensaje a sd
    if(err == -1){
        printf("Error al enviar value 1\n");
    }
    for (int i = 0; i < N_value2; i++){
        sprintf(to_send, "%f", V_value2[i]);
        err = send(sd, (char*) to_send, strlen(to_send) + 1, 0); // Envia el mensaje a sd
        if(err == -1){
            printf("Error al enviar value 1\n");
        }
    }

    recv(sd, received, 256, 0);
    if (sscanf(received, "%d", &res) != 1){
        printf("Error al recibir respuesta");
        res = -1;
    }

    return res;}



/*
int delete_key(int key){
    mqd_t q_servidor;       // cola de mensajes del proceso servidor
    mqd_t q_cliente;        // cola de mensajes para el proceso cliente
    char queue_name[100];
    sprintf(queue_name, "/Cola-%d", getpid());
    struct mensaje mess;
    struct mq_attr attr;
    int res;

    attr.mq_maxmsg = 1;
    attr.mq_msgsize = sizeof(int);

    q_cliente = mq_open(queue_name, O_CREAT|O_RDONLY, 0700, &attr);
    if (q_cliente == -1){
        perror("Error al abrir la cola del cliente\n");
    }
    q_servidor = mq_open("/SERVIDOR", O_WRONLY);
    if (q_servidor == -1){
        perror("Error al abrir la cola del servidor\n");
    }

    // Inicializamos el mensaje
    mess.op = 1;
    strcpy(mess.cola_cliente, queue_name);
    for(int i = 0; i < 32; i++){mess.vector[i] = 0.0;}
    mess.n_elem = 0;
    strcpy(mess.valor_1, "");
    mess.clave = key;

    if (mq_send(q_servidor, (const char *)&mess, sizeof(mess), 0) < 0){
        perror("Error al recibir\n");
        return -1;
    }


    if (mq_receive(q_cliente, (char *) &res, sizeof(int), 0) < 0){
        perror("Error al recibir\n");
        return -1;
    }

    mq_close(q_servidor);
    mq_close(q_cliente);
    mq_unlink(queue_name);


    return res;
}

int get_value(int key, char *value1, int *N_value2, double *V_value2) {
    mqd_t q_servidor;       // cola de mensajes del proceso servidor
    mqd_t q_cliente;        // cola de mensajes para el proceso cliente
    char queue_name[100];
    sprintf(queue_name, "/Cola-%d", getpid());
    struct mensaje mess;
    struct mq_attr attr;

    attr.mq_maxmsg = 1;
    attr.mq_msgsize = sizeof(struct mensaje);

    q_cliente = mq_open(queue_name, O_CREAT|O_RDONLY, 0700, &attr);
    if (q_cliente == -1){
        perror("Error al abrir la cola del cliente\n");
        return -1;
    }
    q_servidor = mq_open("/SERVIDOR", O_WRONLY);
    if (q_servidor == -1){
        perror("Error al abrir la cola del servidor\n");
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
        perror("Error al recibir\n");
        return -1;
    }

    if (mq_receive(q_cliente, (char *) &mess, sizeof(struct mensaje), 0) < 0){
        perror("Error al recibir\n");
        return -1;
    }


    
    int res;
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
    mqd_t q_servidor;       // cola de mensajes del proceso servidor
    mqd_t q_cliente;        // cola de mensajes para el proceso cliente
    char queue_name[100];
    sprintf(queue_name, "/Cola-%d", getpid());
    struct mensaje mess;
    struct mq_attr attr;
    int res;

    attr.mq_maxmsg = 1;
    attr.mq_msgsize = sizeof(int);

    q_cliente = mq_open(queue_name, O_CREAT|O_RDONLY, 0700, &attr);
    if (q_cliente == -1){
        perror("Error al abrir la cola del cliente\n");
    }
    q_servidor = mq_open("/SERVIDOR", O_WRONLY);
    if (q_servidor == -1){
        perror("Error al abrir la cola del servidor\n");
    }


    // Inicializamos el mensaje
    mess.op = 4;
    strcpy(mess.cola_cliente, queue_name);
    for(int i = 0; i < N_value2; i++){mess.vector[i] = V_value2[i];}
    mess.n_elem = N_value2;
    if(strlen(value1) > 255){return -1;}
    strcpy(mess.valor_1, value1);
    mess.clave = key;

    if (mq_send(q_servidor, (const char *)&mess, sizeof(mess), 0) < 0){
        perror("Error al envíar\n");
        return -1;
    }


    if (mq_receive(q_cliente, (char *) &res, sizeof(int), 0) < 0){
        perror("Error al recibir\n");
        return -1;
    }

    mq_close(q_servidor);
    mq_close(q_cliente);
    mq_unlink(queue_name);


    return res;
}

int exist(int key){
    mqd_t q_servidor;       // cola de mensajes del proceso servidor
    mqd_t q_cliente;        // cola de mensajes para el proceso cliente
    char queue_name[100];
    sprintf(queue_name, "/Cola-%d", getpid());
    struct mensaje mess;
    struct mq_attr attr;
    int res;

    attr.mq_maxmsg = 1;
    attr.mq_msgsize = sizeof(int);

    q_cliente = mq_open(queue_name, O_CREAT|O_RDONLY, 0700, &attr);
    if (q_cliente == -1){
        perror("Error al abrir la cola del cliente\n");
    }
    q_servidor = mq_open("/SERVIDOR", O_WRONLY);
    if (q_servidor == -1){
        perror("Error al abrir la cola del servidor\n");
    }


    // Inicializamos el mensaje
    mess.op = 5;
    strcpy(mess.cola_cliente, queue_name);
    for(int i = 0; i < 32; i++){mess.vector[i] = 0.0;}
    mess.n_elem = 0;
    strcpy(mess.valor_1, "");
    mess.clave = key;

    if (mq_send(q_servidor, (const char *)&mess, sizeof(mess), 0) < 0){
        perror("Error al enviar\n");
        return -1;
    }


    if (mq_receive(q_cliente, (char *) &res, sizeof(int), 0) < 0){
        perror("Error al recibir\n");
        return -1;
    }

    mq_close(q_servidor);
    mq_close(q_cliente);
    mq_unlink(queue_name);

    return res;
}
*/