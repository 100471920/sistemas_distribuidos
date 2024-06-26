/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "ejercicio_evaluable_3.h"

int *keys;
char **valores_1;
int* num_elements;
double **vectores;
int num_data = 0; // Numero de elementos almacenados

bool_t
innit_1_svc(void *argp, int *result, struct svc_req *rqstp)
{
    bool_t retval;
    *result = -1;
    free(valores_1);
    free(keys);
    free(vectores);
    free(num_elements);


    *result = 0;
    printf("Operación init realizada\n");


    retval = TRUE;
    return retval;
}

int
innit_prog_1_freeresult (SVCXPRT *transp, xdrproc_t xdr_result, caddr_t result)
{
	xdr_free (xdr_result, result);

	/*
	 * Insert additional freeing code here, if needed
	 */

	return 1;
}

bool_t
set_value_2_svc(struct mensaje *argp, int *result, struct svc_req *rqstp)
{
    bool_t retval;


    retval = TRUE;
    *result = 0;
    for(int i = 0; i < num_data; i++) {
        if (keys[i] == argp->key) {
            *result = -1;
            break;
        }
    }
    if ((argp->vector.vector_len > 32) | (argp->vector.vector_len < 1)){
        *result = -1;
    }


    if(*result != -1){
        num_data++;
        int *temp_keys = realloc(keys, num_data * sizeof(int));
        int *temp_num_elements = realloc(num_elements, num_data * sizeof(int));
        char **temp_valores_1 = NULL;
        temp_valores_1 = realloc(valores_1, num_data * sizeof(char*));
        double **tempo_vectores = realloc(vectores, num_data * sizeof(double*));
        if (temp_keys == NULL) {
            printf("Memory allocation failed\n");
            *result = -1;
            retval = FALSE;
        }
        if (temp_num_elements == NULL) {
            printf("Memory allocation failed\n");
            *result = -1;
            retval = FALSE;
        }
        if (temp_valores_1 == NULL) {
            printf("Memory allocation failed\n");
            *result = -1;
            retval = FALSE;
        }
        if (tempo_vectores == NULL) {
            printf("Memory allocation failed\n");
            *result = -1;
            retval = FALSE;
        }
        // Hacemos la capacidad de la base de datos más grande
        keys = temp_keys;
        valores_1 = temp_valores_1;
        num_elements = temp_num_elements;
        vectores = tempo_vectores;


        // Asignamos los valores al nuevo elemento de la base de datos
        keys[num_data - 1] = argp->key;
        num_elements[num_data - 1] = argp->vector.vector_len;
        valores_1[num_data - 1] = (char *)malloc((sizeof(argp->val_1) + 1) * sizeof(char));
        strcpy(valores_1[num_data - 1], argp->val_1);
        vectores[num_data - 1] = (double *)malloc((argp->vector.vector_len) * sizeof(double));
        for (int i = 0; i < argp->vector.vector_len; i++){
            vectores[num_data - 1][i] = argp->vector.vector_val[i];
        }
    }




    printf("Operación set_value realizada, %d\n", *result);




    return retval;
}


int
set_value_prog_2_freeresult (SVCXPRT *transp, xdrproc_t xdr_result, caddr_t result)
{
	xdr_free (xdr_result, result);

	/*
	 * Insert additional freeing code here, if needed
	 */

	return 1;
}

bool_t
get_value_3_svc(int *argp, struct mensaje *result, struct svc_req *rqstp)
{
    bool_t retval;
    result->key = -1;
    result->val_1 = (char *)malloc((256) * sizeof(char));
    result->vector.vector_val = (double *)malloc((32) * sizeof(double));

    for(int i = 0; i < num_data; i++) {
        if (keys[i] == *argp) {
            // Si se encuentra la clave, copiar los datos al mensaje de respuesta
            result->key = 0; // Indicador de éxito
            result->vector.vector_len = num_elements[i];
            strcpy(result->val_1, valores_1[i]);
            for(int j = 0; j < num_elements[i]; j++) {
                result->vector.vector_val[j] = vectores[i][j];
            }
            break;
        }
    }

    printf("Result (svr) = %d\n", result->key);


    printf("Operación get_value realizada\n");


    retval = TRUE;
    return retval;
}

int
get_value_prog_3_freeresult (SVCXPRT *transp, xdrproc_t xdr_result, caddr_t result)
{
	xdr_free (xdr_result, result);

	/*
	 * Insert additional freeing code here, if needed
	 */

	return 1;
}

bool_t
modify_value_4_svc(struct mensaje *argp, int *result, struct svc_req *rqstp)
{
    bool_t retval;
    *result = -1;
    for (int i = 0; i < num_data; i++){
        if (argp->key == keys[i]){
            valores_1[i] = realloc(valores_1[i], sizeof(argp->val_1) * sizeof(char));
            strcpy(valores_1[i], argp->val_1);
            if (argp->vector.vector_len != num_elements[i]){
                num_elements[i] = argp->vector.vector_len;
                vectores[i] = realloc(vectores[i], sizeof(argp->vector.vector_len) * sizeof(double));
            }
            for (int j = 0; j < argp->vector.vector_len; j++){
                vectores[i][j] = argp->vector.vector_val[j];
            }
            *result = 0;
            break;
        }
    }
    printf("Operación modify_value realizada\n");
    retval = TRUE;
    return retval;
}

int
modify_value_prog_4_freeresult (SVCXPRT *transp, xdrproc_t xdr_result, caddr_t result)
{
	xdr_free (xdr_result, result);

	/*
	 * Insert additional freeing code here, if needed
	 */

	return 1;
}

bool_t
delete_key_5_svc(int *argp, int *result, struct svc_req *rqstp) {
    bool_t retval;
    int index = -1;
    retval = TRUE;

    for (int i = 0; i < num_data; i++) {
        if (keys[i] == *argp) {
            index = i;
        }
    }
    if (index == -1) {
        *result = -1;
        retval = TRUE;
    }
    else {
        free(valores_1[index]);
        free(vectores[index]);

        for (int i = index; i < num_data - 1; i++) {
            keys[i] = keys[i + 1];
            valores_1[i] = valores_1[i + 1];
            num_elements[i] = num_elements[i + 1];
            vectores[i] = vectores[i + 1];
        }
        num_data--;

        keys = realloc(keys, num_data * sizeof(int));
        valores_1 = realloc(valores_1, num_data * sizeof(char *));
        num_elements = realloc(num_elements, num_data * sizeof(int));
        vectores = realloc(vectores, num_data * sizeof(double *));

        *result = 0;
    }
    printf("Operación delete_key realizada\n");

	return retval;
}

int
delete_key_prog_5_freeresult (SVCXPRT *transp, xdrproc_t xdr_result, caddr_t result)
{
	xdr_free (xdr_result, result);

	/*
	 * Insert additional freeing code here, if needed
	 */

	return 1;
}

bool_t
exist_6_svc(int *argp, int *result, struct svc_req *rqstp)
{
	bool_t retval;
    *result = 0;
    for (int i = 0; i < num_data; i++){
        if (keys[i] == *argp){
            *result = 1;
            break;
        }
    }
    retval = TRUE;
	return retval;
}

int
exist_prog_6_freeresult (SVCXPRT *transp, xdrproc_t xdr_result, caddr_t result)
{
	xdr_free (xdr_result, result);

	/*
	 * Insert additional freeing code here, if needed
	 */

	return 1;
}
