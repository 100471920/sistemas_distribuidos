//
// Created by rubenubuntu on 11/03/24.
//
#include <mqueue.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include "mensaje.h"

int main(){
    mqd_t q_servidor;
    mqd_t q_cliente;
    struct mensaje mensaje;
    char queuename[100];


}