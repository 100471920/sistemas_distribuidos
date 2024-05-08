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


void tratar_mensaje(int  *socket) {
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
            printf("Error en recepción\n");
            continue;
        } else if(err == 0){
            break;
        }
        // Cojemos el primer valor del mensaje (operación a realizar)
        token = strtok(message, ",");
        if(strcmp(token, "REGISTER") == 0){
            // Se hace la operación register
            int size;
            token = strtok(NULL, ",");
            size = strlen(token);
            for(int i = 0; i < registered; i++){
                // Se busca si el usuario está registrado anteriormente
                if (strcmp(token, users[i]) == 0){
                    strcpy(resultado, "1");
                    printf("usuario existe\n");
                    if(send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
                        pthread_exit(0);
                    }
                }
            }
            if (strcmp(resultado, "0") == 0){
                // Si no está registrado se le registra
                registered++;
                char ** aux = realloc(users, registered * sizeof(char*));
                if (aux == NULL) {
                    // Si falla la reserva de memoria se resetea
                    printf("Memory allocation failed\n");
                    strcpy(resultado, "2");
                    registered--;
                    if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
                        pthread_exit(0);
                    }
                }
                else{
                    users = aux;
                    printf("users = %p\n", users);
                    users[registered - 1] = (char *) malloc((strlen(token) + 1) * sizeof(char));
                    strcpy(users[registered - 1], token);
                    if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
                        pthread_exit(0);
                    }
                }
            }

        }
        else if (strcmp(token, "UNREGISTER") == 0){
            if (registered > 0){
                token = strtok(NULL, ",");
                int index = -1;
                for(int i = 0; i < registered;i++){
                    if (strcmp(users[i], token) == 0){
                        index = i;
                    }
                }
                if (index == -1){
                
                    strcpy(resultado, "1");
                    if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
                        pthread_exit(0);
                    }
                }
                free(users[index]);
                for(int i = index; i < registered - 1; i++){
                    users[i] = users[i + 1];
                }
                registered--;
                if (registered != 0){
                    users = realloc(users, registered * sizeof(char *));
                }
                if (users == NULL){
                    strcpy(resultado, "2");
                    if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
                        pthread_exit(0);
                    }
                }
            }
            else{
                strcpy(resultado, "1");
            }
            if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
                pthread_exit(0);
            }
            
        }
        else if (strcmp(token, "CONNECT") == 0){
            token = strtok(NULL, ",");
            int index = -1;
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
                    pthread_exit(0);
                }
            }
            
            else if(strcmp(resultado, "2") == 0){
                if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
                    pthread_exit(0);
                }
            }
            else{
                connected++;
                conexions = realloc(conexions, connected * sizeof(char*));
                if (conexions == NULL){
                    strcpy(resultado, "3");
                }
                else{
                    conexions[connected - 1] = (char*) malloc ((strlen(token) + 1) * sizeof(char));
                    if (conexions[connected - 1] == NULL){
                        strcpy(resultado, "3");
                    }
                    else{
                        strcpy(conexions[connected - 1], token);
                    }
                }



                clients_ip = realloc(clients_ip, connected * sizeof(char *));
                if (clients_ip == NULL){
                    strcpy(resultado, "3");
                }
                else{
                    clients_ip[connected - 1] = (char *) malloc((strlen(client_ip) + 1) * sizeof(char));
                    if (clients_ip[connected - 1] == NULL){
                        strcpy(resultado, "3");
                    }
                    else{
                        strcpy(clients_ip[connected - 1], client_ip);
                    }
                }


                token = strtok(NULL, ",");
                client_socket = realloc(client_socket, connected * sizeof(char *));
                if (client_socket == NULL){
                    strcpy(resultado, "3");
                }
                else{
                    client_socket[connected - 1] = (char *) malloc((strlen(token) + 1) * sizeof(char));
                    if (client_socket[connected - 1] == NULL){
                        strcpy(resultado, "3");
                    }
                    else{
                        strcpy(client_socket[connected - 1], token);
                    }
                }



                if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
                    pthread_exit(0);
                }
            }
        }
        else if (strcmp(token, "DISCONNECT") == 0){
            int conectado = -1;
            int registrado = -1;
            token = strtok(NULL, ",");
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
            if (registrado == -1){
                // Si el usuario no está registrado
                strcpy(resultado, "1");
            }
            else if (conectado == -1){
                // Si el usuario está conectado
                strcpy(resultado, "2");
            }
            else{
                // Desconectamos al usuario
                free(conexions[conectado]);
                free(clients_ip[conectado]);
                free(client_socket[conectado]);
                for (int i = conectado; i < connected -1; i++){
                    conexions[i] = conexions[i + 1];
                    clients_ip[i] = clients_ip[i + 1];
                    client_socket[i] = client_socket[i + 1];
                }
                connected--;
                if (connected != 0){
                    conexions = realloc(conexions, connected * sizeof(char*));
                    clients_ip = realloc(clients_ip, connected * sizeof(char*));
                    client_socket = realloc(client_socket, connected * sizeof(char*));
                }
                if (conexions == NULL || clients_ip == NULL || client_socket == NULL){
                    strcpy(resultado, "3");
                }
            }
            if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
                    pthread_exit(0);
            }
            
        }
        else if (strcmp(token, "PUBLISH") == 0){
            // Seleccionamos el usuario
            token = strtok(NULL, ",");
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
                // Hacemos espacio para los datos y los almacenamos
                num_files++;
                file_names = realloc(file_names, num_files * sizeof(char*));
                if (file_names == NULL){
                    // Error en asignacion de memoria
                    strcpy(resultado, "4");
                }
                else{
                    file_names[num_files - 1] = (char*) malloc ((strlen(token) + 1) * sizeof(char));
                    if (file_names[num_files - 1] == NULL){
                        // Error en asignacion de memoria
                        strcpy(resultado, "4");
                    }
                    else{
                
                 strcpy(file_names[num_files - 1], token);
                    }
                }                
                authors = realloc(authors, num_files * sizeof(char*));
                if (authors == NULL){
                    // Error en asignacion de memoria
                    strcpy(resultado, "4");
                }
                else{
                    authors[num_files - 1] = (char*) malloc ((sizeof(conexions[conectado]) + 1) * sizeof(char));
                    if (authors[num_files - 1] == NULL){
                        // Error en asignacion de memoria
                        strcpy(resultado, "4");
                    }
                    else{
                        strcpy(authors[num_files - 1], conexions[conectado]);
                    }
                }
                token = strtok(NULL, ",");
                descriptions = realloc(descriptions, num_files * sizeof(char*));
                if (descriptions == NULL){
                    // Error en asignacion de memoria
                    strcpy(resultado, "4");
                }
                else{
                    descriptions[num_files - 1] = (char*) malloc ((strlen(token) + 1) * sizeof(char));
                    if (descriptions[num_files - 1] == NULL){
                        // Error en asignacion de memoria
                        strcpy(resultado, "4");
                    }
                    else{
                        strcpy(descriptions[num_files - 1], token);
                    }
                }
            }
            if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
                pthread_exit(0);
            }
        }
        else if (strcmp(token, "DELETE") == 0){
            if (num_files > 0){
                // Seleccionamos el usuario
                token = strtok(NULL, ",");
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

                    free(file_names[uploaded]);
                    free(descriptions[uploaded]);
                    for (int i = uploaded; i < num_files - 1; i++){
                        file_names[i] = file_names[i + 1];
                        descriptions[i] = descriptions[i + 1];
                    }
                    num_files--;
                    file_names = realloc(file_names, num_files * sizeof(char*));
                    if (file_names == NULL){
                        strcpy(resultado, "4");
                    }
                    descriptions = realloc(descriptions, num_files * sizeof(char*));
                    if (descriptions == NULL){
                        strcpy(resultado, "4");
                    }
                }
            }
            else{
                strcpy(resultado, "3");
            }
            if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
                pthread_exit(0);
            }
        }
        else if (strcmp(token, "LIST_USERS") == 0){
            // Seleccionamos el usuario
            token = strtok(NULL, ",");
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
            if (registrado == -1){
                // Error no está registrado
                strcpy(resultado, "1");
                if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
                pthread_exit(0);
            }
            }
            else if (conectado == -1){
                // Error no está conectado
                strcpy(resultado, "2");
                if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
                pthread_exit(0);
            }
            }
            else{
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
                pthread_exit(0);
            }
            }

        }
        else if (strcmp(token, "LIST_CONTENT") == 0){
            // Seleccionamos el usuario
            token = strtok(NULL, ",");
            int registrado = -1;
            int registrado_2 = -1;
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
                pthread_exit(0);
            }
            }
            else if (conectado == -1){
                // Error no está conectado
                strcpy(resultado, "2");
                if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
                pthread_exit(0);
            }
            }
            else if (registrado_2 == -1){
                // Error no está conectado
                strcpy(resultado, "3");
                if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
                pthread_exit(0);
            }
            }
            else{
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
                printf("TO_SEND = %s\n", to_send);
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
                printf("TO_SEND = %s\n", to_send);
                if (send(sc, (char*) to_send, strlen(to_send) + 1, 0) < 0) {
                pthread_exit(0);
            }
            }

        }else if (strcmp(token, "GET_FILE") == 0){
            // Seleccionamos el usuario
            // Comprobaciones usuario principal
            printf("esta entrando\n");
            for (int i = 0;i < connected;i++){
                printf("%s",conexions[i]);
            }
            token = strtok(NULL, ",");
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
            if (registrado == -1){
                // Error no está registrado
                strcpy(resultado, "1");
                if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
                pthread_exit(0);
            }
            }
            else if (conectado == -1){
                // Error no está conectado
                strcpy(resultado, "4");
                if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
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
                pthread_exit(0);
            }
            }
            else if (conectado == -1){
                // Error no está conectado
                strcpy(resultado, "4");
                if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
                pthread_exit(0);
            }
            }
            else{
                strcpy(resultado, "0");
                // Calculamos lo larga que va a ser nuestra cadena de texto
                int total_size = strlen(resultado);
                total_size++; // Deja espacio para una coma
                total_size += strlen(clients_ip[conectado]);
                total_size++; // Deja espacio para una coma
                total_size += strlen(client_socket[conectado]);
                total_size++; // \0 señalizando final
                
                char to_send[total_size];
                strcpy(to_send, "");
                printf("TO_SEND = %s\n", to_send);
                // Concatenamos el resultado final
                strcat(to_send, "0,");
                strcat(to_send, clients_ip[conectado]);
                strcat(to_send, ",");
                strcat(to_send, client_socket[conectado]);
                    
                printf("TO_SEND = %s\n", to_send);
                if (send(sc, (char*) to_send, strlen(to_send) + 1, 0) < 0) {
                pthread_exit(0);
            }
            }

        }
    }
    
}



int main(int argc, char *argv[]){

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
        printf("Error en bind, revisa que otro programa no este ocupando el puerto selecionado\n");
        exit(-1);
    }

    err = listen(sd, SOMAXCONN);
    // Esto hace que el socket tenga un numero maximo de peticiones de conexion
    if (err == -1) {
        printf("Error en listen\n");
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
        printf("s>");
        if (sc == -1) {
            printf("Error en accept\n");
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