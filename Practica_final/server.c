#include <stdio.h>      // Para entrada y salida estándar
#include <stdlib.h>     // Para funciones de propósito general, como malloc() y exit()
#include <string.h>     // Para funciones de manipulación de cadenas de caracteres
#include <unistd.h>     // Para funciones y constantes relacionadas con el sistema operativo, como close()
#include <pthread.h>    // Para soporte de hilos (multithreading)
#include <sys/socket.h> // Para funciones relacionadas con sockets
#include <netinet/in.h> // Para estructuras relacionadas con direcciones de sockets
#include <arpa/inet.h>  // Para funciones de conversión de direcciones IP
#include <errno.h>      // Para el manejo de errores
#include <stdbool.h>
#include "rpc_conexion.h"
#include <rpc/rpc.h>

//operaciones que se envian contra el servidor
/*
REGISTER        (UserName)
UNREGISTER          ||
CONNECT             ||
DISCONNECT          ||

PUBLISH         (file_name,description)
DELETE          (file_name)
LIST_USERS      ()
LIST_CONTENT    (UserName)
*/

char **users;           // Usuarios registrados
char **clients_ip;      // Ip de los usuarios
char **client_socket;   // Socket de los usuarios
char **conexions;       // Usuarios conectados
char **descriptions;    // Descripciones de los ficheros
char **authors;         // Autores de los ficheros
char **file_names;      // Nombre de los ficheros
int num_files = 0;      // Numero de ficheros subidos
int registered = 0;     // Numero de clientes registrados
int connected = 0;      // Numero de clientes conectados

pthread_mutex_t mutex_shared_variables;
pthread_mutex_t mutex_mensaje;
int mensaje_no_copiado = 1;
pthread_cond_t cond_mensaje;
int sd;
struct sockaddr_in client_addr;


void REGISTER(int sc, char *token){
    
    char resultado[2];
    strcpy(resultado, "0");
    registered++;
    char ** aux = realloc(users, registered * sizeof(char*));
    if (aux == NULL) {
        // Si falla la reserva de memoria se resetea
        printf("Memory allocation failed\ns> ");
        strcpy(resultado, "2");
        registered--;
        if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
            pthread_mutex_unlock(&mutex_shared_variables);
            pthread_exit(0);
        }
    }
    else{
        users = aux;
        users[registered - 1] = (char *) malloc((strlen(token) + 1) * sizeof(char));
        strcpy(users[registered - 1], token);
        if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
            pthread_mutex_unlock(&mutex_shared_variables);
            pthread_exit(0);
        }
    }
}

void UNREGISTER(int sc, int to_delete){
    char resultado[2];
    free(users[to_delete]);
    for(int i = to_delete; i < registered - 1; i++){
        users[i] = users[i + 1];
    }
    registered--;
    char **aux = NULL;
    if (registered != 0){
        aux = realloc(users, registered * sizeof(char *));
    }
    if (aux == NULL && registered > 0){
        strcpy(resultado, "2");
    }
    else{
        users = aux;
        strcpy(resultado, "0");
    }
    if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
            pthread_mutex_unlock(&mutex_shared_variables);
            pthread_exit(0);
    }
}

void CONNECT(int sc, char* token, char client_ip[INET_ADDRSTRLEN]){
    char resultado[2];
    strcpy(resultado, "0");
    connected++;
    char**aux = realloc(conexions, connected * sizeof(char*));
    if (aux == NULL){
        strcpy(resultado, "3");
        connected--;
    }
    else{
        conexions = aux;
        conexions[connected - 1] = (char*) malloc ((strlen(token) + 1) * sizeof(char));
        if (conexions[connected - 1] == NULL){
            strcpy(resultado, "3");
        }
        else{
            strcpy(conexions[connected - 1], token);
        }
    }



    aux = realloc(clients_ip, connected * sizeof(char *));
    if (aux == NULL){
        strcpy(resultado, "3");
        connected--;
        free(conexions[connected]);
    }
    else{
        clients_ip = aux;
        clients_ip[connected - 1] = (char *) malloc((strlen(client_ip) + 1) * sizeof(char));
        if (clients_ip[connected - 1] == NULL){
            strcpy(resultado, "3");
        }
        else{
            strcpy(clients_ip[connected - 1], client_ip);
        }
    }


    token = strtok(NULL, ",");
    aux = realloc(client_socket, connected * sizeof(char *));
    if (aux == NULL){
        strcpy(resultado, "3");
        connected--;
        free(clients_ip[connected]);
        free(conexions[connected]);
    }
    else{
        client_socket = aux;
        client_socket[connected - 1] = (char *) malloc((strlen(token) + 1) * sizeof(char));
        if (client_socket[connected - 1] == NULL){
            strcpy(resultado, "3");
        }
        else{
            strcpy(client_socket[connected - 1], token);
        }
    }

    if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
        pthread_mutex_unlock(&mutex_shared_variables);
        pthread_exit(0);
    }
}

void DISCONNECT(int to_delete, char*resultado){
    // Desconectamos al usuario
    free(conexions[to_delete]);
    free(clients_ip[to_delete]);
    free(client_socket[to_delete]);
    for (int i = to_delete; i < connected -1; i++){
        conexions[i] = conexions[i + 1];
        clients_ip[i] = clients_ip[i + 1];
        client_socket[i] = client_socket[i + 1];
    }
    char**aux_1;
    char**aux_2;
    char**aux_3;
    connected--;
    if (connected != 0){
        aux_1 = realloc(conexions, connected * sizeof(char*));
        aux_2= realloc(clients_ip, connected * sizeof(char*));
        aux_3 = realloc(client_socket, connected * sizeof(char*));
    }
    if (aux_1 == NULL || aux_2 == NULL || aux_3 == NULL){
        strcpy(resultado, "3");
    }
}

void PUBLISH(char *token, char *resultado, int index_author){
    // Hacemos espacio para los datos y los almacenamos
    num_files++;
    char ** aux = realloc(file_names, num_files * sizeof(char*));
    if (aux == NULL){
        // Error en asignacion de memoria
        strcpy(resultado, "4");
        num_files--;
    }
    else{
        file_names = aux;
        file_names[num_files - 1] = (char*) malloc ((strlen(token) + 1) * sizeof(char));
        if (file_names[num_files - 1] == NULL){
            // Error en asignacion de memoria
            strcpy(resultado, "4");
            num_files--;
        }
        else{
            strcpy(file_names[num_files - 1], token);
        }
    }                
    aux = realloc(authors, num_files * sizeof(char*));
    if (aux == NULL){
        // Error en asignacion de memoria
        strcpy(resultado, "4");
        free(file_names[num_files - 1]);
        num_files--;
    }
    else{
        authors = aux;
        authors[num_files - 1] = (char*) malloc ((sizeof(conexions[index_author]) + 1) * sizeof(char));
        if (authors[num_files - 1] == NULL){
            // Error en asignacion de memoria
            strcpy(resultado, "4");
            free(file_names[num_files - 1]);
            num_files--;
        }
        else{
            strcpy(authors[num_files - 1], conexions[index_author]);
        }
    }
    token = strtok(NULL, ",");

    aux = realloc(descriptions, num_files * sizeof(char*));
    if (aux == NULL){
        // Error en asignacion de memoria
        strcpy(resultado, "4");
        free(file_names[num_files - 1]);
        free(authors[num_files - 1]);
        num_files--;
    }
    else{
        descriptions = aux;
        descriptions[num_files - 1] = (char*) malloc ((strlen(token) + 1) * sizeof(char));
        if (descriptions[num_files - 1] == NULL){
            // Error en asignacion de memoria
            strcpy(resultado, "4");
            free(file_names[num_files - 1]);
            free(authors[num_files - 1]);
            num_files--;
        }
        else{
            strcpy(descriptions[num_files - 1], token);
        }
    }
}

void DELETE(int to_delete, char*resultado){
    free(file_names[to_delete]);
    free(descriptions[to_delete]);
    for (int i = to_delete; i < num_files - 1; i++){
        file_names[i] = file_names[i + 1];
        descriptions[i] = descriptions[i + 1];
    }
    num_files--;
    if (num_files != 0){
        char ** aux = realloc(file_names, num_files * sizeof(char*));
        if (aux == NULL){
            strcpy(resultado, "4");
            num_files++;
        }
        else{
            file_names = aux;
            aux = realloc(descriptions, num_files * sizeof(char*));
            if (aux == NULL){
                strcpy(resultado, "4");
            }
            else{
                descriptions = aux;
            }
        }
    }
}

void LIST_USERS(int sc){
    char resultado[2];
    strcpy(resultado, "0");
    // Calculamos lo larga que va a ser nuestra cadena de texto
    int total_size = strlen(resultado);
    total_size++; // Deja espacio para una coma
    for (int i = 0; i < connected; i++){
        total_size += strlen(users[i]);
        total_size++; // Deja espacio para una coma
        total_size += strlen(clients_ip[i]);
        total_size++; // Deja espacio para una coma
        total_size += strlen(client_socket[i]);
        total_size++; // \0 señalizando final y , en el resto
    }
    char to_send[total_size];
    strcpy(to_send, "");
    // Concatenamos el resultado final
    strcat(to_send, "0,");
    for (int i = 0; i < connected; i++){
        strcat(to_send, users[i]);
        strcat(to_send, ",");
        strcat(to_send, clients_ip[i]);
        strcat(to_send, ",");
        strcat(to_send, client_socket[i]);
        if (i != connected - 1){
            strcat(to_send, ",");
        } 
    }
    if (send(sc, (char*) to_send, strlen(to_send) + 1, 0) < 0) {
        pthread_mutex_unlock(&mutex_shared_variables);
        pthread_exit(0);
    }
            
}

void LIST_CONTENT(int sc, char*token){
    char resultado[2];
    strcpy(resultado, "0");
    // Calculamos lo larga que va a ser nuestra cadena de texto
    int total_size = strlen(resultado);
    total_size++; // Deja espacio para una coma
    for (int i = 0; i < num_files; i++){
        if (strcmp(token, authors[i]) == 0){
            total_size += strlen(file_names[i]);
            total_size++; // Deja espacio para una coma
            total_size += strlen(descriptions[i]);
            total_size++; // \0 señalizando final y , en el resto
        }
    }
    char to_send[total_size];
    strcpy(to_send, "");
    // Concatenamos el resultado final
    strcat(to_send, "0,");
    for (int i = 0; i < num_files; i++){
        if (strcmp(token, authors[i]) == 0){
            strcat(to_send, file_names[i]);
            strcat(to_send, ",");
            strcat(to_send, descriptions[i]);
            if (i != num_files - 1){
                strcat(to_send, ",");
            } 
        }
    }
    if (send(sc, (char*) to_send, strlen(to_send) + 1, 0) < 0) {
        pthread_mutex_unlock(&mutex_shared_variables);
        pthread_exit(0);
    }
}

void GET_FILE(int sc, int objetivo){
    char resultado[2];
    strcpy(resultado, "0");
    // Calculamos lo larga que va a ser nuestra cadena de texto
    int total_size = strlen(resultado);
    total_size++; // Deja espacio para una coma
    total_size += strlen(clients_ip[objetivo]);
    total_size++; // Deja espacio para una coma
    total_size += strlen(client_socket[objetivo]);
    total_size++; // \0 señalizando final
    
    char to_send[total_size];
    strcpy(to_send, "");
    // Concatenamos el resultado final
    strcat(to_send, "0,");
    strcat(to_send, clients_ip[objetivo]);
    strcat(to_send, ",");
    strcat(to_send, client_socket[objetivo]);
    
    if (send(sc, (char*) to_send, strlen(to_send) + 1, 0) < 0) {
        pthread_mutex_unlock(&mutex_shared_variables);
        pthread_exit(0);
    }
}


void tratar_mensaje(int  *socket) {
    CLIENT *client;
    client = clnt_create("localhost", OPERACIONES_PROG, OPERACIONES_VERS, "tcp");
    char client_ip[INET_ADDRSTRLEN]; // IP del cliente que manda el mensaje
    char resultado[2]; // Variable que almacena el resultado en caso de que este sea solo un número
    strcpy(resultado, "0");
    int sc; // Descriptor del socket de escucha 
    char message[800]; // Almacenamos el mensaje en una variable suficientemente grande
    char *token; // Variable que va a ir deserializando el mensaje
    // Copiamos el descriptor pasado como parámetro
    pthread_mutex_lock(&mutex_mensaje);
    // Copiamos el socket desde el que se recibe el mensaje y la ip del cliente
    sc = (*(int *) socket);
    inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
    // Una vez copiado el mensaje, se reanuda el hilo padre
    mensaje_no_copiado = false;
    pthread_cond_signal(&cond_mensaje);
    pthread_mutex_unlock(&mutex_mensaje);

    for(;;){
        int err = recv(sc, (char *) &message, 533, 0);   // recibe la operación
        strcpy(resultado, "0");

        if (err == -1) {
            printf("Reception error\ns> ");
            continue;
        } else if(err == 0){
            break;
        }
        // Cojemos el primer valor del mensaje (operación a realizar)
        token = strtok(message, ",");
        printf("%s\ns> ", token);

        struct usuario_operacion send_rpc;
        send_rpc.operacion = NULL;
        send_rpc.fecha = NULL;
        send_rpc.usuario = NULL;
        send_rpc.file_name = "";
    
        if(strcmp(token, "REGISTER") == 0){
            // Se hace la operación register
            send_rpc.operacion = (char*) malloc((strlen(token) + 1) * sizeof(char));
            strcpy(send_rpc.operacion, token);

            
            token = strtok(NULL, ",");
            send_rpc.fecha = (char*) malloc((strlen(token) + 1) * sizeof(char));
            strcpy(send_rpc.fecha, token);

            
            token = strtok(NULL, ",");
            send_rpc.usuario = (char*) malloc((strlen(token) + 1) * sizeof(char));
            strcpy(send_rpc.usuario, token);

            imprimir_operacion_usuario_1(&send_rpc, client);
            
            pthread_mutex_lock(&mutex_shared_variables);
            
            
            for(int i = 0; i < registered; i++){
                // Se busca si el usuario está registrado anteriormente
                if (strcmp(send_rpc.usuario, users[i]) == 0){
                    strcpy(resultado, "1");
                    if(send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
                        pthread_mutex_unlock(&mutex_shared_variables);
                        pthread_exit(0);
                    }
                }
            }
            if (strcmp(resultado, "0") == 0){
                // Si no está registrado se le registra
                REGISTER(sc, send_rpc.usuario);
            }
            pthread_mutex_unlock(&mutex_shared_variables);

        }
        else if (strcmp(token, "UNREGISTER") == 0){
            send_rpc.operacion = (char*) malloc((strlen(token) + 1) * sizeof(char));
            strcpy(send_rpc.operacion, token);

            token = strtok(NULL, ",");
            send_rpc.fecha = (char*) malloc((strlen(token) + 1) * sizeof(char));
            strcpy(send_rpc.fecha, token);
            
            token = strtok(NULL, ",");
            send_rpc.usuario = (char*) malloc((strlen(token) + 1) * sizeof(char));
            strcpy(send_rpc.usuario, token);

            imprimir_operacion_usuario_1(&send_rpc, client);


            pthread_mutex_lock(&mutex_shared_variables);
            if (registered > 0){
                int index = -1;
                for(int i = 0; i < registered;i++){
                    if (strcmp(users[i], send_rpc.usuario) == 0){
                        index = i;
                    }
                }
                int conectado = -1;
                if(index !=-1){
                    for (int i = 0; i < connected; i++){
                        // Buscamos si el usuario está conectado
                        if (strcmp(conexions[i], token) == 0){
                            conectado = i;
                        }
                    }
                }

                if (index == -1){
                    strcpy(resultado, "1");
                    if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
                        pthread_mutex_unlock(&mutex_shared_variables);
                        pthread_exit(0);
                    }
                }

                // Miramos si el usuario esta conectado para desconectartlo???
                else if (conectado >= 0){
                    strcpy(resultado, "3");
                    if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
                        pthread_mutex_unlock(&mutex_shared_variables);
                        pthread_exit(0);
                    }
                }
                else{
                    UNREGISTER(sc, index);
                }
            }
            else{
                strcpy(resultado, "1");
                if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
                    pthread_mutex_unlock(&mutex_shared_variables);
                    pthread_exit(0);
                }
            }
            pthread_mutex_unlock(&mutex_shared_variables);
            
            
        }
        else if (strcmp(token, "CONNECT") == 0){
            send_rpc.operacion = (char*) malloc((strlen(token) + 1) * sizeof(char));
            strcpy(send_rpc.operacion, token);

            token = strtok(NULL, ",");
            send_rpc.fecha = (char*) malloc((strlen(token) + 1) * sizeof(char));
            strcpy(send_rpc.fecha, token);
            
            token = strtok(NULL, ",");
            send_rpc.usuario = (char*) malloc((strlen(token) + 1) * sizeof(char));
            strcpy(send_rpc.usuario, token);

            imprimir_operacion_usuario_1(&send_rpc, client);

            int index = -1;

            pthread_mutex_lock(&mutex_shared_variables);

            for (int i = 0; i < registered; i ++){
                if (strcmp(users[i], token) == 0){
                    index = i;
                }
            }
            for (int i = 0; i < connected; i++){
                if (strcmp(conexions[i], token) == 0){
                    strcpy(resultado, "2");
                }
            }
            if (index == -1){
                strcpy(resultado, "1");
                if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
                    pthread_mutex_unlock(&mutex_shared_variables);
                    pthread_exit(0);
                }
            }
            
            else if(strcmp(resultado, "2") == 0){
                if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
                    pthread_mutex_unlock(&mutex_shared_variables);
                    pthread_exit(0);
                }
            }
            else{
                CONNECT(sc, token, client_ip);
            }
            pthread_mutex_unlock(&mutex_shared_variables);
        }
        else if (strcmp(token, "DISCONNECT") == 0){
            send_rpc.operacion = (char*) malloc((strlen(token) + 1) * sizeof(char));
            strcpy(send_rpc.operacion, token);

            token = strtok(NULL, ",");
            send_rpc.fecha = (char*) malloc((strlen(token) + 1) * sizeof(char));
            strcpy(send_rpc.fecha, token);
            
            token = strtok(NULL, ",");
            send_rpc.usuario = (char*) malloc((strlen(token) + 1) * sizeof(char));
            strcpy(send_rpc.usuario, token);

            imprimir_operacion_usuario_1(&send_rpc, client);

            int conectado = -1;
            int registrado = -1;

            pthread_mutex_lock(&mutex_shared_variables);

            for(int i = 0; i < registered;i++){
                // Buscamos si el usuario está registrado
                if (strcmp(users[i], token) == 0){
                    registrado = i;
                }
            }
            for (int i = 0; i < connected; i++){
                // Buscamos si el usuario está conectado
                if (strcmp(conexions[i], token) == 0){
                    conectado = i;
                }
            }

            token = strtok(NULL, ",");
            
            if (registrado == -1){
                // Si el usuario no está registrado
                strcpy(resultado, "1");
            }
            else if (conectado == -1){
                // Si el usuario está conectado
                strcpy(resultado, "2");
            }
            else if (token == NULL ){
                strcpy(resultado, "4");
            }
            else if (strcmp(conexions[conectado], token) != 0){
                strcpy(resultado, "4");
            }
            else{
                DISCONNECT(conectado, resultado);
            }
            if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
                    pthread_mutex_unlock(&mutex_shared_variables);
                    pthread_exit(0);
            }
            pthread_mutex_unlock(&mutex_shared_variables);
            
        }
        else if (strcmp(token, "PUBLISH") == 0){
            send_rpc.operacion = (char*) malloc((strlen(token) + 1) * sizeof(char));
            strcpy(send_rpc.operacion, token);

            token = strtok(NULL, ",");
            send_rpc.fecha = (char*) malloc((strlen(token) + 1) * sizeof(char));
            strcpy(send_rpc.fecha, token);
            
            token = strtok(NULL, ",");
            send_rpc.usuario = (char*) malloc((strlen(token) + 1) * sizeof(char));
            strcpy(send_rpc.usuario, token);


            int registrado = -1;

            pthread_mutex_lock(&mutex_shared_variables);

            for (int i = 0; i < registered;i++){
                // Buscamos si el usuario está registrado
                if (strcmp(users[i], token) == 0){
                    registrado = i;
                }
            }
            int conectado = -1;
            for (int i = 0; i < connected; i++){
                // Buscamos no si el usuario está conectado
                if (strcmp(conexions[i], token) == 0){
                    conectado = i;
                }
            }

            int uploaded = -1;
            // Seleccionamos el nombre del fichero
            token = strtok(NULL, ",");
            send_rpc.file_name = (char*) malloc((strlen(token) + 1) * sizeof(char));
            strcpy(send_rpc.file_name, token);
            imprimir_operacion_usuario_1(&send_rpc, client);

            for (int i = 0; i < num_files;i++){
                if ((strcmp(file_names[i], token) == 0) && (strcmp(conexions[conectado], authors[i]) == 0)){
                    uploaded = i;
                }
            }

            if (registrado == -1){
                // Error no está registrado
                strcpy(resultado, "1");
            }
            else if (conectado == -1){
                // Error no está conectado
                strcpy(resultado, "2");
            }
            else if (uploaded != -1){
                // Error el archivo ya está subido
                strcpy(resultado, "3");
            }
            else {
                PUBLISH(token, resultado, conectado);
            }
            pthread_mutex_unlock(&mutex_shared_variables);
            if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
                pthread_exit(0);
            }
        }
        else if (strcmp(token, "DELETE") == 0){
            send_rpc.operacion = (char*) malloc((strlen(token) + 1) * sizeof(char));
            strcpy(send_rpc.operacion, token);

            token = strtok(NULL, ",");
            send_rpc.fecha = (char*) malloc((strlen(token) + 1) * sizeof(char));
            strcpy(send_rpc.fecha, token);
            
            token = strtok(NULL, ",");
            send_rpc.usuario = (char*) malloc((strlen(token) + 1) * sizeof(char));
            strcpy(send_rpc.usuario, token);
      

            pthread_mutex_lock(&mutex_shared_variables);

            if (num_files > 0){
                // Seleccionamos el usuario
                int registrado = -1;
                for (int i = 0; i < registered;i++){
                    // Buscamos si el usuario está registrado
                    if (strcmp(users[i], token) == 0){
                        registrado = i;
                    }
                }
                int conectado = -1;
                for (int i = 0; i < connected; i++){
                    // Buscamos no si el usuario está conectado
                    if (strcmp(conexions[i], token) == 0){
                        conectado = i;
                    }
                }

                int uploaded = -1;
                // Seleccionamos el nombre del fichero
                token = strtok(NULL, ",");
                send_rpc.file_name = (char*) malloc((strlen(token) + 1) * sizeof(char));
                strcpy(send_rpc.file_name, token);
                imprimir_operacion_usuario_1(&send_rpc, client);

                for (int i = 0; i < num_files;i++){
                    if ((strcmp(file_names[i], token) == 0) && (strcmp(conexions[conectado], authors[i]) == 0)){
                        uploaded = i;
                    }
                }

                if (registrado == -1){
                    // Error no está registrado
                    strcpy(resultado, "1");
                }
                else if (conectado == -1){
                    // Error no está conectado
                    strcpy(resultado, "2");
                }
                else if (uploaded == -1){
                    // Error el archivo no está subido
                    strcpy(resultado, "3");
                }
                else { 
                    DELETE(uploaded, resultado);
                }
            }
            else{
                strcpy(resultado, "3");
            }
            pthread_mutex_unlock(&mutex_shared_variables);
            if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
                pthread_exit(0);
            }
        }
        else if (strcmp(token, "LIST_USERS") == 0){
            send_rpc.operacion = (char*) malloc((strlen(token) + 1) * sizeof(char));
            strcpy(send_rpc.operacion, token);

            token = strtok(NULL, ",");
            send_rpc.fecha = (char*) malloc((strlen(token) + 1) * sizeof(char));
            strcpy(send_rpc.fecha, token);
            
            token = strtok(NULL, ",");
            send_rpc.usuario = (char*) malloc((strlen(token) + 1) * sizeof(char));
            strcpy(send_rpc.usuario, token);

            imprimir_operacion_usuario_1(&send_rpc, client);

            int registrado = -1;

            pthread_mutex_lock(&mutex_shared_variables);

            for (int i = 0; i < registered;i++){
                // Buscamos si el usuario está registrado
                if (strcmp(users[i], token) == 0){
                    registrado = i;
                }
            }
            int conectado = -1;
            for (int i = 0; i < connected; i++){
                // Buscamos no si el usuario está conectado
                if (strcmp(conexions[i], token) == 0){
                    conectado = i;
                }
            }
            if (registrado == -1){
                // Error no está registrado
                strcpy(resultado, "1");
                if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
                    pthread_mutex_unlock(&mutex_shared_variables);
                    pthread_exit(0);
            }
            }
            else if (conectado == -1){
                // Error no está conectado
                strcpy(resultado, "2");
                if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
                    pthread_mutex_unlock(&mutex_shared_variables);
                    pthread_exit(0);
            }
            }
            else{
                LIST_USERS(sc);
            }
            pthread_mutex_unlock(&mutex_shared_variables);
        }
        else if (strcmp(token, "LIST_CONTENT") == 0){
            send_rpc.operacion = (char*) malloc((strlen(token) + 1) * sizeof(char));
            strcpy(send_rpc.operacion, token);

            token = strtok(NULL, ",");
            send_rpc.fecha = (char*) malloc((strlen(token) + 1) * sizeof(char));
            strcpy(send_rpc.fecha, token);
            
            token = strtok(NULL, ",");
            send_rpc.usuario = (char*) malloc((strlen(token) + 1) * sizeof(char));
            strcpy(send_rpc.usuario, token);

            imprimir_operacion_usuario_1(&send_rpc, client);


            int registrado = -1;
            int registrado_2 = -1;

            pthread_mutex_lock(&mutex_shared_variables);

            for (int i = 0; i < registered;i++){
                // Buscamos si el usuario está registrado
                if (strcmp(users[i], token) == 0){
                    registrado = i;
                }
            }
            int conectado = -1;
            for (int i = 0; i < connected; i++){
                // Buscamos no si el usuario está conectado
                if (strcmp(conexions[i], token) == 0){
                    conectado = i;
                }
            }
            token = strtok(NULL, ",");
            for (int i = 0; i < registered;i++){
                // Buscamos si el usuario está registrado
                if (strcmp(users[i], token) == 0){
                    registrado_2 = i;
                }
            }
            if (registrado == -1){
                // Error no está registrado
                strcpy(resultado, "1");
                if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
                    pthread_mutex_unlock(&mutex_shared_variables);
                    pthread_exit(0);
                }
            }
            else if (conectado == -1){
                // Error no está conectado
                strcpy(resultado, "2");
                if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
                    pthread_mutex_unlock(&mutex_shared_variables);
                    pthread_exit(0);
                }
            }
            else if (registrado_2 == -1){
                // Error no está conectado
                strcpy(resultado, "3");
                if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
                    pthread_mutex_unlock(&mutex_shared_variables);
                    pthread_exit(0);
                }
            }
            else{
                LIST_CONTENT(sc, token);
            }
            pthread_mutex_unlock(&mutex_shared_variables);

        }
        else if (strcmp(token, "GET_FILE") == 0){
            // Seleccionamos el usuario
            // Comprobaciones usuario principal
            token = strtok(NULL, ",");
            int registrado = -1;
            
            pthread_mutex_lock(&mutex_shared_variables);
            
            for (int i = 0; i < registered;i++){
                // Buscamos si el usuario está registrado
                if (strcmp(users[i], token) == 0){
                    registrado = i;
                }
            }
            int conectado = -1;
            for (int i = 0; i < connected; i++){
                // Buscamos no si el usuario está conectado
                if (strcmp(conexions[i], token) == 0){
                    conectado = i;
                }
            }
            if (registrado == -1){
                // Error no está registrado
                strcpy(resultado, "1");
                if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
                    pthread_mutex_unlock(&mutex_shared_variables);
                    pthread_exit(0);
                }
            }
            else if (conectado == -1){
                // Error no está conectado
                strcpy(resultado, "4");
                if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
                    pthread_mutex_unlock(&mutex_shared_variables);
                    pthread_exit(0);
                }
            }

            // Comprobaciones usuario objetivo
            token = strtok(NULL, ",");
            registrado = -1;
            for (int i = 0; i < registered;i++){
                // Buscamos si el usuario está registrado
                if (strcmp(users[i], token) == 0){
                    registrado = i;
                }
            }
            conectado = -1;
            for (int i = 0; i < connected; i++){
                // Buscamos no si el usuario está conectado
                if (strcmp(conexions[i], token) == 0){
                    conectado = i;
                }
            }
            if (registrado == -1){
                // Error no está registrado
                strcpy(resultado, "3");
                if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
                    pthread_mutex_unlock(&mutex_shared_variables);
                    pthread_exit(0);
                }
            }
            else if (conectado == -1){
                // Error no está conectado
                strcpy(resultado, "4");
                if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
                    pthread_mutex_unlock(&mutex_shared_variables);
                    pthread_exit(0);
                }
            }
            else{
                GET_FILE(sc, conectado);
            }
            pthread_mutex_unlock(&mutex_shared_variables);

        }
    }
    
}



int main(int argc, char *argv[]){
    printf("s> ");
    if (argc != 2){
        exit(-1);
    }
    int puerto = atoi(argv[1]);

    pthread_attr_t t_attr;		// atributos de los threads
    pthread_t thid;
    struct sockaddr_in server_addr;
    socklen_t size;
    int sc;

    sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sd < 0) {
        perror("Error al abrir el socket");
        exit(-1);
    }

    int val = 1;
    int err = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *) &val, sizeof(int));
    if (err < 0) {
        perror("Error al hacer el socket reusable");
        exit(-1);
    }

    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(puerto);

    err = bind(sd, (const struct sockaddr *) &server_addr,sizeof(server_addr));
    if (err == -1) {
        printf("Bind error, port may be in use\ns> ");
        exit(-1);
    }

    err = listen(sd, SOMAXCONN);
    // Esto hace que el socket tenga un numero maximo de peticiones de conexion
    if (err == -1) {
        printf("Error at listen\ns> ");
        return -1;
    }

    pthread_mutex_init(&mutex_shared_variables,NULL);
    pthread_mutex_init(&mutex_mensaje, NULL);
    pthread_cond_init(&cond_mensaje, NULL);
    pthread_attr_init(&t_attr);

    // Se crean los hilos de forma independiente
    pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED);

    while(1) {
        sc = accept(sd, (struct sockaddr *) &client_addr, (socklen_t *) &size); // Genera internamente otro puerto donde recibe los mensajes
        if (sc == -1) {
            printf("Error at accept\ns> ");
            return -1;
        }
        if (pthread_create(&thid, &t_attr, (void *)tratar_mensaje, &sc) == 0) {
            // Se espera a que el thread copie el mensaje
            pthread_mutex_lock(&mutex_mensaje);
            while (mensaje_no_copiado) pthread_cond_wait(&cond_mensaje, &mutex_mensaje);
            mensaje_no_copiado = true;
            pthread_mutex_unlock(&mutex_mensaje);
        }
    }
}



/*
ip de guernica
guernika.lab.inf.uc3m.es
ssh 163.117.142.41
                42
                43
*/