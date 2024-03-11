//
// Created by rubenubuntu on 11/03/24.
//
#include <mqueue.h>
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "mensaje.h"


pthread_mutex_t mutex_mensaje;
int mensaje_no_copiado = 1;
pthread_cond_t cond_mensaje;
mqd_t  q_servidor;

void tratar_mensaje(void  *mess) {
    struct peticion mensaje;    /* mensaje local */
    mqd_t q_cliente;        /* cola del cliente */
    int resultado;        /* resultado de la operación */

    /* el thread copia el mensaje a un mensaje local */
    pthread_mutex_lock(&mutex_mensaje);

    mensaje = (*(struct peticion *) mess);

    // Una vez copiado el mensaje, se reanuda el hilo padre
    mensaje_no_copiado = false;

    pthread_cond_signal(&cond_mensaje);

    pthread_mutex_unlock(&mutex_mensaje);

    /* ejecutar la petición del cliente y preparar respuesta */
}

int main(){
    struct mq_attr attr;
    struct mensaje mensaje;
    pthread_attr_t t_attr;		// atributos de los threads
    pthread_t thid;

    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(struct mensaje);

    q_servidor = mq_open("/SERVIDOR", O_CREAT|O_RDONLY, 0700, &attr);
    if (q_servidor == -1) {
        perror("Error al abrir la cola");
        return -1;
    }
    printf("Cola Servidor: %d\n", q_servidor);



    pthread_mutex_init(&mutex_mensaje, NULL);
    pthread_cond_init(&cond_mensaje, NULL);
    pthread_attr_init(&t_attr);

    // Se crean los hilos de forma independiente
    pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED);

    while(1) {

        if (mq_receive(q_servidor, (char *) &mensaje, sizeof(mensaje), 0) < 0 ){
            perror("Error al recibir el mensaje");
            return -1;
        }

        if (pthread_create(&thid, &t_attr, (void *)tratar_mensaje, (void *)&mensaje)== 0) {
            // Se espera a que el thread copie el mensaje
            pthread_mutex_lock(&mutex_mensaje);
            while (mensaje_no_copiado) pthread_cond_wait(&cond_mensaje, &mutex_mensaje);
            mensaje_no_copiado = true;
            pthread_mutex_unlock(&mutex_mensaje);
        }
    }

    mq_close(q_servidor);
    mq_unlink("/SERVIDOR");


}