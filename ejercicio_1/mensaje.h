//
// Created by rubenubuntu on 28/02/24.
//

#ifndef DISTRIBUIDOS_MENSAJE_H
#define DISTRIBUIDOS_MENSAJE_H
#define MAXSIZE	256 // 255 caracteres + \0

struct mensaje{
    int op;
    char cola_cliente[100];
    int clave;
    char valor_1[MAXSIZE];
    int n_elem;
    double vector[32];
};

#endif //DISTRIBUIDOS_MENSAJE_H
