/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "ejercicio_evaluable_3.h"

bool_t
xdr_mensaje (XDR *xdrs, mensaje *objp)
{
	register int32_t *buf;

	 if (!xdr_int (xdrs, &objp->key))
		 return FALSE;
	 if (!xdr_string (xdrs, &objp->val_1, 256))
		 return FALSE;
	 if (!xdr_array (xdrs, (char **)&objp->vector.vector_val, (u_int *) &objp->vector.vector_len, 32,
		sizeof (double), (xdrproc_t) xdr_double))
		 return FALSE;
	return TRUE;
}