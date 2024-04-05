
#hacia e l servidor
"""
REGISTER        (UserName)
UNREGISTER          ||
CONNECT             ||
DISCONNECT          ||

PUBLISH         (file_name,description)
DELETE          (file_name)
LIST_USERS      ()
LIST_CONTENT    (UserName)
"""


#hacia otros usuario
"""
GET_FILE        (UserName,remote_file_name,local_file_name)
    remote_file_name: es el nombre del archivo como esta en el servidor 
    local_file_name: es el nombre de la copia que me va a trasmitir y yo voy a guardar
"""