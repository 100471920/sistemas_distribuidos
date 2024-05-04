//
// Created by rubenubuntu on 11/03/24.
//
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>


int *keys;
char **valores_1;
int* num_elements;
double **vectores;
int registered = 0; // Numero de elementos almacenados

pthread_mutex_t mutex_shared_variables;
pthread_mutex_t mutex_mensaje;
int mensaje_no_copiado = 1;
pthread_cond_t cond_mensaje;
int sd;


void tratar_mensaje(int  *socket) {
    char *resultado = (char*)malloc(3 * sizeof(char));
    int sc;    /* socket local */
    int op = 0;
    char message[1807];
    char *token;

    /* el thread copia el mensaje a un mensaje local */
    pthread_mutex_lock(&mutex_mensaje);


    sc = (*(int *) socket);

    // Una vez copiado el mensaje, se reanuda el hilo padre
    mensaje_no_copiado = false;

    pthread_cond_signal(&cond_mensaje);

    pthread_mutex_unlock(&mutex_mensaje);

    while(op != -1) {
        strcpy(resultado, "-1");
        int err = recv(sc, (char *) &message, 1807, 0);   // recibe la operación
        if (err == -1) {
            printf("Error en recepción\n");
            continue;
        } else if(err == 0){
            break;
        }
        // Esta función cambia en message "," por "\0" e internamente sabe en que \0 se encuentra,
        // por lo que luego se puede llamar a la misma funcion con NULL y seguirá en el mismo estado.
        // Esto se hace para poder parsear el mensaje recibido.
        token = strtok(message, ",");
        op = atoi(token);
        if (op < 0 || op > 5){
            printf("Error al leer entero");
            op = 1000;
        }
        // Ejecutar la petición y enviar respuesta
        if (op == 0) {
            // Funcion init no leer mas
            strcpy(resultado, "0");
            registered = 0;

            pthread_mutex_lock(&mutex_shared_variables);

            // Se liberan las variables que simulan la base de datos.
            free(valores_1);
            free(keys);
            free(vectores);
            free(num_elements);

            pthread_mutex_unlock(&mutex_shared_variables);
            printf("Operación init realizada\n");

            if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
                pthread_exit(0);
            }
        }

        else if (op == 1) {
            // Funcion delete_key (leer un parametro)
            int index = -1;
            int key;

            // Leemos la llave introducida
            token = strtok(NULL, ",");
            key = atoi(token);
            pthread_mutex_lock(&mutex_shared_variables);
            for (int i = 0; i < registered; i++) {
                if (keys[i] == key) {
                    index = i;
                }
            }
            if (index == -1) {
                if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
                    pthread_mutex_unlock(&mutex_shared_variables);
                    pthread_exit(0);
                }
                pthread_mutex_unlock(&mutex_shared_variables);
                pthread_exit(0);
            }
            free(valores_1[index]);
            free(vectores[index]);

            // Se recoloca la base de datos para que no se queden espacios intermedios
            for (int i = index; i < registered - 1; i++) {
                keys[i] = keys[i + 1];
                valores_1[i] = valores_1[i + 1];
                num_elements[i] = num_elements[i + 1];
                vectores[i] = vectores[i + 1];
            }
            registered--;

            // Se relocaliza para ocupar el tamaño justo en memoria
            keys = realloc(keys, registered * sizeof(int));
            valores_1 = realloc(valores_1, registered * sizeof(char *));
            num_elements = realloc(num_elements, registered * sizeof(int));
            vectores = realloc(vectores, registered * sizeof(double *));

            strcpy(resultado, "0");
            pthread_mutex_unlock(&mutex_shared_variables);
            printf("Operación delete_key realizada\n");

            if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
                pthread_exit(0);
            }
        }
        else if (op == 2) {
            // set_value (leer mas parametros)
            strcpy(resultado, "0");
            int key;
            char *value_1;
            int n_elem;
            double vector[32];


            // Cada vez que se hace strtok() se pasa a otro valor de entrada, por lo que se van almacenando los valores
            // en variables intermedias
            token = strtok(NULL, ",");
            key = atoi(token);
            token = strtok(NULL, ",");
            n_elem = atoi(token);
            if ((n_elem > 32) | (n_elem < 1)) {
                strcpy(resultado, "-1");
            }
            else{
            for (int i = 0; i < n_elem;i++){
                token = strtok(NULL, ",");
                vector[i] = atof(token);
            }

            value_1 = strtok(NULL, ",");


            }

            // Se reserva memoria para los nuevos valores
            int *temp_keys = realloc(keys, registered * sizeof(int));
            int *temp_num_elements = realloc(num_elements, registered * sizeof(int));
            char **temp_valores_1 = NULL;
            temp_valores_1 = realloc(valores_1, registered * sizeof(char *));
            double **tempo_vectores = realloc(vectores, registered * sizeof(double *));
            if (temp_keys == NULL) {
                printf("Memory allocation failed\n");
                strcpy(resultado, "-1");

            }
            if (temp_num_elements == NULL) {
                printf("Memory allocation failed\n");
                strcpy(resultado, "-1");

            }
            if (temp_valores_1 == NULL) {
                printf("Memory allocation failed\n");
                strcpy(resultado, "-1");

            }
            if (tempo_vectores == NULL) {
                printf("Memory allocation failed\n");
                strcpy(resultado, "-1");
            }

            // Se hace lock por las condiciones de carrera
            pthread_mutex_lock(&mutex_shared_variables);

            // Si cuando se intenta insertar key esta ya existe, da error
            for (int i = 0; i < registered; i++) {
                if (keys[i] == key) {
                    strcpy(resultado, "-1");
                    break;
                }
            }
            // Solo se ejecuta si no ha habido errores previamente
            if (strcmp(resultado, "0") == 0) {
                registered++; // Se añade un dato a la base de datos
                // Hacemos la capacidad de la base de datos más grande
                keys = temp_keys;
                valores_1 = temp_valores_1;
                num_elements = temp_num_elements;
                vectores = tempo_vectores;

                // Asignamos los valores al nuevo elemento de la base de datos
                keys[registered - 1] = key;
                num_elements[registered - 1] = n_elem;
                valores_1[registered - 1] = (char *) malloc((sizeof(value_1) + 1) * sizeof(char));
                strcpy(valores_1[registered - 1], value_1);
                vectores[registered - 1] = (double *) malloc((n_elem) * sizeof(double));
                for (int i = 0; i < n_elem; i++) {
                    vectores[registered - 1][i] = vector[i];
                }
            }

            pthread_mutex_unlock(&mutex_shared_variables);

            printf("Operación set_value realizada\n");
            // Enviar respuesta al cliente
            if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
                pthread_exit(0);
            }
        }
        else if (op == 3) {
            // Función get_value
            strcpy(resultado, "-1");
            int key;
            char* to_send = malloc(1 * sizeof(char)); // Inicialización con malloc
            if (to_send == NULL) {
                printf("Memory allocation failed.\n");
                break;
            }
            to_send[0] = '\0';

            //extraemos la clave que se busca
            token = strtok(NULL, ",");
            key = atoi(token);

            pthread_mutex_lock(&mutex_shared_variables);

            // primero añadimos el primer valor del mensaje que simepre va a ser el resultado
            size_t str_length = strlen(resultado);
            to_send = realloc(to_send, (strlen(to_send) + str_length + 1) * sizeof(char));// +1 por el final del string
            if (to_send == NULL) {
                printf("Memory reallocation failed.\n");
                strcpy(resultado, "-1");
                break;
            }
            sprintf(to_send + strlen(to_send), "%s,", resultado);

            // Buscar valores en la base de datos
            for (int i = 0; i < registered; i++) {
                if (keys[i] == key) {
                    // Si se encuentra la clave, copiar  n_elem
                    int int_length = snprintf(NULL, 0, "%d", num_elements[i]);
                    to_send = realloc(to_send, (strlen(to_send) + int_length + 2) * sizeof(char)); // +2 por el final de string y la coma de separacion
                    if (to_send == NULL) {
                        printf("Memory reallocation failed.\n");
                        strcpy(resultado, "-1");
                        break;
                    }
                    sprintf(to_send + strlen(to_send), "%d,", num_elements[i]);

                    //se copian todos los elementos del vector
                    int double_length;
                    for (int j= 0; j < num_elements[i]; j++){
                        double_length = snprintf(NULL, 0, "%f", vectores[i][j]);
                        to_send = realloc(to_send, (strlen(to_send) + double_length + 2) * sizeof(char));// +2 por el final de string y la coma de separacion
                        if (to_send == NULL) {
                            printf("Memory reallocation failed.\n");
                            strcpy(resultado, "-1");
                            break;
                        }
                        sprintf(to_send + strlen(to_send), "%f,", vectores[i][j]);
                    }
                    str_length = strlen(valores_1[i]);

                    // Finalmente la variable value1
                    to_send = realloc(to_send, (strlen(to_send) + str_length + 1) * sizeof(char));// +1 por el final del string
                    if (to_send == NULL) {
                        printf("Memory reallocation failed.\n");
                        strcpy(resultado, "-1");
                        break;
                    }
                    strcat(to_send, valores_1[i]);
                    strcpy(resultado, "0");
                    break;
                }
            }

            pthread_mutex_unlock(&mutex_shared_variables);

            //si el resultado es correcto se modifica antes de enviar
            if (strcmp(resultado, "0") == 0) {
                // Cambiar los 2 primeros caracteres por lo de un resultado valido
                memcpy(to_send, "00", 2);
            } 

            //se envia el resultado
            err = send(sc, (char*) to_send, strlen(to_send) + 1, 0); // Enviar el mensaje a sc
            if (err == -1) {
                printf("Error al enviar el mensaje\n");
                pthread_exit(0);
            }

            free(to_send); // Liberar la memoria asignada a to_send
            
            printf("Operación get_value realizada\n");
            
        }




        else if (op == 4) {
            // Función modify_value
            // La lectura de parámetros es idéntica a set_value
            int key;
            char *value_1;
            int n_elem;
            double vector[32];


            // Cada vez que se hace strtok() se pasa a otro valor de entrada, por lo que se van almacenando los valores
            // en variables intermedias
            token = strtok(NULL, ",");
            key = atoi(token);
            token = strtok(NULL, ",");
            n_elem = atoi(token);
            if ((n_elem > 32) | (n_elem < 1)) {
                strcpy(resultado, "-1");
            }

            else {
                for (int i = 0; i < n_elem; i++) {
                    token = strtok(NULL, ",");
                    vector[i] = atof(token);
                }

                value_1 = strtok(NULL, ",");

                pthread_mutex_lock(&mutex_shared_variables);

                for (int i = 0; i < registered; i++) {
                    if (key == keys[i]) {
                        free(valores_1[i]);

                        valores_1[i] = malloc((strlen(value_1) + 1) * sizeof(char));

                        strcpy(valores_1[i], value_1);
                        if (n_elem != num_elements[i]) {
                            num_elements[i] = n_elem;
                            free(vectores[i]);
                            vectores[i] = malloc(n_elem * sizeof(double));
                        }
                        for (int j = 0; j < n_elem; j++) {
                            vectores[i][j] = vector[j];
                        }
                        strcpy(resultado, "0");
                        break;
                    }
                }

                pthread_mutex_unlock(&mutex_shared_variables);
                printf("Operación modify_value realizada\n");
                if (send(sc, (char *) resultado, strlen(resultado) + 1, 0) < 0) {
                    pthread_exit(0);
                }
            }
        }
        else if (op == 5) {
            // función exist
            int key;
            char* to_send = malloc(1 * sizeof(char)); // Inicialización con malloc
            if (to_send == NULL) {
                printf("Memory allocation failed.\n");
                break;
            }
            to_send[0] = '\0';

            // obtenemos llave que se busca
            token = strtok(NULL, ",");
            key = atoi(token);

            // El resultado por defecto es 0, si se ecuentra es 1
            strcpy(resultado, "0");
            pthread_mutex_lock(&mutex_shared_variables);

            for (int i = 0; i < registered; i++) {
                if (keys[i] == key) {
                    strcpy(resultado, "1");
                    break;
                }
            }

            pthread_mutex_unlock(&mutex_shared_variables);
            printf("Operación exist realizada\n");

            // Enviar respuesta al cliente
            if (send(sc, (char*) resultado, strlen(resultado) + 1, 0) < 0) {
                pthread_exit(0);
            }
        }
    } 

    pthread_exit(0);

}



int main(int argc, char *argv[]){
    if (argc != 2) {
        printf("Uso: %s <puerto>\n", argv[0]);
        return 1;
    }

    int puerto = atoi(argv[1]);
    // Verificar si el puerto es 0 (significa que no era un número entero válido)
    if (puerto == 0 && argv[1][0] != '0') {
        printf("El puerto debe ser un número entero válido.\n");
        return 1;
    }
    // Verificar si el puerto está dentro del rango válido (0-65535)
    if (puerto < 0 || puerto > 65535) {
        printf("El puerto debe estar en el rango de 0 a 65535.\n");
        return 1;
    }

    pthread_attr_t t_attr;		// atributos de los threads
    pthread_t thid;
    struct sockaddr_in server_addr, client_addr;
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
        if (pthread_create(&thid, &t_attr, (void *)tratar_mensaje, &sc)== 0) {
            // Se espera a que el thread copie el mensaje
            pthread_mutex_lock(&mutex_mensaje);
            while (mensaje_no_copiado) pthread_cond_wait(&cond_mensaje, &mutex_mensaje);
            mensaje_no_copiado = true;
            pthread_mutex_unlock(&mutex_mensaje);
        }
    }
}