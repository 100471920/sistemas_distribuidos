from enum import Enum
import argparse
import socket
import threading
import os
import copy

# ****************** ATTRIBUTES ******************




class client:

    # ******************** TYPES *********************
    # *
    # * @brief Return codes for the protocol methods
    class RC(Enum) :
        OK = 0
        ERROR = 1
        USER_ERROR = 2

    # ****************** ATTRIBUTES ******************
    _server = None
    _port = -1
    _reg_username = None
    _username = None
    _connected = False
    _thread = None
    _server_socket = None
    # ******************** METHODS *******************
    


    @staticmethod
    def  register(user):
        #  Write your code here
        try:
            if (user is None):
                print("c> REGISTER FAIL")
                return client.RC.USER_ERROR
            # Conectarse al servidor
            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
                sock.connect((client._server, client._port))

                # Enviar el comando REGISTER
                command = f"REGISTER,{user}\0"
                sock.sendall(command.encode())

                # Recibir la respuesta del servidor
                response = sock.recv(1024).decode().strip()
                # Analizar la respuesta
                if (response == "0\0"):
                    client.reg_username = user
                    print("c> REGISTER OK")
                    return client.RC.OK
                elif (response == "1\0"):
                    print("c> USERNAME IN USE")
                    return client.RC.ERROR
                else:
                    print("c> REGISTER FAIL")
                    return client.RC.USER_ERROR

        except Exception as e:
            print("c> REGISTER FAIL")
            return client.RC.USER_ERROR

   
    @staticmethod
    def  unregister(user) :
        #  Write your code here
        try:
            if (user is None):
                print("c> USER DOES NOT EXIST")
                return client.RC.USER_ERROR
            # Conectarse al servidor
            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
                sock.connect((client._server, client._port))

                # Enviar el comando UNREGISTER
                command = f"UNREGISTER,{user}\0"
                sock.sendall(command.encode())

                # Recibir la respuesta del servidor
                response = sock.recv(1024).decode().strip()
                # Analizar la respuesta
                if (response == "0\0"):
                    client.reg_username = None
                    print("c> UNREGISTER OK")
                    return client.RC.OK
                elif (response == "1\0"):
                    print("c> USER DOES NOT EXIST")
                    return client.RC.ERROR
                else:
                    print("c> UNREGISTER FAIL")
                    return client.RC.USER_ERROR

        except Exception as e:
            print("c> REGISTER FAIL")
            return client.RC.USER_ERROR

    
    @staticmethod
    def  connect(user) :
        #  Write your code here

        # 1 el cliente crea un hilo para atender peticiones get file
            # 1.1 buscar puerto libre
            # 1.2 crear socket llamando ServerSocket 
            # 1.3 que el hilo escuche por ServerSocket

        # 2 enviar cadena CONNECT, <user_name>, str<puerto>
        try:
            if (user is None):
                print("c> CONNECT FAIL, USER DOES NOT EXIST")
                return client.RC.USER_ERROR
            elif not (client._username is None):
                print("c> CONNECT FAIL, ONE CONNECTED USER PER TERMINAL")
                return client.RC.USER_ERROR
                  
            client._server_socket = find_free_port()
            client._connected = True
            client._thread = threading.Thread(target=client.attendpetitions, args=(client._server_socket,))
            client._thread.start()
        
            # Conectarse al servidor
            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
                sock.connect((client._server, client._port))

                # Enviar el comando CONNECT
                command = f"CONNECT,{user},{client._server_socket.getsockname()[1]}\0"
                sock.sendall(command.encode())
                
                # Recibir la respuesta del servidor
                response = sock.recv(1024).decode().strip()
                # Analizar la respuesta
                if (response == "0\0"):
                    client._username = user
                    print("c> CONNECT OK")
                    return client.RC.OK
                elif (response == "1\0"):
                    print("c> CONNECT FAIL, USER DOES NOT EXIST")
                    return client.RC.ERROR
                elif (response == "2\0"):
                    print("c> USER ALREADY CONNECTED")
                    return client.RC.ERROR
                else:
                    print("c> CONNECT FAIL")
                    return client.RC.USER_ERROR

        except Exception as e:
            print("c> CONNECT FAIL")
            return client.RC.USER_ERROR

    
    @staticmethod
    def disconnect(user) :
        #  Write your code here
        try:
            if (user is None):
                print("c> DISCONNECT FAIL, USER DOES NOT EXIST")
                return client.RC.USER_ERROR

            # Conectarse al servidor
            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
                sock.connect((client._server, client._port))

                # Enviar el comando DISCONNECT
                command = f"DISCONNECT,{user}\0"
                sock.sendall(command.encode())

                # Recibir la respuesta del servidor
                response = sock.recv(1024).decode().strip()
                # Analizar la respuesta
                if (response == "0\0"):
                    
                    client._connected = False
                    client._thread.join()
                    client._thread = None
                    client._username = None
                    print("c> DISCONNECT OK")
                    return client.RC.OK
                elif (response == "1\0"):
                    print("c> DISCONNECT FAIL / USER DOES NOT EXIST")
                    return client.RC.ERROR
                elif (response == "2\0"):
                    print("c> DISCONNECT FAIL / USER NOT CONNECTED")
                    return client.RC.ERROR
                else:
                    print("c> DISCONNECT FAIL")
                    return client.RC.USER_ERROR

        except Exception as e:
            print("c> DISCONNECT FAIL ",e)
            return client.RC.USER_ERROR

    @staticmethod
    def publish(fileName,  description) :
        #  Write your code here
        try:
            if (fileName is None or description is None):
                print("c> PUBLISH FAIL")
                return client.RC.USER_ERROR
            elif (client._reg_username is None and client._username is None):
                print("c> PUBLISH FAIL, USER DOES NOT EXIST")
                return client.RC.USER_ERROR
            elif (client._username is None):
                print("c> PUBLISH FAIL, USER NOT CONNECTED")
                return client.RC.USER_ERROR
            
            
            # Que el la longitud de nombre se avalida
            if (len(fileName)>256 or len(description)>256):
                print("c> PUBLISH FAIL")
                return client.RC.ERROR

            # Conectarse al servidor
            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
                sock.connect((client._server, client._port))
                ##

                # Enviar el comando PUBLISH
                command = f"PUBLISH,{client._username},{fileName},{description}\0"
                sock.sendall(command.encode())

                # Recibir la respuesta del servidor
                response = sock.recv(1024).decode().strip()
                # Analizar la respuesta
                if (response == "0\0"):
                    print("c> PUBLISH OK")
                    return client.RC.OK
                elif (response == "1\0"):
                    print("c> PUBLISH FAIL, USER DOES NOT EXIST")
                    return client.RC.ERROR
                elif (response == "2\0"):
                    print("c> PUBLISH FAIL, USER NOT CONNECTED")
                    return client.RC.ERROR
                elif (response == "3\0"):
                    print("c> PUBLISH FAIL, CONTENT ALREADY PUBLISHED")
                    return client.RC.ERROR
                else:
                    print("c> PUBLISH FAIL")
                    return client.RC.USER_ERROR

        except Exception as e:
            print("c> PUBLISH FAIL")
            return client.RC.USER_ERROR

    @staticmethod
    def delete(fileName) :
        #  Write your code here
        try:
            if (fileName is None):
                print("c> DELETE FAIL")
                return client.RC.USER_ERROR
            elif (client._reg_username is None and client._username is None):
                print("c> DELETE FAIL, USER DOES NOT EXIST")
                return client.RC.USER_ERROR
            elif (client._username is None):
                print("c> DELETE FAIL, USER NOT CONNECTED")
                return client.RC.USER_ERROR


            
            # Que el la longitud de nombre se avalida
            if (len(fileName)>256):
                print("c> DELETE FAIL")
                return client.RC.ERROR
            
            # Conectarse al servidor
            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
                sock.connect((client._server, client._port))

                # Enviar el comando DELETE
                command = f"DELETE,{client._username},{fileName}\0"
                sock.sendall(command.encode())

                # Recibir la respuesta del servidor
                response = sock.recv(1024).decode().strip()
                # Analizar la respuesta
                if (response == "0\0"):
                    print("c> DELETE OK")
                    return client.RC.OK
                elif (response == "1\0"):
                    print("c> DELETE FAIL, USER DOES NOT EXIST")
                    return client.RC.ERROR
                elif (response == "2\0"):
                    print("c> DELETE FAIL, USER NOT CONNECTED")
                    return client.RC.ERROR
                elif (response == "3\0"):
                    print("c> DELETE FAIL, CONTENT NOT PUBLISHED")
                    return client.RC.ERROR
                else:
                    print("c> DELETE FAIL")
                    return client.RC.USER_ERROR

        except Exception as e:
            print("c> DELETE FAIL")
            return client.RC.USER_ERROR
    @staticmethod
    def listusers() :
        #  Write your code here
        try:
            if (client._reg_username is None and client._username is None):
                print("c> LIST_USERS FAIL, USER DOES NOT EXIST")
                return client.RC.USER_ERROR
            elif (client._username is None):
                print("c> LIST_USERS FAIL, USER NOT CONNECTED")
                return client.RC.USER_ERROR
        
            # Conectarse al servidor
            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
                sock.connect((client._server, client._port))

                # Enviar el comando LIST_USERS
                command = f"LIST_USERS,{client._username}\0"
                sock.sendall(command.encode())

                # Recibir la respuesta del servidor
                received_data = b""
                while True:
                    data = sock.recv(1)
                    if (data.decode() == "\0"):
                        break
                    received_data += data

                # Decodificar y procesar los datos recibidos
                response = received_data.decode().strip()
                parts = response.split(",")
                # Analizar la respuesta
                if (parts[0] == "0"):
                    print("c> LIST_USERS OK")
                    for i in range(1, len(parts), 3):
                        print(parts[i] + " " + parts[i+1] + " " + parts[i+2])
                        
                    return client.RC.OK
                elif (parts[0] == "1"):
                    print("c> LIST_USERS FAIL, USER DOES NOT EXIST")
                    return client.RC.ERROR
                elif (parts[0] == "2"):
                    print("c> LIST_USERS FAIL, USER NOT CONNECTED")
                    return client.RC.ERROR
                else:
                    print("c> LIST_USERS FAIL")
                    return client.RC.USER_ERROR

        except Exception as e:
            print("c> LIST_USERS FAIL")
            return client.RC.USER_ERROR

    @staticmethod
    def  listcontent(user) :
        #  Write your code here
        try:
            if (user is None):
                print("c> LIST_CONTENT FAIL")
                return client.RC.USER_ERROR
            elif (client._reg_username is None and client._username is None):
                print("c> LIST_CONTENT FAIL, USER DOES NOT EXIST")
                return client.RC.USER_ERROR
            elif (client._username is None):
                print("c> LIST_CONTENT FAIL, USER NOT CONNECTED")
                return client.RC.USER_ERROR

            # Conectarse al servidor
            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
                sock.connect((client._server, client._port))

                # Enviar el comando LIST_CONTENT
                command = f"LIST_CONTENT,{client._username},{user}\0"
                sock.sendall(command.encode())

                # Recibir la respuesta del servidor
                received_data = b""
                while True:
                    data = sock.recv(1)
                    if (data.decode() == "\0"):
                        break
                    received_data += data

                # Decodificar y procesar los datos recibidos
                response = received_data.decode().strip()
                parts = response.split(",")

                # Analizar la respuesta
                # Si no tiene contenido subido pero la operacion es correta
                if (parts[0] == "0" and len(parts) < 3):
                    print("c> LIST_CONTENT OK")
                    return client.RC.OK
                # Si tienen contenido publicado y la operacion es correcta
                elif (parts[0] == "0"):
                    print("c> LIST_CONTENT OK")
                    for i in range(1, len(parts), 2):
                        print("\t" + parts[i] + ' "' + parts[i+1] + '"')
                        
                    return client.RC.OK
                # Fallo usuario emisor no existe
                elif (parts[0] == "1"):
                    print("c> LIST_CONTENT FAIL, USER DOES NOT EXIST")
                    return client.RC.ERROR
                # Fallo usuario emisor no esta conectado
                elif (parts[0] == "2"):
                    print("c> LIST_CONTENT FAIL, USER NOT CONNECTED")
                    return client.RC.ERROR
                # Fallo usuario objetivo no existe
                elif (parts[0] == "3"):
                    print("c> LIST_CONTENT FAIL, REMOTE USER DOES NOT EXIST")
                    return client.RC.ERROR
                # Cualquier otro fallo
                else:
                    print("c> LIST_CONTENT FAIL")
                    return client.RC.USER_ERROR

        except Exception as e:
            print("c> LIST_CONTENT FAIL")
            return client.RC.USER_ERROR

    @staticmethod
    def  getfile(user,  remote_FileName,  local_FileName) :
        #  Write your code here
        try:      
            if (user is None or remote_FileName is None or local_FileName is None):
                print("c> GET_FILE FAIL")
                return client.RC.USER_ERROR  
            elif (client._reg_username is None and client._username is None):
                print("c> GET_FILE FAIL, USER DOES NOT EXIST")
                return client.RC.USER_ERROR
            elif (client._username is None):
                print("c> GET_FILE FAIL, USER NOT CONNECTED")
                return client.RC.USER_ERROR
                
            # Conectarse al servidor para coger el ip y puerto del otro cliente
            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
                sock.connect((client._server, client._port))

                # Enviar el comando GET_FILE
                command = f"GET_FILE,{client._username},{user}\0"
                sock.sendall(command.encode())

                # Recibir la respuesta del servidor
                response = sock.recv(1024).decode().strip()
                parts = response.split(",")

                # Analizar la respuesta
                if (parts[0] == "0"):
                    parts[2]= parts[2].rstrip('\x00')
                    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as remote_sock:
                        remote_sock.connect((parts[1], int(parts[2])))
                        # Enviar el nombre del archivo remoto
                        command = f"GET_FILE,{remote_FileName}\0" 
                        remote_sock.sendall(command.encode())
                        response = remote_sock.recv(1024).decode().strip().split(",")

                        # Abrir el archivo local para escritura binaria
                        if (response[0]== "0"):
                            script_directory = os.path.dirname(os.path.abspath(__file__))
                            local_file_directory = "Ficheros"
                            local_file_path = os.path.join(script_directory, local_file_directory)
                            
                            with open(os.path.join(local_file_path, local_FileName), 'wb') as local_file:
                                # Recibir y escribir los datos del archivo remoto en el archivo local
                                data = ", ".join(response[1:]) #remote_sock.recv(1024)
                                while data:
                                    local_file.write(data.encode())
                                    data = remote_sock.recv(1024)

                            # Imprimir mensaje de éxito
                            print("c> GET_FILE OK")
                            return client.RC.OK
                        elif (response[0]== "1"):
                            print("GET_FILE FAIL / fILE NOT EXIST")
                            return client.RC.ERROR 
                        else:
                            print("c> GET_FILE FAIL")
                            return client.RC.USER_ERROR

                else:
                    print("c> GET_FILE FAIL")
                    return client.RC.USER_ERROR

        except Exception as e:
            print("c> GET_FILE FAIL",e)
             #Si la transferencia no se puede completar, borra el archivo local si existe
            if (os.path.exists(local_FileName)):
                os.remove(local_FileName)
            return client.RC.USER_ERROR
    @staticmethod
    def quit():
        #  Write your code here
        try:
            if (client._username is None):
                print("c> QUIT OK")
                return client.RC.OK
            # Conectarse al servidor
            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
                sock.connect((client._server, client._port))

    
                command = f"DISCONNECT,{client._username}\0"
                sock.sendall(command.encode())

                response= sock.recv(1024).decode().strip()

                # Analizar la respuesta
                if (response == "0\0"):
                    client._connected = False
                    client._thread.join()
                    print("c> QUIT OK")
                    return client.RC.OK
                else:
                    print("c> QUIT FAIL")
                    return client.RC.USER_ERROR

        except Exception as e:
            print("c> QUIT FAIL",e)
            return client.RC.USER_ERROR


    @staticmethod
    def attendpetitions(server_socket):
        try:
            while client._connected:
                # Esperar por una conexión entrante
                server_socket.settimeout(2)
                try:
                    client_socket, addr = server_socket.accept()
                except socket.timeout:
                    # Revisar si se ha desconectado mientras esperaba en accept
                    if not client._connected:
                        break
                    continue

                # Restaurar el tiempo de espera a infinito
                server_socket.settimeout(None)

               # Conexión entrante desde {addr}
                try:
                    
                    script_directory = os.path.dirname(os.path.abspath(__file__))
                    local_file_directory = "Ficheros"
                    local_file_path = os.path.join(script_directory, local_file_directory)
                    
                    # Recibir el comando del cliente
                    command = client_socket.recv(1024).decode().strip()
                    parts = command.split(",")

                    # Analizar el comando
                    if (parts[0] == "GET_FILE"):
                        # Extraer el nombre del archivo local del comando
                        local_file = parts[1].strip().rstrip('\x00')
                        # Verificar si el archivo local existe en la carpeta banco_archivos
                        if (os.path.exists(os.path.join(local_file_path, local_file))):

                            # Abrir y enviar el contenido del archivo al cliente
                            try:
                                with open(os.path.join(local_file_path, local_file), "rb") as file:
                                    enviar=b"0,"
                                    content = file.read(1024)
                                    while content:
                                        enviar+=content
                                        content = file.read(1024)
                                        
                                    client_socket.sendall(enviar)
                                return client.RC.OK
     
                            except Exception:
                                # Enviar código 2 en caso de error durante la transferencia del archivo
                                client_socket.sendall(b"2")
                                return client.RC.USER_ERROR

                        else:
                            # Enviar un mensaje de error al cliente, archivo no existe
                            client_socket.sendall(b"1")
                            return client.RC.ERROR
                    else:
                        # No se envio la operación GET_FILE
                        client_socket.sendall(b"2")
                        return client.RC.ERROR
                
                except Exception:
                    # Ocurrio algun problema con el otro cliente o las comunicaciones
                    continue
                

        except Exception:
            #print("Error en attendpetitions:", str(e))
            # Cerrar el socket del cliente
            client_socket.close()
            return client.RC.USER_ERROR
            
        finally:
            # Cerrar el socket del cliente
            if (not client._connected):
                server_socket.close()
            return client.RC.USER_ERROR


    # *
    # **
    # * @brief Command interpreter for the client. It calls the protocol functions.
    @staticmethod
    def shell():

        while (True) :
            try :
                command = input("c> ")
                line = command.split(" ")
                if (len(line) > 0):

                    line[0] = line[0].upper()

                    if (line[0]=="REGISTER") :
                        if (len(line) == 2) :
                            client.register(line[1])
                        else :
                            print("Syntax error. Usage: REGISTER <userName>")

                    elif(line[0]=="UNREGISTER") :
                        if (len(line) == 2) :
                            client.unregister(line[1])
                        else :
                            print("Syntax error. Usage: UNREGISTER <userName>")

                    elif(line[0]=="CONNECT") :
                        if (len(line) == 2) :
                            client.connect(line[1])
                        else :
                            print("Syntax error. Usage: CONNECT <userName>")
                    
                    elif(line[0]=="PUBLISH") :
                        if (len(line) >= 3) :
                            #  Remove first two words
                            description = ' '.join(line[2:])
                            client.publish(line[1], description)
                        else :
                            print("Syntax error. Usage: PUBLISH <fileName> <description>")

                    elif(line[0]=="DELETE") :
                        if (len(line) == 2) :
                            client.delete(line[1])
                        else :
                            print("Syntax error. Usage: DELETE <fileName>")

                    elif(line[0]=="LIST_USERS") :
                        if (len(line) == 1) :
                            client.listusers()
                        else :
                            print("Syntax error. Use: LIST_USERS")

                    elif(line[0]=="LIST_CONTENT") :
                        if (len(line) == 2) :
                            client.listcontent(line[1])
                        else :
                            print("Syntax error. Usage: LIST_CONTENT <userName>")

                    elif(line[0]=="DISCONNECT") :
                        if (len(line) == 2) :
                            client.disconnect(line[1])
                        else :
                            print("Syntax error. Usage: DISCONNECT <userName>")

                    elif(line[0]=="GET_FILE") :
                        if (len(line) == 4) :
                            client.getfile(line[1], line[2], line[3])
                        else :
                            print("Syntax error. Usage: GET_FILE <userName> <remote_fileName> <local_fileName>")

                    elif(line[0]=="QUIT") :
                        if (len(line) == 1) :
                            client.quit()

                            # Aqui si esta conectado deberia desconectarse

                            break
                        else :
                            print("Syntax error. Use: QUIT")
                    else :
                        print("Error: command " + line[0] + " not valid.")
            except Exception as e:
                print("Exception: " + str(e))

    # *
    # * @brief Prints program usage
    @staticmethod
    def usage() :
        print("Usage: python3 client.py -s <server> -p <port>")


    # *
    # * @brief Parses program execution arguments
    @staticmethod
    def  parseArguments(argv):
        parser = argparse.ArgumentParser()
        parser.add_argument('-s', type=str, required=True, help='Server IP')
        parser.add_argument('-p', type=int, required=True, help='Server Port')
        args = parser.parse_args()

        if (args.s is None):
            parser.error("Usage: python3 client.py -s <server> -p <port>")
            return False

        if ((args.p < 1024) or (args.p > 65535)):
            parser.error("Error: Port must be in the range 1024 <= port <= 65535")
            return False
        
        client._server = args.s
        client._port = args.p

        return True


    # ******************** MAIN *********************
    @staticmethod
    def main(argv) :
        if (not client.parseArguments(argv)) :
            client.usage()
            return

        #  Write code her

        client.shell()
        print("+++ FINISHED +++")
    
def find_free_port():
    server_socket = None
    port = 1024
    while(server_socket == None or port < 65535):
        try:
            server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            server_socket.bind(('localhost', port))  # Enlazar el socket al puerto
            server_socket.listen(1)  # Empezar a escuchar conexiones entrantes
            return server_socket # Salir del bucle si se encontró un puerto disponible
        except OSError:
            port += 1  # El puerto está en uso, intentar con el siguiente

if __name__=="__main__":
    client.main([])
    