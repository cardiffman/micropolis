/*
 * win_date.c
 *
 *  Created on: Jun 10, 2018
 *      Author: Mike
 */
#include "win_view.h"
#include <windows.h>
#ifdef USE_TCL
#include <tcl.h>
#endif
#include "macros.h"

#ifdef USE_TCL
extern Tcl_Interp* tk_mainInterp;
#endif
extern HWND MainWindow;

#ifdef USE_TCL
Tcl_HashTable DateCmds;

int
DoDateCmd(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv)
{
  SimDate *date = (SimDate *) clientData;
  Tcl_HashEntry *ent;
  int result = TCL_OK;
  int (*cmd)();

  if (argc < 2) {
    return TCL_ERROR;
  }

  if ((ent = Tcl_FindHashEntry(&DateCmds, argv[1]))) {
    cmd = (int (*)())ent->clientData;
    Tcl_Preserve((ClientData) date);
    result = cmd(date, interp, argc, argv);
    Tcl_Release((ClientData) date);
  } else {
    Tcl_AppendResult(interp, "unknown command name: \"",
		     argv[0], " ", argv[1], "\".", (char *) NULL);
    result = TCL_ERROR;
  }
  return result;
}

static int
DateCmdconfigure(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv) { return TCL_OK; }
static int
DateCmdposition(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv) { return TCL_OK; }
static int
DateCmdsize(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv) { return TCL_OK; }
static int
DateCmdVisible(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv) { return TCL_OK; }
static int
DateCmdReset(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv) { return TCL_OK; }
static int
DateCmdSet(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv) { return TCL_OK; }

int
DateViewCmd(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv)
{
	SimDate *date;
	fputs("DateViewCmd: ", stdout);
	fputs(argv[0], stdout);
	for (int arg = 1; arg < argc; arg++)
		fprintf(stdout, " %s", argv[arg]);
	fputc('\n', stdout);

	date = ckalloc(sizeof(SimDate));
	Tcl_CreateCommand(interp, argv[1]/*Tk_PathName(graph->tkwin)*/, DoDateCmd,
			date, NULL);
	return TCL_OK;
}
void date_command_init(void)
{
	Tcl_CreateCommand(tk_mainInterp, "dateview", DateViewCmd,
			MainWindow, NULL);

	Tcl_InitHashTable(&DateCmds, TCL_STRING_KEYS);

#if 1
#define DATE_CMD(name) HASHED_CMD(Date, name)

	DATE_CMD(configure);
	DATE_CMD(position);
	DATE_CMD(size);
	DATE_CMD(Visible);
	DATE_CMD(Reset);
	DATE_CMD(Set);
#endif
}
#endif
