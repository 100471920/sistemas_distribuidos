//
// Created by rubenubuntu on 11/03/24.
//
#include <mqueue.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include "mensaje.h"
#include "claves.h"

char queuename[100];

int main() {
    printf("Self PID = %d\n", getpid());
    char peticion[MAXSIZE];
    int resultado;

    while(1) {
        printf("Indique la operación a realizar: ");
        scanf("%s", peticion);
        printf("Peticion = %s\n", peticion);
        int key;
        char valor_1[MAXSIZE];
        int n_elem;
        double vector[32];

        if (strcmp(peticion,"init") == 0) {
            printf("Exec init");
            resultado = init();
        }

        else if (strcmp(peticion, "set_value") == 0){
            printf("Set value\n");
            printf("Indique la clave sobre la que se desea hacer set_value(key, valor_1, num_elements, vector): key = ");
            if ((scanf("%d", &key)) != 1){
                printf("[ERROR] El valor de kay debe ser un int\n");
            }
            printf("Indique la clave sobre la que se desea hacer set_value(key, valor_1, num_elements, vector): valor_1 = ");
            scanf("%s", valor_1);
            printf("Indique la clave sobre la que se desea hacer set_value(key, valor_1, num_elements, vector): num_elements = ");
            if ((scanf("%d", &n_elem)) != 1){
                printf("[ERROR] El valor de num_elements debe ser un int\n");
            }
            for(int i;i < n_elem;i++){
                printf("Indique la clave sobre la que se desea hacer set_value(key, valor_1, num_elements, vector): vector[%d] = ", i);
                if ((scanf("%lf", &vector[i])) != 1){
                    printf("[ERROR] El valor de num_elements debe ser un double\n");
                    break;
                }
            }
            set_value(key, valor_1, n_elem, vector);

        }
        else if (strcmp(peticion, "get_value") == 0){
            printf("get value\n");
        }
        else if (strcmp(peticion, "modify_value") == 0){printf("mod value\n");}
        else if (strcmp(peticion, "delete_key") == 0){
            printf("Indique la clave sobre la que se desea hacer delete_key(key): key = ");
            if ((scanf("%d", &key)) == 1){
                resultado = delete_key(key);
            }
            else{
                printf("[ERROR] La clave introducida no es de tipo int\n");
            }
        }
        else if (strcmp(peticion, "exist") == 0){printf("exist value\n");}
        else {
            exit_f();
            return -1;
        }
        printf("Resultado = %d\n", resultado);
    }

    return 0;
}
