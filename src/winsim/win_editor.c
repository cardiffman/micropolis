/*
 * win_editor.c
 *
 *  Created on: Jun 10, 2018
 *      Author: Mike
 */
#include "win_view.h"
#include "sim.h"
#include "macros.h"
int DoOverlay = 2;
int BobHeight = 8;
#ifdef USE_TCL
Tcl_HashTable EditorCmds;
#endif

extern HWND MainWindow;
#ifdef USE_TCL
extern Tcl_Interp *tk_mainInterp;
#endif

int DoUpdateEditor(SimView *view)
{
	  view->invalid = 0;
	  return 1;
}

#ifdef USE_TCL
int
TileViewCmd(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv);

int EditorCmdconfigure(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv) { return TCL_OK; }
int EditorCmdposition(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv) { return TCL_OK; }
int EditorCmdsize(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv) { return TCL_OK; }
int EditorCmdAutoGoto(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv) { return TCL_OK; }
int EditorCmdSound(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv) { return TCL_OK; }
int EditorCmdSkip(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv) { return TCL_OK; }
int EditorCmdUpdate(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv) { return TCL_OK; }
int EditorCmdPan(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv) { return TCL_OK; }
int EditorCmdToolConstrain(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv) { return TCL_OK; }
int EditorCmdToolMode(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv) { return TCL_OK; }
int EditorCmdDoTool(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv) { return TCL_OK; }
int EditorCmdToolDown(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv) { return TCL_OK; }
int EditorCmdToolDrag(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv) { return TCL_OK; }
int EditorCmdToolState(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv) { return TCL_OK; }
int EditorCmdToolUp(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv) { return TCL_OK; }
int EditorCmdPanStart(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv) { return TCL_OK; }
int EditorCmdPanTo(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv) { return TCL_OK; }
int EditorCmdPanBy(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv) { return TCL_OK; }
int EditorCmdTweakCursor(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv) { return TCL_OK; }
int EditorCmdVisible(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv) { return TCL_OK; }
int EditorCmdKeyDown(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv) { return TCL_OK; }
int EditorCmdKeyUp(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv) { return TCL_OK; }
int EditorCmdTileCoord(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv) { return TCL_OK; }
int EditorCmdChalkStart(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv) { return TCL_OK; }
int EditorCmdChalkTo(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv) { return TCL_OK; }
int EditorCmdAutoGoing(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv) { return TCL_OK; }
int EditorCmdAutoSpeed(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv) { return TCL_OK; }
int EditorCmdAutoGoal(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv) { return TCL_OK; }
int EditorCmdSU(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv) { return TCL_OK; }
int EditorCmdShowMe(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv) { return TCL_OK; }
int EditorCmdFollow(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv) { return TCL_OK; }
int EditorCmdShowOverlay(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv) { return TCL_OK; }
int EditorCmdOverlayMode(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv) { return TCL_OK; }
int EditorCmdDynamicFilter(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv) { return TCL_OK; }
void editor_command_init(void)
{
	  Tcl_CreateCommand(tk_mainInterp, "editorview", TileViewCmd, MainWindow, NULL);
	  puts("Command \"editorview\" installed");
#if 0
	  Tcl_InitHashTable(&EditorCmds, TCL_STRING_KEYS);
	#define EDITOR_CMD(name) HASHED_CMD(Editor, name)
	  EDITOR_CMD(configure);
	  EDITOR_CMD(position);
	  EDITOR_CMD(size);
	  EDITOR_CMD(AutoGoto);
	  EDITOR_CMD(Sound);
	  EDITOR_CMD(Skip);
	  EDITOR_CMD(Update);
	  EDITOR_CMD(Pan);
	  EDITOR_CMD(ToolConstrain);
	  EDITOR_CMD(ToolState);
	  EDITOR_CMD(ToolMode);
	  EDITOR_CMD(DoTool);
	  EDITOR_CMD(ToolDown);
	  EDITOR_CMD(ToolDrag);
	  EDITOR_CMD(ToolUp);
	  EDITOR_CMD(PanStart);
	  EDITOR_CMD(PanTo);
	  EDITOR_CMD(PanBy);
	  EDITOR_CMD(TweakCursor);
	  EDITOR_CMD(Visible);
	  EDITOR_CMD(KeyDown);
	  EDITOR_CMD(KeyUp);
	  EDITOR_CMD(TileCoord);
	  EDITOR_CMD(ChalkStart);
	  EDITOR_CMD(ChalkTo);
	  EDITOR_CMD(AutoGoing);
	  EDITOR_CMD(AutoSpeed);
	  EDITOR_CMD(AutoGoal);
	  EDITOR_CMD(SU);
	  EDITOR_CMD(ShowMe);
	  EDITOR_CMD(Follow);
	  EDITOR_CMD(ShowOverlay);
	  EDITOR_CMD(OverlayMode);
	  EDITOR_CMD(DynamicFilter);
#endif
}

int
DoEditorCmd(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv)
{
  SimView *view = (SimView *) clientData;
  Tcl_HashEntry *ent;
  int result = TCL_OK;
  int (*cmd)();

  if (argc < 2) {
    return TCL_ERROR;
  }

  if ((ent = Tcl_FindHashEntry(&EditorCmds, argv[1]))) {
    cmd = (int (*)())ent->clientData;
    Tcl_Preserve((ClientData) view);
    result = cmd(view, interp, argc, argv);
    Tcl_Release((ClientData) view);
  } else {
    Tcl_AppendResult(interp, "unknown command name: \"",
		     argv[0], " ", argv[1], "\".", (char *) NULL);
    result = TCL_ERROR;
  }
  return result;
}
#endif

void DoNewEditor(SimView *view)
{
  sim->editors++; view->next = sim->editor; sim->editor = view;
  view->invalid = 1;
}
