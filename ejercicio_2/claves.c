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

    // Se copia el código de operación
    strcpy(to_send, "0");
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

    // Se envía la operación
    err = send(sd, (char*) to_send, strlen(to_send) + 1, 0); // Envia el mensaje a sd
    if(err == -1){
        printf("Error al enviar\n");
    }


    // Se espera la respuesta del servidor
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
    char *to_send = malloc(3 * sizeof(char));

    if (to_send == NULL) {
        printf("Memory allocation failed.\n");
        return -1;
    }
    // Se escribe en el buffer to_send el número de operacion
    strcpy(to_send, "2,");

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
    // Se calculan los caracteres de la clave introducida y se reserva espacio para copiarlo en la valiable to_send
    int int_length = snprintf(NULL, 0, "%d", key);
    to_send = realloc(to_send, (strlen(to_send) + int_length + 2) * sizeof(char)); // +2 por el final de string y la coma de separacion
    if (to_send == NULL) {
        printf("Memory reallocation failed.\n");
        return -1;
    }
    sprintf(to_send + strlen(to_send), "%d,", key);

    // Se hace lo mismo que para key pero con N_value2, que tambien es un int
    int_length = snprintf(NULL, 0, "%d", N_value2);
    to_send = realloc(to_send, (strlen(to_send) + int_length + 2) * sizeof(char)); // +2 por el final de string y la coma de separacion
    if (to_send == NULL) {
        printf("Memory reallocation failed.\n");
        return -1;
    }
    sprintf(to_send + strlen(to_send), "%d,", N_value2);

    int double_length;
    for (int i = 0; i < N_value2; i++){
        // Se hace lo mismo que con las variables int pero con una double
        double_length = snprintf(NULL, 0, "%f", V_value2[i]);
        to_send = realloc(to_send, (strlen(to_send) + double_length + 2) * sizeof(char));// +2 por el final de string y la coma de separacion
        if (to_send == NULL) {
            printf("Memory reallocation failed.\n");
            return -1;
        }
        sprintf(to_send + strlen(to_send), "%f,", V_value2[i]);
    }
    size_t str_length = strlen(value1);

    // Finalmente se reserva y concatena la variable value1
    to_send = realloc(to_send, (strlen(to_send) + str_length + 1) * sizeof(char));// +1 por el final del string
    if (to_send == NULL) {
        printf("Memory reallocation failed.\n");
        return -1;
    }
    strcat(to_send, value1);

    // Se envía la información al servidor y se libera la memoria
    err = send(sd, (char*) to_send, strlen(to_send) + 1, 0); // Envia el mensaje a sd
    if(err == -1){
        printf("Error al enviar value 1\n");

    }
    free(to_send);


    // Se espera la respuesta del servidor
    recv(sd, received, 256, 0);
    if (sscanf(received, "%d", &res) != 1){
        printf("Error al recibir respuesta");
        res = -1;
    }

    return res;}

int get_value(int key, char *value1, int *N_value2, double *V_value2) {

    int res;
    int sd;
    struct sockaddr_in server_addr;

    //inicializa el mensaje que se va a enviar
    char *to_send = malloc(3 * sizeof(char));
    if (to_send == NULL) {
        printf("Memory allocation failed.\n");
        return -1;
    }
    strcpy(to_send, "3,");

    // se establece comunicación
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

    // se contruye el mensaje
    int int_length = snprintf(NULL, 0, "%d", key);
    to_send = realloc(to_send, (strlen(to_send) + int_length + 2) * sizeof(char)); // +1 for the null terminator
    if (to_send == NULL) {
        printf("Memory reallocation failed.\n");
        return -1;
    }
    sprintf(to_send + strlen(to_send), "%d", key);

    //se envia el mensaje
    err = send(sd, (char*) to_send, strlen(to_send) + 1, 0); // Envia el mensaje a sd
    if(err == -1){
        printf("Error al enviar value 1\n");

    }
    free(to_send);

    //se recive el resultado
    char message[1807];
    int err1 = recv(sd, (char *) &message, 1807, 0);   // recibe la operación
    if (err1 == -1) {
        printf("Error en recepción de tupla en operacion get_value\n");
        res = -1;
    }

    char *token;
    token = strtok(message, ",");
    res = atoi(token);

    //si el resultado es correcto se obtienen todos los datos
    if (res >= 0){
        token = strtok(NULL, ",");
        *N_value2 = atoi(token);
        for (int i = 0; i < *N_value2; i++){
            token = strtok(NULL, ",");
            V_value2[i] = atof(token);
        }
        value1 = strtok(NULL, ",");
    }

    return res;
}


int delete_key(int key){
    int res;
    int sd;
    char received[256];
    struct sockaddr_in server_addr;

    //inicializa el mensaje que se va a enviar
    char *to_send = malloc(3 * sizeof(char));
    if (to_send == NULL) {
        printf("Memory allocation failed.\n");
        return -1;
    }
    strcpy(to_send, "1,");

    // se establece comunicación
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

    // se contruye el mensaje
    int int_length = snprintf(NULL, 0, "%d", key);
    to_send = realloc(to_send, (strlen(to_send) + int_length + 2) * sizeof(char)); // +1 for the null terminator
    if (to_send == NULL) {
        printf("Memory reallocation failed.\n");
        return -1;
    }
    sprintf(to_send + strlen(to_send), "%d", key);

    //se envia el mensaje
    err = send(sd, (char*) to_send, strlen(to_send) + 1, 0); // Envia el mensaje a sd
    if(err == -1){
        printf("Error al enviar value 1\n");

    }
    free(to_send);
    // Se espera la respuesta del servidor
    recv(sd, received, 256, 0);
    if (sscanf(received, "%d", &res) != 1){
        printf("Error al recibir respuesta");
        res = -1;
    }


    return res;
}

int modify_value(int key, char *value1, int N_value2, double *V_value2){
    int res;
    int sd;
    struct sockaddr_in server_addr;
    char received[256];
    char *to_send = malloc(3 * sizeof(char));

    if (to_send == NULL) {
        printf("Memory allocation failed.\n");
        return -1;
    }
    // Se escribe en el buffer to_send el número de operacion
    strcpy(to_send, "4,");

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
    // Se calculan los caracteres de la clave introducida y se reserva espacio para copiarlo en la valiable to_send
    int int_length = snprintf(NULL, 0, "%d", key);
    to_send = realloc(to_send, (strlen(to_send) + int_length + 2) * sizeof(char)); // +2 por el final de string y la coma de separacion
    if (to_send == NULL) {
        printf("Memory reallocation failed.\n");
        return -1;
    }
    sprintf(to_send + strlen(to_send), "%d,", key);

    // Se hace lo mismo que para key pero con N_value2, que tambien es un int
    int_length = snprintf(NULL, 0, "%d", N_value2);
    to_send = realloc(to_send, (strlen(to_send) + int_length + 2) * sizeof(char)); // +2 por el final de string y la coma de separacion
    if (to_send == NULL) {
        printf("Memory reallocation failed.\n");
        return -1;
    }
    sprintf(to_send + strlen(to_send), "%d,", N_value2);

    int double_length;
    for (int i = 0; i < N_value2; i++){
        // Se hace lo mismo que con las variables int pero con una double
        double_length = snprintf(NULL, 0, "%f", V_value2[i]);
        to_send = realloc(to_send, (strlen(to_send) + double_length + 2) * sizeof(char));// +2 por el final de string y la coma de separacion
        if (to_send == NULL) {
            printf("Memory reallocation failed.\n");
            return -1;
        }
        sprintf(to_send + strlen(to_send), "%f,", V_value2[i]);
    }
    size_t str_length = strlen(value1);

    // Finalmente se reserva y concatena la variable value1
    to_send = realloc(to_send, (strlen(to_send) + str_length + 1) * sizeof(char));// +1 por el final del string
    if (to_send == NULL) {
        printf("Memory reallocation failed.\n");
        return -1;
    }
    strcat(to_send, value1);
    printf("Se envia: %s\n", to_send);

    // Se envía la información al servidor y se libera la memoria
    err = send(sd, (char*) to_send, strlen(to_send) + 1, 0); // Envia el mensaje a sd
    if(err == -1){
        printf("Error al enviar value 1\n");

    }
    free(to_send);


    // Se espera la respuesta del servidor
    recv(sd, received, 256, 0);
    if (sscanf(received, "%d", &res) != 1){
        printf("Error al recibir respuesta");
        res = -1;
    }

    return res;
}

int exist(int key){
    int res;
    int sd;
    struct sockaddr_in server_addr;
    char received[256];

    //inicializa el mensaje
    char *to_send = malloc(3 * sizeof(char));
    if (to_send == NULL) {
        printf("Memory allocation failed.\n");
        return -1;
    }
    strcpy(to_send, "5,");

    // se establece la comunicación
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

    //constuir el mensaje
    int int_length = snprintf(NULL, 0, "%d", key);
    to_send = realloc(to_send, (strlen(to_send) + int_length + 2) * sizeof(char)); // +1 for the null terminator
    if (to_send == NULL) {
        printf("Memory reallocation failed.\n");
        return -1;
    }
    sprintf(to_send + strlen(to_send), "%d", key);

    //enviar el mensaje
    err = send(sd, (char*) to_send, strlen(to_send) + 1, 0); // Envia el mensaje a sd
    if(err == -1){
        printf("Error al enviar value 1\n");

    }
    free(to_send);

    // Se espera la respuesta del servidor
    recv(sd, received, 256, 0);
    if (sscanf(received, "%d", &res) != 1){
        printf("Error al recibir respuesta");
        res = -1;
    }

    return res;
}
