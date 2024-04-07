//
// Created by rubenubuntu on 11/03/24.
//
#include <string.h>
#include <stdio.h>
#include "mensaje.h"
#include "claves.h"


void limpiarBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main() {
    char peticion[MAXSIZE];
    int resultado = -1;

    while(1) {
        printf("Indique la operación a realizar: ");
        scanf("%s", peticion);
        int key;
        char valor_1[MAXSIZE];
        int n_elem;
        double vector[32];

        if (strcmp(peticion,"init") == 0) {
            resultado = init();
        }

        else if (strcmp(peticion, "set_value") == 0){
            printf("Indique la clave sobre la que se desea hacer set_value(key, valor_1, num_elements, vector): key = ");
            if ((scanf("%d", &key)) != 1){
                printf("[ERROR] El valor de key debe ser un int\n");
            } else {
                printf("Indique el valor 1 para set_value(key, valor_1, num_elements, vector): valor_1 = ");
                scanf("%s", valor_1);
                printf("Indique el numero de elementos de valor 2 para set_value(key, valor_1, num_elements, vector): num_elements = ");
                if ((scanf("%d", &n_elem)) != 1){
                    printf("[ERROR] El valor de num_elements debe ser un int\n");
                }  else {
                    for (int i = 0; i < n_elem; i++) {
                        printf("Indique el elemento para set_value(key, valor_1, num_elements, vector): vector[%d] = ",
                               i);
                        if ((scanf("%lf", &vector[i])) != 1) {
                            printf("[ERROR] El valor de los elementos del vector debe ser un double\n");
                            resultado = -1;
                            break;
                        } else { resultado = 0; };
                    }
                    if (resultado == 0) {
                        resultado = set_value(key, valor_1, n_elem, vector);
                    }
                }
            }
        }
        
        else if (strcmp(peticion, "get_value") == 0){
            printf("Get value\n");
            printf("Indique la clave de la tupla que desea obtener: key = ");
            if ((scanf("%d", &key)) != 1){
                printf("[ERROR] La clave introducida no es de tipo int\n");
                resultado = -1;
            }
            else {
                resultado = get_value(key, valor_1, &n_elem, vector);
                if (resultado == 0) {
                    printf("Tupla encontrada:\n");
                    printf("Clave: %d\n", key);
                    printf("Valor1: %s\n", valor_1);
                    printf("Valor2 longitud: %d\n", n_elem);
                    printf("Valor2 vector:\n");
                    printf("[");
                    for (int i = 0; i < n_elem; i++) {
                        printf("%.2lf ", vector[i]);
                        if (i != n_elem - 1) {
                            printf(", ");
                        }
                    }
                    printf("]\n");
                } else {
                    printf("[Error] No se pudo obtener la tupla para la clave %d ,compruebe que existe\n", key);
                }
            }
        }

        else if (strcmp(peticion, "modify_value") == 0){
            printf("Indique la clave sobre la que se desea hacer modify_value(key, valor_1, num_elements, vector): key = ");
            if ((scanf("%d", &key)) != 1){
                printf("[ERROR] El valor de key debe ser un int\n");
            } else {
                printf("Indique el valor 1 para modify_value(key, valor_1, num_elements, vector): valor_1 = ");
                scanf("%s", valor_1);
                printf("Indique el numero de elementos de valor 2 para modify_value(key, valor_1, num_elements, vector): num_elements = ");
                if ((scanf("%d", &n_elem)) != 1){
                    printf("[ERROR] El valor de num_elements debe ser un int\n");
                }  else {
                    for (int i = 0; i < n_elem; i++) {
                        printf("Indique el elemento para hacer modify_value(key, valor_1, num_elements, vector): vector[%d] = ",
                               i);
                        if ((scanf("%lf", &vector[i])) != 1) {
                            printf("[ERROR] El valor de los elementos del vector debe ser un double\n");
                            resultado = -1;
                            break;
                        } else { resultado = 0; };
                    }
                    if (resultado == 0) {
                        resultado = modify_value(key, valor_1, n_elem, vector);
                    }
                }
            }
        }

        else if (strcmp(peticion, "delete_key") == 0){
            printf("Indique la clave sobre la que se desea hacer delete_key(key): key = ");
            if ((scanf("%d", &key)) == 1){
                resultado = delete_key(key);
            }
            else{
                printf("[ERROR] La clave introducida no es de tipo int\n");
            }
        }

        else if (strcmp(peticion, "exist") == 0){
            printf("Indique la clave sobre la que se desea hacer exist(key): key = ");
            if ((scanf("%d", &key)) == 1){
                resultado = exist(key);
            }
            else{
                printf("[ERROR] La clave introducida no es de tipo int\n");
            }
        }

        else {
            resultado = -1;
            printf("[ERROR] Operación no reconocida\n");
        }
        printf("Resultado = %d\n", resultado);
        limpiarBuffer();
    }
}
