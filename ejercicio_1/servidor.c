//
// Created by rubenubuntu on 11/03/24.
//
#include <mqueue.h>
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "mensaje.h"
#include <stdlib.h>


int *keys;
char **valores_1;
int* num_elements;
double **vectores;
int num_data = 0; // Numero de elementos almacenados

pthread_mutex_t mutex_mensaje;
int mensaje_no_copiado = 1;
pthread_cond_t cond_mensaje;
mqd_t  q_servidor;

void tratar_mensaje(void  *mess) {
    int resultado = -1;
    printf("Hilo creado\n");
    struct mensaje mensaje;    /* mensaje local */
    mqd_t q_cliente;        /* cola del cliente */

    /* el thread copia el mensaje a un mensaje local */
    pthread_mutex_lock(&mutex_mensaje);

    mensaje = (*(struct mensaje *) mess);

    // Una vez copiado el mensaje, se reanuda el hilo padre
    mensaje_no_copiado = false;

    pthread_cond_signal(&cond_mensaje);

    pthread_mutex_unlock(&mutex_mensaje);

    /* ejecutar la petición del  y preparar respuesta */
    printf("Mensaje copiado\n");
    q_cliente = mq_open(mensaje.cola_cliente, O_WRONLY);
    if (mensaje.op == 0){
        // Funcion init
        resultado = 1;
        if(mq_send(q_cliente, (const char *) &resultado, sizeof(int), 0) <0){
            pthread_exit(0);
        }
    }
    else if (mensaje.op == 1){
        // Funcion delete_key
        resultado = 100;
        if(mq_send(q_cliente, (const char *) &resultado, sizeof(int), 0) <0){
            pthread_exit(0);
        }
    }

    else if (mensaje.op == 2){
        // set_value
        num_data++;
        int *temp_keys = realloc(keys, num_data * sizeof(int));
        int *temp_num_elements = realloc(num_elements, num_data * sizeof(int));
        char **temp_valores_1 = NULL;
        temp_valores_1 = realloc(temp_valores_1, num_data * sizeof(char*));
        double **tempo_vectores = realloc(vectores, num_data * sizeof(double*));
        if (temp_keys == NULL) {
            printf("Memory allocation failed\n");
            resultado = -1;
            if(mq_send(q_cliente, (const char *) &resultado, sizeof(int), 0) <0){
                pthread_exit(0);
            }
        }
        if (temp_num_elements == NULL) {
            printf("Memory allocation failed\n");
            resultado = -1;
            if(mq_send(q_cliente, (const char *) &resultado, sizeof(int), 0) <0){
                pthread_exit(0);
            }
        }
        if (temp_valores_1 == NULL) {
            printf("Memory allocation failed\n");
            resultado = -1;
            if(mq_send(q_cliente, (const char *) &resultado, sizeof(int), 0) <0){
                pthread_exit(0);
            }
        }
        if (tempo_vectores == NULL) {
            printf("Memory allocation failed\n");
            resultado = -1;
            if(mq_send(q_cliente, (const char *) &resultado, sizeof(int), 0) <0){
                pthread_exit(0);
            }
        }
        // Hacemos la capacidad de la base de datos más grande
        keys = temp_keys;
        valores_1 = temp_valores_1;
        num_elements = temp_num_elements;
        vectores = tempo_vectores;

        // Asignamos los valores al nuevo elemento de la base de datos
        keys[num_data - 1] = mensaje.clave;
        num_elements[num_data - 1] = mensaje.n_elem;
        valores_1[num_data - 1] = (char *)malloc((sizeof(mensaje.valor_1) + 1) * sizeof(char));
        strcpy(valores_1[num_data - 1], mensaje.valor_1);
        vectores[num_data - 1] = (double *)malloc((mensaje.n_elem) * sizeof(double));
        for (int i = 0; i < mensaje.n_elem; i++){
            vectores[num_data - 1][i] = mensaje.vector[i];
        }
        printf("mensaje.key = %d, copy_key = %d\n", mensaje.clave, keys[num_data - 1]);
        printf("num_elements = %d, copy = %d\n", mensaje.n_elem, num_elements[num_data - 1]);
        printf("valor_1 = %s, copy = %s\n", mensaje.valor_1, valores_1[num_data - 1]);
        for(int i = 0; i < mensaje.n_elem;i++){
            printf("Vector[%d][%d] = %lf", num_data-1, i, vectores[num_data - 1][i]);
        }
    }
    else if (mensaje.op == 3){
        // Funcion get_values

        // Buscar valores en data
        for(int i = 0; i < sizeof(keys); i++)
        {
        }

        //Si se ecuentra
        if(mq_send(q_cliente, (const char *) &resultado, sizeof(int), 0) <0){
            pthread_exit(0);
        }
        // devolvel error no exiten
    }






    mq_send(q_cliente, (const char *) &resultado, sizeof(int), 0);
    pthread_exit(0);

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
        printf("Mensaje recibido\n");
        if(mensaje.op == -1){
            // Mensaje para terminar la ejecución del servidor y que este cierre sus colas
            printf("Terminando ejecución...\n");
            break;
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
    mq_unlink("/SERVIDOR"); // De momento comentar esta linea para que se quede abierta la cola
    return 0;

}