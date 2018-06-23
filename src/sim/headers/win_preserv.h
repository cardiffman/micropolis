/*
 * win_preserv.h
 *
 *  Created on: Jun 11, 2018
 *      Author: Mike
 */

#ifndef SRC_SIM_HEADERS_WIN_PRESERV_H_
#define SRC_SIM_HEADERS_WIN_PRESERV_H_

#ifndef _CLIENTDATA
typedef int *ClientData;
#define _CLIENTDATA
#endif
typedef void (Win_FreeProc) (ClientData clientData);

void
Tk_Preserve(
    ClientData clientData);	/* Pointer to malloc'ed block of memory. */
void
Tk_Release(
    ClientData clientData);	/* Pointer to malloc'ed block of memory. */

void
Tk_EventuallyFree(
    ClientData clientData,	/* Pointer to malloc'ed block of memory. */
    Win_FreeProc *freeProc);	/* Procedure to actually do free. */




#endif /* SRC_SIM_HEADERS_WIN_PRESERV_H_ */
