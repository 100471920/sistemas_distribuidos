# Compilador y opciones de compilación
CC = gcc
CFLAGS = -Wall -g -I/usr/include/tirpc -I/usr/include/rpc

# Bibliotecas adicionales
LDLIBS += -lnsl -lpthread -ldl -ltirpc

# Nombre de los archivos
CLIENTE = server
SERVIDOR = rpc_server

# Archivos generados por rpcgen
RPC_FILES = rpc_conexion_svc.c rpc_conexion_clnt.c rpc_conexion_xdr.c rpc_conexion.h

all: $(CLIENTE) $(SERVIDOR)

# Reglas para compilar el cliente
$(CLIENTE): server.c $(RPC_FILES)
	$(CC) $(CFLAGS) -o $(CLIENTE) server.c rpc_conexion_clnt.c rpc_conexion_xdr.c $(LDLIBS)

# Reglas para compilar el servidor
$(SERVIDOR): rpc_server.c $(RPC_FILES)
	$(CC) $(CFLAGS) -o $(SERVIDOR) rpc_server.c rpc_conexion_svc.c rpc_conexion_xdr.c $(LDLIBS)

# Regla para generar archivos rpc con rpcgen
$(RPC_FILES): rpc_conexion.x
	rpcgen -C rpc_conexion.x

clean:
	rm -f $(CLIENTE) $(SERVIDOR) $(RPC_FILES)
