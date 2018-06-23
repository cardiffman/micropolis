/* w_no_tk.c
 *
 * Micropolis, Unix Version.  This game was released for the Unix platform
 * in or about 1990 and has been modified for inclusion in the One Laptop
 * Per Child program.  Copyright (C) 1989 - 2007 Electronic Arts Inc.  If
 * you need assistance with this program, you may contact:
 *   http://wiki.laptop.org/go/Micropolis  or email  micropolis@laptop.org.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.  You should have received a
 * copy of the GNU General Public License along with this program.  If
 * not, see <http://www.gnu.org/licenses/>.
 * 
 *             ADDITIONAL TERMS per GNU GPL Section 7
 * 
 * No trademark or publicity rights are granted.  This license does NOT
 * give you any right, title or interest in the trademark SimCity or any
 * other Electronic Arts trademark.  You may not distribute any
 * modification of this program using the trademark SimCity or claim any
 * affliation or association with Electronic Arts Inc. or its employees.
 * 
 * Any propagation or conveyance of this program must include this
 * copyright notice and these terms.
 * 
 * If you convey this program (or any modifications of it) and assume
 * contractual liability for the program to recipients of it, you agree
 * to indemnify Electronic Arts for any liability that those contractual
 * assumptions impose on Electronic Arts.
 * 
 * You may not misrepresent the origins of this program; modified
 * versions of the program must be marked as such and not identified as
 * the original program.
 * 
 * This disclaimer supplements the one included in the General Public
 * License.  TO THE FULLEST EXTENT PERMISSIBLE UNDER APPLICABLE LAW, THIS
 * PROGRAM IS PROVIDED TO YOU "AS IS," WITH ALL FAULTS, WITHOUT WARRANTY
 * OF ANY KIND, AND YOUR USE IS AT YOUR SOLE RISK.  THE ENTIRE RISK OF
 * SATISFACTORY QUALITY AND PERFORMANCE RESIDES WITH YOU.  ELECTRONIC ARTS
 * DISCLAIMS ANY AND ALL EXPRESS, IMPLIED OR STATUTORY WARRANTIES,
 * INCLUDING IMPLIED WARRANTIES OF MERCHANTABILITY, SATISFACTORY QUALITY,
 * FITNESS FOR A PARTICULAR PURPOSE, NONINFRINGEMENT OF THIRD PARTY
 * RIGHTS, AND WARRANTIES (IF ANY) ARISING FROM A COURSE OF DEALING,
 * USAGE, OR TRADE PRACTICE.  ELECTRONIC ARTS DOES NOT WARRANT AGAINST
 * INTERFERENCE WITH YOUR ENJOYMENT OF THE PROGRAM; THAT THE PROGRAM WILL
 * MEET YOUR REQUIREMENTS; THAT OPERATION OF THE PROGRAM WILL BE
 * UNINTERRUPTED OR ERROR-FREE, OR THAT THE PROGRAM WILL BE COMPATIBLE
 * WITH THIRD PARTY SOFTWARE OR THAT ANY ERRORS IN THE PROGRAM WILL BE
 * CORRECTED.  NO ORAL OR WRITTEN ADVICE PROVIDED BY ELECTRONIC ARTS OR
 * ANY AUTHORIZED REPRESENTATIVE SHALL CREATE A WARRANTY.  SOME
 * JURISDICTIONS DO NOT ALLOW THE EXCLUSION OF OR LIMITATIONS ON IMPLIED
 * WARRANTIES OR THE LIMITATIONS ON THE APPLICABLE STATUTORY RIGHTS OF A
 * CONSUMER, SO SOME OR ALL OF THE ABOVE EXCLUSIONS AND LIMITATIONS MAY
 * NOT APPLY TO YOU.
 */
#include <windows.h>
#include "w_win.h"
#include "w_date.h"
#include "w_editor.h"
#include "w_graph.h"
#include "w_map.h"
#include "w_resrc.h"
#include "w_sim.h"
#include "w_sound.h"
#include "w_sprite.h"
#include "w_stubs.h"
#include "w_x.h"
#include "s_disast.h"
#include "sim.h"
#include "win_view.h"
#include <stdio.h>
#include <assert.h>

#ifdef MSDOS
#define filename2UNIX(name)	\
    {   char *p; for (p = name; *p; p++) if (*p == '\\') *p = '/'; }
#else
#define filename2UNIX(name)	/**/
#endif

#ifdef USE_TCL
Tcl_Interp *tk_mainInterp = NULL;
Tcl_CmdBuf buffer = NULL;
#endif
#if 0
Tk_TimerToken sim_timer_token = 0;
int sim_timer_idle = 0;
int sim_timer_set = 0;
#endif
HWND MainWindow;
int UpdateDelayed = 0;
#if 0
int AutoScrollEdge = 16;
int AutoScrollStep = 16;
int AutoScrollDelay = 10;
Tk_TimerToken earthquake_timer_token;
#endif
int earthquake_timer_set = 0;
#if 0
int earthquake_delay = 3000;
#endif
int PerformanceTiming;
double FlushTime;
int NeedRest = 0;
int DoEditorCmd(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv);
int DoMapCmd(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv);

typedef struct {
    char *name;			/* Name of command. */
    int (*cmdProc) (ClientData clientData, Tcl_Interp *interp,
	    int argc, const char**argv);
				/* Command procedure. */
} WinCmd;


void InvalidateMaps(void)
{
#if 0
  SimView *view;

//fprintf(stderr, "InvalidateMaps\n");
  for (view = sim->map; view != NULL; view = view->next) {
    view->invalid = 1;
    view->skip = 0;
    EventuallyRedrawView(view);
  }
  sim_skip = 0;
#endif
}


void InvalidateEditors(void)
{
#if 0
  SimView *view;

//fprintf(stderr, "InvalidateEditors\n");
  for (view = sim->editor; view != NULL; view = view->next) {
    view->invalid = 1;
    view->skip = 0;
    EventuallyRedrawView(view);
  }
  sim_skip = 0;
#endif
}

void RedrawMaps(void)
{
#if 0
  SimView *view;

//fprintf(stderr, "RedrawMaps\n");

  for (view = sim->map; view != NULL; view = view->next) {
    view->skip = 0;
    EventuallyRedrawView(view);
  }
  sim_skip = 0;
#endif
}


#if 0
void RedrawEditors(void)
{
#if 0
  SimView *view;

//fprintf(stderr, "RedrawEditors\n");

  for (view = sim->editor; view != NULL; view = view->next) {
    view->skip = 0;
    EventuallyRedrawView(view);
  }
  sim_skip = 0;
#endif
}

#if 0
static void
DisplayTileView(ClientData clientData)
{
  SimView *view = (SimView *) clientData;
  Tk_Window tkwin = view->tkwin;
  Pixmap pm = None;
  Drawable d;

  view->flags &= ~VIEW_REDRAW_PENDING;
  if (view->visible && (tkwin != NULL) && Tk_IsMapped(tkwin)) {
    switch (view->class) {
    case Editor_Class:
      view->skip = 0;
      view->update = 1;
      DoUpdateEditor(view);
      break;
    case Map_Class:
//fprintf(stderr, "DisplayTileView\n");
      view->skip = 0;
      view->update = 1;
      DoUpdateMap(view);
      break;
    }
  }
}
#endif
#endif

/* comefrom:
    ConfigureTileView
    TileViewEventProc expose configure motion
    InvalidateMaps
    EraserTo
    DoSetMapState
    AddInk
    EraserTo
 */

void EventuallyRedrawView(SimView *view)
{
#if 0
  if (!(view->flags & VIEW_REDRAW_PENDING)) {
    Tk_DoWhenIdle(DisplayTileView, (ClientData) view);
    view->flags |= VIEW_REDRAW_PENDING;
  }
#endif
}

void CancelRedrawView(SimView *view)
{
#if 0
  if (view->flags & VIEW_REDRAW_PENDING) {
    Tk_CancelIdleCall(DisplayTileView, (ClientData) view);
  }
#endif
  view->flags &= ~VIEW_REDRAW_PENDING;
}

#if 0
#if 0


#if 0
static void
DelayedMap(ClientData clientData)
{
  while (Tk_DoOneEvent(TK_IDLE_EVENTS) != 0) {
    /* Empty loop body. */
  }
  if (MainWindow == NULL) {
    return;
  }
  Tk_MapWindow(MainWindow);
}
#endif


#endif

static void
MicropolisTimerProc(ClientData clientData)
{
#if 0
  sim_timer_token = NULL;
  sim_timer_set = 0;

  if (NeedRest > 0) {
    NeedRest--;
  }

  if (SimSpeed) {
    sim_loop(1);
    StartMicropolisTimer();
  } else {
    StopMicropolisTimer();
  }

#endif
}


void
ReallyStartMicropolisTimer(ClientData clientData)
{
#if 0
  int delay = sim_delay;
  XDisplay *xd = XDisplays;

  StopMicropolisTimer();

	while (xd != NULL) {
		if ((NeedRest > 0) || ShakeNow || (xd->tkDisplay->buttonWinPtr != NULL)
				|| (xd->tkDisplay->grabWinPtr != NULL)) {
			if (ShakeNow || NeedRest) {
				if (delay < 50000)
					delay = 50000;
			} else {
			}
			break;
		}
		xd = xd->next;
	}

  sim_timer_token =
    Tk_CreateMicroTimerHandler(
      0,
      delay,
      MicropolisTimerProc,
      (ClientData)0);

  sim_timer_set = 1;
#endif
}
#endif

void StartMicropolisTimer(void)
{
#if 0
  if (sim_timer_idle == 0) {
    sim_timer_idle = 1;
    Tk_DoWhenIdle(
      ReallyStartMicropolisTimer,
      NULL);
  }
#endif
}


void StopMicropolisTimer(void)
{
#if 0
  if (sim_timer_idle != 0) {
    sim_timer_idle = 0;
    Tk_CancelIdleCall(
      ReallyStartMicropolisTimer,
      NULL);
  }

  if (sim_timer_set) {
    if (sim_timer_token != 0) {
      Tk_DeleteTimerHandler(sim_timer_token);
      sim_timer_token = 0;
    }
    sim_timer_set = 0;
  }
#endif
}

#if 0
void FixMicropolisTimer(void)
{
#if 0
  if (sim_timer_set) {
    StartMicropolisTimer();
  }
#endif
}


static void
DelayedUpdate(ClientData clientData)
{
//fprintf(stderr, "DelayedUpdate\n");
  UpdateDelayed = 0;
  sim_skip = 0;
  sim_update();
}
#endif

void Kick(void)
{
	// Do sim_update() when idle
}

void
StopEarthquake(void)
{
  ShakeNow = 0;
#if 0
  if (earthquake_timer_set) {
    Tk_DeleteTimerHandler(earthquake_timer_token);
  }
#endif
  earthquake_timer_set = 0;
}

void DoEarthQuake(void)
{
#ifdef USE_TCL
  MakeSound("city", "Explosion-Low");
  Eval("UIEarthQuake");
  ShakeNow++;
  if (earthquake_timer_set) {
#if 0
    Tk_DeleteTimerHandler(earthquake_timer_token);
#endif
  }
#if 0
  Tk_CreateTimerHandler(earthquake_delay, (void (*)())StopEarthquake, (ClientData) 0);
#endif
#endif
  earthquake_timer_set = 1;
}


void StopToolkit(void)
{
#ifdef USE_TCL
  if (tk_mainInterp != NULL) {
    Eval("catch {DoStopMicropolis}");
  }
#endif
}

int Eval(const char *buf)
{
#ifdef USE_TCL
	int result = Tcl_Eval(tk_mainInterp, buf);
	if (result != TCL_OK) {
		const char *errorinfo = Tcl_GetVar(tk_mainInterp, "errorInfo", TCL_GLOBAL_ONLY);
		if (errorinfo == NULL)
			errorinfo = "<no backtrace>";
		fprintf(stderr, "Micropolis: error in TCL code: %s\n%s\n",
				Tcl_GetStringResult(tk_mainInterp), errorinfo);
	}
	return (result);
#else
	return 0;
#endif
}

#if 0
void tk_main(void)
{
  char *p, *msg;
  char buf[20];
  char initCmd[256];
#if 0
  Tk_3DBorder border;

  tk_mainInterp = Tcl_CreateExtendedInterp();
#elif defined(USE_TCL)
  tk_mainInterp = Tcl_CreateInterp();
#endif

#ifdef USE_TCL
  /* XXX: Figure out Extended TCL */
  tclAppName     = "Wish";
  tclAppLongname = "Wish - Tk Shell";
  tclAppVersion  = TK_VERSION;
  Tcl_ShellEnvInit (interp, TCLSH_ABORT_STARTUP_ERR,
                    name,
                    0, NULL,           /* argv var already set  */
                    fileName == NULL,  /* interactive?          */
                    NULL);             /* Standard default file */
#endif

#if 0
  MainWindow = Tk_CreateMainWindow(tk_mainInterp, FirstDisplay, "Micropolis");
  if (MainWindow == NULL) {
    fprintf(stderr, "%s\n", tk_mainInterp->result);
    sim_really_exit(1); // Just sets tkMustExit and ExitReturn
  }
  Tk_SetClass(MainWindow, "Tk");
  Tk_CreateEventHandler(MainWindow, StructureNotifyMask,
			StructureProc, (ClientData) NULL);
/*  Tk_DoWhenIdle(DelayedMap, (ClientData) NULL); */

  Tk_GeometryRequest(MainWindow, 256, 256);
  border = Tk_Get3DBorder(tk_mainInterp, MainWindow, None, "gray75");
  if (border == NULL) {
    Tcl_SetResult(tk_mainInterp, (char *) NULL, TCL_STATIC);
    Tk_SetWindowBackground(MainWindow,
			   WhitePixelOfScreen(Tk_Screen(MainWindow)));
  } else {
    Tk_SetBackgroundFromBorder(MainWindow, border);
  }
  XSetForeground(Tk_Display(MainWindow),
		 DefaultGCOfScreen(Tk_Screen(MainWindow)),
		 BlackPixelOfScreen(Tk_Screen(MainWindow)));
#endif

  sim_command_init();
  map_command_init();
  editor_command_init();
  graph_command_init();
  date_command_init();
  sprite_command_init();

#ifdef CAM
  cam_command_init();
#endif

#if 0
  Tcl_CreateCommand(tk_mainInterp, "piemenu", Tk_PieMenuCmd,
		    (ClientData)MainWindow, (void (*)()) NULL);
  Tcl_CreateCommand(tk_mainInterp, "interval", Tk_IntervalCmd,
		    (ClientData)MainWindow, (void (*)()) NULL);
#endif

  sim = MakeNewSim();

#ifdef USE_TCL
  sprintf(initCmd, "source %s/micropolis.tcl", ResourceDir);
  filename2UNIX(initCmd);
  if (Eval(initCmd)) {
    sim_exit(1); // Just sets tkMustExit and ExitReturn
    goto bail;
  }
#endif

  sim_init();

#ifdef USE_TCL
  buffer = Tcl_CreateCmdBuf();
#endif

#if 0
  if (sim_tty) {
    Tk_CreateFileHandler(0, TK_READABLE, StdinProc, (ClientData) 0);
  }

  { char buf[1024];

    sprintf(buf, "UIStartMicropolis {%s} {%s} {%s}",
	    HomeDir, ResourceDir, HostName);
    filename2UNIX(buf);
    if (Eval(buf) != TCL_OK) {
      sim_exit(1); // Just sets tkMustExit and ExitReturn
      goto bail;
    }
  }

  if (sim_tty) {
    printf("sim:\n");
  }
  fflush(stdout);

  Tk_MainLoop();
#endif

  sim_exit(0); // Just sets tkMustExit and ExitReturn

#if 0
 bail:
#endif

#ifdef USE_TCL
  if (buffer != NULL) {
    Tcl_DeleteCmdBuf(buffer);
  }

  Tcl_DeleteInterp(tk_mainInterp);
#endif
}
#endif

