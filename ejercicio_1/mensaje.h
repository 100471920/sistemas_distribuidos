//
// Created by rubenubuntu on 28/02/24.
//

#ifndef DISTRIBUIDOS_MENSAJE_H
#define DISTRIBUIDOS_MENSAJE_H
#define MAXSIZE	256

struct valor_2{
    int n_elem;
    double *vector;
};

struct mensaje{
    int clave;
    char valor_1[MAXSIZE];
    struct valor_2 valor_2;
};

#endif //DISTRIBUIDOS_MENSAJE_H