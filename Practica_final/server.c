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

char **users;
char **clients_ip;
char **client_socket;
int *conexions;
int registered = 0; // Numero de clientes registrados
int connected = 0;  // Numero de clientes conectados

pthread_mutex_t mutex_shared_variables;
pthread_mutex_t mutex_mensaje;
int mensaje_no_copiado = 1;
pthread_cond_t cond_mensaje;
int sd;
struct sockaddr_in client_addr;


void tratar_mensaje(int  *socket) {
    char client_ip[INET_ADDRSTRLEN];
    char resultado[2];
    strcpy(resultado, "0");
    int sc; // Descriptor del socket de escucha 
    char message[533];
    char *token;
    // Copiamos el descriptor pasado como parámetro
    pthread_mutex_lock(&mutex_mensaje);
    sc = (*(int *) socket);
    inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
    printf("IP = %s\n", client_ip);
    // Una vez copiado el mensaje, se reanuda el hilo padre
    mensaje_no_copiado = false;

    pthread_cond_signal(&cond_mensaje);
    pthread_mutex_unlock(&mutex_mensaje);

    
    for (int i = 0; i < registered; i++){
        printf("Usuario %i = %s\n", i, users[i]);
    }
    for (int i = 0; i < connected; i++){
        printf("Client_socket = %s, Client_ip = %s\n", client_socket[i], clients_ip[i]);
    }


    for(;;){
        int err = recv(sc, (char *) &message, 533, 0);   // recibe la operación
        if (err == -1) {
            printf("Error en recepción\n");
            continue;
        } else if(err == 0){
            break;
        }
        token = strtok(message, ",");
        printf("%s",token);
        printf("Pre\n");
        if(strcmp(token, "REGISTER") == 0){
            printf("Post\n");

            // Se hace la operación register
            int size;
            token = strtok(NULL, ",");
            size = sizeof(token);
            printf("token = %s\n", token);
            for(int i = 0; i < registered; i++){
                if (strcmp(token, users[i]) == 0){
                    strcpy(resultado, "1");
                    printf("usuario existe\n");
                    if(send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
                        pthread_exit(0);
                    }
                }
            }
            if (strcmp(resultado, "0") == 0){
            registered++;
            users = realloc(users, registered * sizeof(char*));
            if (users == NULL) {
                printf("Memory allocation failed\n");
                strcpy(resultado, "2");
                registered--;
                if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
                    pthread_exit(0);
                }
            }
            users[registered - 1] = (char *) malloc((sizeof(token) + 1) * sizeof(char));
            strcpy(users[registered - 1], token);
            if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
                pthread_exit(0);
            }
            }

        }
        else if (strcmp(token, "UNREGISTER") == 0){
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
            users = realloc(users, registered * sizeof(char *));
            if (users == NULL){
                strcpy(resultado, "2");
                if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
                    pthread_exit(0);
                }
            }

            if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
                pthread_exit(0);
            }

        }
        else if (strcmp(token, "CONNECT") == 0){
            token = strtok(NULL, ",");
            int index = -1;

            for (int i = 0; i < registered; i ++){
                if (strcmp(users[i], token)){
                    index = i;
                }
            }
            if (index == -1){
                strcpy(resultado, "1");
                if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
                    pthread_exit(0);
                }
            }
            for (int i = 0; i < connected; i++){
                if (conexions[i] == index){
                    strcpy(resultado, "2");
                }
            }
            if(strcmp(resultado, "2")){
                if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
                    pthread_exit(0);
                }
            }

            else{
                connected++;
                conexions = realloc(conexions, connected * sizeof(int));
                conexions[connected - 1] = index;
                clients_ip = realloc(clients_ip, connected * sizeof(char *));
                clients_ip[connected - 1] = (char *) malloc((sizeof(client_ip) + 1) * sizeof(char));
                strcpy(clients_ip[connected - 1], client_ip);
                token = strtok(NULL, ",");
                client_socket = realloc(client_socket, connected * sizeof(char *));
                client_socket[connected - 1] = (char *) malloc((sizeof(token) + 1) * sizeof(char));
                strcpy(client_socket[connected - 1], client_ip);

                if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
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