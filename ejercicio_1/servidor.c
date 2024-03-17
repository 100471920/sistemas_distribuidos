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
int inicializado = 1;

pthread_mutex_t mutex_shared_variables;
pthread_mutex_t mutex_mensaje;
int mensaje_no_copiado = 1;
pthread_cond_t cond_mensaje;
mqd_t  q_servidor;

void tratar_mensaje(void  *mess) {
    int resultado = -1;
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
    q_cliente = mq_open(mensaje.cola_cliente, O_WRONLY);
    if (mensaje.op == 0 && inicializado == 1){
        // Funcion init
        resultado = 0;
        inicializado = 0;
        num_data = 0;

        pthread_mutex_lock(&mutex_shared_variables);

        free(valores_1);
        free(keys);
        free(vectores);
        free(num_elements);

        pthread_mutex_unlock(&mutex_shared_variables);
        printf("Operación init realizada\n");

        if(mq_send(q_cliente, (const char *) &resultado, sizeof(int), 0) <0){
            pthread_exit(0);
        }
    }

    else if (mensaje.op == 1){
        // Funcion delete_key
        int index = -1;

        pthread_mutex_lock(&mutex_shared_variables);

        for (int i = 0; i < num_data; i++){
            if (keys[i] == mensaje.clave){
                index = i;
            }
        }
        if (index == -1){
            if(mq_send(q_cliente, (const char *) &index, sizeof(int), 0) <0){
                pthread_exit(0);
            }
            pthread_exit(0);
        }
        free(valores_1[index]);
        free(vectores[index]);

        for (int i = index; i < num_data - 1; i++){
            keys[i] = keys[i + 1];
            valores_1[i] = valores_1[i + 1];
            num_elements[i] = num_elements[i + 1];
            vectores[i] = vectores[i + 1];
        }
        num_data--;

        keys = realloc(keys, num_data * sizeof(int));
        valores_1 = realloc(valores_1, num_data * sizeof(char *));
        num_elements = realloc(num_elements, num_data * sizeof(int));
        vectores = realloc(vectores, num_data * sizeof(double *));

        resultado = 0;

        pthread_mutex_unlock(&mutex_shared_variables);
        printf("Operación delete_key realizada\n");

        if(mq_send(q_cliente, (const char *) &resultado, sizeof(int), 0) <0){
            pthread_exit(0);
        }
    }

    else if (mensaje.op == 2){
        // set_value
        resultado = 0;

        pthread_mutex_lock(&mutex_shared_variables);

        for(int i = 0; i < num_data; i++) {
            if (keys[i] == mensaje.clave) {
                resultado = -1;
                break;
            }
        }
        if ((mensaje.n_elem > 32) | (mensaje.n_elem < 1)){
            resultado = -1;
        }

        if(resultado != -1){
            num_data++;
            int *temp_keys = realloc(keys, num_data * sizeof(int));
            int *temp_num_elements = realloc(num_elements, num_data * sizeof(int));
            char **temp_valores_1 = NULL;
            temp_valores_1 = realloc(valores_1, num_data * sizeof(char*));
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
            }

        pthread_mutex_unlock(&mutex_shared_variables);

        printf("Operación set_value realizada\n");
        // Enviar respuesta al cliente
        if(mq_send(q_cliente, (const char *) &resultado, sizeof(int), 0) <0){
            perror("Error al enviar resultado al cliente\n");
            pthread_exit(0);
        }
    }

    else if (mensaje.op == 3){
        // Funcion get_value

        pthread_mutex_lock(&mutex_shared_variables);

        // Buscar valores en la base de datos
        for(int i = 0; i < num_data; i++) {
            if (keys[i] == mensaje.clave) {
                // Si se encuentra la clave, copiar los datos al mensaje de respuesta
                resultado = 0; // Indicador de éxito
                mensaje.n_elem = num_elements[i];
                strcpy(mensaje.valor_1, valores_1[i]);
                for(int j = 0; j < mensaje.n_elem; j++) {
                    mensaje.vector[j] = vectores[i][j];
                }
                break;
            }
        }

        mensaje.op = resultado;

        pthread_mutex_unlock(&mutex_shared_variables);
        printf("Operación get_value realizada\n");

        // Enviar respuesta al cliente
        if(mq_send(q_cliente, (const char *) &mensaje, sizeof(mensaje), 0) <0){
            perror("Error al enviar resultado al cliente\n");
            pthread_exit(0);
        }
    }

    else if (mensaje.op == 4){
        // Función modify_value

        pthread_mutex_lock(&mutex_shared_variables);

        for (int i = 0; i < num_data; i++){
            if (mensaje.clave == keys[i]){
                valores_1[i] = realloc(valores_1[i], sizeof(mensaje.valor_1) * sizeof(char));
                strcpy(valores_1[i], mensaje.valor_1);
                if (mensaje.n_elem != num_elements[i]){
                    num_elements[i] = mensaje.n_elem;
                    vectores[i] = realloc(vectores[i], sizeof(mensaje.n_elem) * sizeof(double));
                }
                for (int j = 0; j < mensaje.n_elem; j++){
                    vectores[i][j] = mensaje.vector[j];
                }
                resultado = 0;
                break;
            }
        }
        pthread_mutex_unlock(&mutex_shared_variables);
        printf("Operación modify_value realizada\n");

    }

    else if (mensaje.op == 5){
        // función exist
        resultado = 0;
        pthread_mutex_lock(&mutex_shared_variables);

        for (int i; i < num_data; i++){
            if (keys[i] == mensaje.clave){
                resultado = 1;
                break;
            }
        }
        pthread_mutex_unlock(&mutex_shared_variables);
        printf("Operación exist realizada\n");

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
        perror("Error al abrir la cola\n");
        return -1;
    }


    pthread_mutex_init(&mutex_shared_variables,NULL);
    pthread_mutex_init(&mutex_mensaje, NULL);
    pthread_cond_init(&cond_mensaje, NULL);
    pthread_attr_init(&t_attr);

    // Se crean los hilos de forma independiente
    pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED);

    while(1) {

        if (mq_receive(q_servidor, (char *) &mensaje, sizeof(mensaje), 0) < 0 ){
            perror("Error al recibir el mensaje\n");
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
}