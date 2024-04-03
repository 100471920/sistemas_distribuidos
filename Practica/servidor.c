#include <stdio.h>      // Para entrada y salida estándar
#include <stdlib.h>     // Para funciones de propósito general, como malloc() y exit()
#include <string.h>     // Para funciones de manipulación de cadenas de caracteres
#include <unistd.h>     // Para funciones y constantes relacionadas con el sistema operativo, como close()
#include <pthread.h>    // Para soporte de hilos (multithreading)
#include <sys/socket.h> // Para funciones relacionadas con sockets
#include <netinet/in.h> // Para estructuras relacionadas con direcciones de sockets
#include <arpa/inet.h>  // Para funciones de conversión de direcciones IP
#include <errno.h>      // Para el manejo de errores

//operaciones que se envian contra el servidor
/*
REGISTER        (UserName)
UNREGISTER          ||
CONNECT             ||
DISCONNECT          ||

PUBLISH         (file_name,description)
DELETE          (file_name)
LIST_USERS      ()
LIST_CONTENT    (UserName)
*/

void tratar_mensaje(){
    int op_num = 0;//que sea lo primero que lee un INT que es el numero de operación
    if (op_num ==1){

    }
}


int main(){
    //cosas que se definen antes del bucle
    while(1){

    }
}











/*
ip de guernica
guernika.lab.inf.uc3m.es
ssh 163.117.142.41
                42
                43
*/