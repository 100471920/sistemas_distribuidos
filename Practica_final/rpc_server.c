
/*
    Hacer la conexion rpc con el servidor

*/



#include "rpc_conexion.h"

void *imprimir_operacion_usuario_1_svc(usuario_operacion *argp){
    printf("%s\t\t %s\t\t %s\t\t %s\n", argp->usuario, argp->operacion, argp->file_name, argp->fecha);
    return (void *)NULL;
}

