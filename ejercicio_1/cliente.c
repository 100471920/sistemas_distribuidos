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

void limpiarBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main() {
    printf("Self PID = %d\n", getpid());
    char peticion[MAXSIZE];
    int resultado = -1;

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
                printf("[ERROR] El valor de key debe ser un int\n");
            } else {
                printf("Indique la clave sobre la que se desea hacer set_value(key, valor_1, num_elements, vector): valor_1 = ");
                scanf("%s", valor_1);
                if (strlen(valor_1) > 255) {
                    printf("La cadena es más larga que 255 caracteres.\n");
                } else {
                    printf("Indique la clave sobre la que se desea hacer set_value(key, valor_1, num_elements, vector): num_elements = ");
                    if ((scanf("%d", &n_elem)) != 1){
                        printf("[ERROR] El valor de num_elements debe ser un int\n");
                    } else if(n_elem>32 || n_elem<1){
                        printf("el numero de elemento posibles en el vector es de entre 1 y 32");
                    } else {
                        for(int i = 0;i < n_elem;i++){
                            printf("Indique la clave sobre la que se desea hacer set_value(key, valor_1, num_elements, vector): vector[%d] = ", i);
                            if ((scanf("%lf", &vector[i])) != 1){
                                printf("[ERROR] El valor de los elementos del vector debe ser un double\n");
                                resultado = -1;
                                break;
                            } else {resultado = 0;};
                        }
                        if(resultado == 0){
                            resultado = set_value(key, valor_1, n_elem, vector);
                            if(resultado < 0){
                                printf("algo fallo, comprueba que no existe una tupla con la clave que uso");
                            }
                        }
                    }
                }    
            }
        }
        
        else if (strcmp(peticion, "get_value") == 0){
            printf("Get value\n");
            printf("Indique la clave de la tupla que desea obtener: key = ");
            if ((scanf("%d", &key)) != 1){
                printf("[ERROR] La clave introducida no es de tipo int\n");
            }
            else {
                char value1[MAXSIZE];
                int N_value2;
                double V_value2[32];
                resultado = get_value(key, value1, &N_value2, V_value2);
                if (resultado == 0) {
                    printf("Tupla encontrada:\n");
                    printf("Clave: %d\n", key);
                    printf("Valor1: %s\n", value1);
                    printf("Valor2 longitud: %d\n", N_value2);
                    printf("Valor2 vector:\n");
                    for (int i = 0; i < N_value2; i++) {
                        printf("%.2lf ", V_value2[i]);
                    }
                    printf("\n");
                } else {
                    printf("Error: No se pudo obtener la tupla para la clave %d ,compruebe que existe", key);
                }
            }
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

        else if (strcmp(peticion, "salir") == 0){
            exit_f();
            return -1;
        }
        else {
            resultado = -1;
            printf("operacion no reconocida pruebe otra vez\nse reconocen las sigueites operaciones\n");
            printf("set_value\nget_value\nmodify_value\ndelete_key\nexist\nsalir");
        }
        printf("\nResultado = %d\n", resultado);
        limpiarBuffer();
    }

    return 0;
}
