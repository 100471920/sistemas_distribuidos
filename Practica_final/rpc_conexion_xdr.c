/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "rpc_conexion.h"

bool_t
xdr_usuario_operacion (XDR *xdrs, usuario_operacion *objp)
{
	register int32_t *buf;

	 if (!xdr_string (xdrs, &objp->usuario, 256))
		 return FALSE;
	 if (!xdr_string (xdrs, &objp->operacion, 20))
		 return FALSE;
	 if (!xdr_string (xdrs, &objp->fecha, 20))
		 return FALSE;
	 if (!xdr_string (xdrs, &objp->file_name, 256))
		 return FALSE;
	return TRUE;
}
