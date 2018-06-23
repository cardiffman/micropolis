/*
 * w_editor.h
 *
 *  Created on: Jun 3, 2018
 *      Author: Mike
 */

#ifndef SRC_SIM_HEADERS_W_EDITOR_H_
#define SRC_SIM_HEADERS_W_EDITOR_H_
#include "X11/Xlib.h"
#include <tcl.h>
extern int DoOverlay;
extern int BobHeight;

typedef struct SimView SimView;
int DoUpdateEditor(SimView *view);
void DoNewEditor(SimView *view);
void editor_command_init(void);
void ClipTheOverlay(SimView *view);
void DrawTheOverlay(SimView *view, GC gc, Pixmap pm, int color,
	       int top, int bottom, int left, int right,
	       int onoverlay);
void DrawOverlay(SimView *view);
void TimeElapsed(struct timeval *elapsed,
	    struct timeval *start,
	    struct timeval *finish);
void DrawCursor(SimView *view);
void DrawPending(SimView *view);
void DrawOutside(SimView *view);
void HandleAutoGoto(SimView *view);
int
DoEditorCmd(ClientData clientData, Tcl_Interp *interp, int argc, char **argv);




#endif /* SRC_SIM_HEADERS_W_EDITOR_H_ */
