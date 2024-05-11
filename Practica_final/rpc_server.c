
/*
    Hacer la conexion rpc con el servidor

*/



#include "rpc_conexion.h"

void *imprimir_operacion_usuario_1_svc(usuario_operacion *argp, struct svc_req *rqstp){
    printf("ASDASDASDA\n");
    printf("Usuario: %s,\tOperación: %s,\tFecha: %s\n", argp->usuario, argp->operacion, argp->fecha);
    return (void *)NULL;
}

