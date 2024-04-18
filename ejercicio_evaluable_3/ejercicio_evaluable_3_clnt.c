/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include <memory.h> /* for memset */
#include "ejercicio_evaluable_3.h"

/* Default timeout can be changed using clnt_control() */
static struct timeval TIMEOUT = { 25, 0 };

enum clnt_stat 
innit_1(void *argp, int *clnt_res, CLIENT *clnt)
{
	return (clnt_call(clnt, INNIT,
		(xdrproc_t) xdr_void, (caddr_t) argp,
		(xdrproc_t) xdr_int, (caddr_t) clnt_res,
		TIMEOUT));
}

enum clnt_stat 
set_value_2(struct mensaje *argp, int *clnt_res, CLIENT *clnt)
{
	return (clnt_call(clnt, SET_VALUE,
		(xdrproc_t) xdr_mensaje, (caddr_t) argp,
		(xdrproc_t) xdr_int, (caddr_t) clnt_res,
		TIMEOUT));
}

enum clnt_stat 
get_value_3(int *argp, struct mensaje *clnt_res, CLIENT *clnt)
{
	return (clnt_call(clnt, GET_VALUE,
		(xdrproc_t) xdr_int, (caddr_t) argp,
		(xdrproc_t) xdr_mensaje, (caddr_t) clnt_res,
		TIMEOUT));
}

enum clnt_stat 
modify_value_4(struct mensaje *argp, int *clnt_res, CLIENT *clnt)
{
	return (clnt_call(clnt, MODIFY_VALUE,
		(xdrproc_t) xdr_mensaje, (caddr_t) argp,
		(xdrproc_t) xdr_int, (caddr_t) clnt_res,
		TIMEOUT));
}

enum clnt_stat 
delete_key_5(int *argp, int *clnt_res, CLIENT *clnt)
{
	return (clnt_call(clnt, DELETE_KEY,
		(xdrproc_t) xdr_int, (caddr_t) argp,
		(xdrproc_t) xdr_int, (caddr_t) clnt_res,
		TIMEOUT));
}

enum clnt_stat 
exist_6(int *argp, int *clnt_res, CLIENT *clnt)
{
	return (clnt_call(clnt, EXIST,
		(xdrproc_t) xdr_int, (caddr_t) argp,
		(xdrproc_t) xdr_int, (caddr_t) clnt_res,
		TIMEOUT));
}