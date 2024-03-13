//
// Created by rubenubuntu on 11/03/24.
//
#include <mqueue.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include "mensaje.h"
#include "claves.h"

char queuename[100];

int main() {
    printf("Return de claves init: %d\n", init());
    printf("Self PID = %d\n", getpid());
    char peticion;

    while(1) {
        printf("Indique la operación a realizar: ");
        scanf(" %c", &peticion); // Note the space before %c to consume whitespace characters
        if (peticion == 'i') {
            init();
        } else {
            exit_f();
            return -1;
        }
    }

    return 0;
}
