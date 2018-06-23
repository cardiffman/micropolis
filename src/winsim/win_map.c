/*
 * win_map.c
 *
 *  Created on: Jun 9, 2018
 *      Author: Mike
 */
#include "w_map.h"
#include "win_view.h"
#include "s_disast.h"
#include "sim.h"
#include "s_scan.h"
#include "g_map.h"
#include "s_sim.h"
#include "w_x.h"
#include "macros.h"
#include <assert.h>

#define MAX_PIX 256
int max_pix = MAX_PIX;

Ink *NewInk();

struct Pix {
  long color;
  short x, y;
};

#ifdef USE_TCL
extern Tcl_Interp *tk_mainInterp;
#endif
struct Pix pix[MAX_PIX];
#ifdef USE_TCL
Tcl_HashTable MapCmds;
#endif

void WireDrawMap(SimView *view)
{
  int different, x, y, i, last, pts;
  unsigned char *old, *new;
  POINT *points;

  if (!view->x->color) {
    MemDrawMap(view);
    return;
  }

  memcpy(view->other_data, view->data, view->line_bytes * view->m_height); /* XXX: handle depth */
  MemDrawMap(view);

#if 0
  // This has weird notions in it
  old = view->other_data; new = view->data; /* XXX: handle depth */
  different = 0;

  /* Scan the pixels that have changed */
  for (y = 0; y < view->m_height; y++) {
    for (x = 0; x < view->m_width; x++) {
      if (old[x] != new[x]) {
	if (different >= max_pix) {
	  /* Wow, lots of the pixels have changed.
	     Maybe we ought to just do it the hard way. */
	  XPutImage(view->x->dpy, view->pixmap, view->x->gc, view->image,
		    0, 0, 0, 0, view->m_width, view->m_height);
	  return;
	}
	pix[different].color = new[x];
	pix[different].x = x;
	pix[different].y = y;
	different++;
      }
    }
    old += view->line_bytes; new += view->line_bytes; /* XXX: handle depth */
  }

  /* Whew, the images are identical! */
  if (different == 0)
    return;
#endif

#if 0
#if 1

  /* Always draw the whole pixmap, for now. */
  XPutImage(view->x->dpy, view->pixmap, view->x->gc, view->image,
	    0, 0, 0, 0, view->m_width, view->m_height);

#else

  /* TODO: Fix this. I disabled this incremental drawing code for now since it seems to be buggy. */

  /* Sort the changed pixels by their color */
  qsort(pix, different, sizeof (struct Pix), (int (*)())CompareColor);

  /* Draw the points of each color that have changed */
  points = (XPoint *)malloc(sizeof (XPoint) * different);
  last = 0; pts = 0;
  for (i = 0; i <= different; i++) {
    if ((i == different) ||
	(pix[i].color != pix[last].color)) {
      XSetForeground(view->x->dpy, view->x->gc, pix[last].color);
      XDrawPoints(view->x->dpy, view->pixmap, view->x->gc,
		  points, pts, CoordModeOrigin);
      if (i == different)
	break;
      pts = 0;
      last = i;
    }
    points[pts].x = pix[i].x;
    points[pts].y = pix[i].y;
    pts++;
  }
  free(points);

#endif
#endif

}

void DrawMapEditorViews(SimView *view)
{
	assert(!"DrawMapEditorViews");
#if 0
  Pixmap pm = Tk_WindowId(view->tkwin);
  struct SimView *ed;
  int left, right, top, bottom, width, height;
  int mine;

  XSetLineAttributes(view->x->dpy, view->x->gc, 1,
		     LineSolid, CapButt, JoinBevel);

  for (ed = sim->editor; ed != NULL; ed = ed->next) {
    if ((ed->x != view->x) || (ed->show_me == 0))
      continue;

    width = ed->w_width;
    height = ed->w_height;
    left = ed->pan_x - (width / 2);
    top = ed->pan_y - (height / 2);
    right = left + width;
    bottom = top + height;

    left = left * 3 / 16;
    top = top * 3 / 16;
    right = right * 3 / 16;
    bottom = bottom * 3 / 16;
    width = right - left;
    height = bottom - top;

    XSetForeground(view->x->dpy, view->x->gc,
		   view->pixels[COLOR_WHITE]);
    XDrawRectangle(view->x->dpy, pm, view->x->gc,
		   left - 3, top - 3, width + 3, height + 3);

    XSetForeground(view->x->dpy, view->x->gc,
		   view->pixels[COLOR_BLACK]);
    XDrawRectangle(view->x->dpy, pm, view->x->gc,
		   left - 1, top - 1, width + 3, height + 3);

    XSetForeground(view->x->dpy, view->x->gc,
		   view->pixels[COLOR_YELLOW]);
    XDrawRectangle(view->x->dpy, pm, view->x->gc,
		   left - 2, top - 2, width + 3, height + 3);
  }
#endif
}
void DrawMapInk(SimView *view)
{
	assert(!"DrawMapInk");
#if 0
  Pixmap pm = view->pixmap2;
  SimView *v;
  Ink *ink, *ink2 = NewInk();
  int i, X, Y, x, y;

  XSetLineAttributes(view->x->dpy, view->x->gc, 0,
		     LineSolid, CapButt, JoinBevel);

  for (ink = sim->overlay; ink != NULL; ink = ink->next) {
    X = ink->x; x = (X * 3) >>4;
    Y = ink->y; y = (Y * 3) >>4;

    if (ink->length <= 1) {
      XSetForeground(view->x->dpy, view->x->gc,
		     view->pixels[ink->color]);
      XFillArc(view->x->dpy, pm, view->x->gc,
	       ink->x - 1, ink->y - 1, 1, 1, 0, 360 * 64);
    } else {
      StartInk(ink2, x, y);

      for (i = 1; i < ink->length; i++) {
	X += ink->points[i].x; x = (X * 3) >>4;
	Y += ink->points[i].y; y = (Y * 3) >>4;
	AddInk(ink2, x, y);
      }

      XSetForeground(view->x->dpy, view->x->gc,
		     view->pixels[ink->color]);
      XDrawLines(view->x->dpy, pm, view->x->gc,
		 ink2->points, ink2->length, CoordModePrevious);
    }
  }

  FreeInk(ink2);
#endif
}

int DoUpdateMap(SimView *view)
{
  int dx, dy, i;

  view->updates++;

// fprintf(stderr, "UpdateMaps sim_skips %d skips %d skip %d visible %d\n", sim_skips, view->skips, view->skip, view->visible);

  if (!view->visible) {
    return 0;
  }

  if ((!ShakeNow) &&
      (!view->update) &&
      (sim_skips ||
       view->skips)) {
    if (sim_skips) {
      if (sim_skip > 0) {
	return 0;
      }
    } else {
      if (view->skip > 0) {
	--view->skip;
	return 0;
      } else {
	view->skip = view->skips;
      }
    }
  }

  view->update = 0;
  view->skip = 0;

//  view->invalid = 1;

  if (view->invalid || NewMap || ShakeNow) {

    view->invalid = 0;

    switch (view->type) {

    case X_Mem_View:
      MemDrawMap(view);
      break;

    case X_Wire_View:
      WireDrawMap(view);
      break;
    }

  }

  /* XXX: don't do this stuff if just redrawing overlay */

  for (dx = dy = i = 0; i < ShakeNow; i++) {
    dx += Rand(16) - 8;
    dy += Rand(16) - 8;
  }

#if 0
  XCopyArea(view->x->dpy, view->pixmap, view->pixmap2, view->x->gc,
	    dx, dy, view->w_width, view->w_height, 0, 0);
  DrawMapInk(view);

  /* XXX: do this if just redrawing overlay */

  XCopyArea(view->x->dpy, view->pixmap2,
	    Tk_WindowId(view->tkwin), view->x->gc,
	    0, 0, view->w_width, view->w_height, 0, 0);
#endif

  if (view->show_editors) {
    DrawMapEditorViews(view);
  }

  return 1;
}

#ifdef USE_TCL
int
DoMapCmd(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv)
{
  SimView *view = (SimView *) clientData;
  Tcl_HashEntry *ent;
  int result = TCL_OK;
  int (*cmd)();

  if (argc < 2) {
    return TCL_ERROR;
  }

  if ((ent = Tcl_FindHashEntry(&MapCmds, argv[1]))) {
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


void DoNewMap(SimView *view)
{
  sim->maps++; view->next = sim->map; sim->map = view;
/* NewMap = 1; */
  view->invalid = 1;
}

#ifdef USE_TCL
int MapCmdconfigure(SimView *view, Tcl_Interp *interp, int argc, char **argv) { return TCL_OK; }
int MapCmdposition(SimView *view, Tcl_Interp *interp, int argc, char **argv) { return TCL_OK; }
int MapCmdsize(SimView *view, Tcl_Interp *interp, int argc, char **argv) { return TCL_OK; }
int MapCmdMapState(SimView *view, Tcl_Interp *interp, int argc, char **argv) { return TCL_OK; }
int MapCmdShowEditors(SimView *view, Tcl_Interp *interp, int argc, char **argv) { return TCL_OK; }
int MapCmdPanStart(SimView *view, Tcl_Interp *interp, int argc, char **argv) { return TCL_OK; }
int MapCmdPanTo(SimView *view, Tcl_Interp *interp, int argc, char **argv) { return TCL_OK; }
int MapCmdVisible(SimView *view, Tcl_Interp *interp, int argc, char **argv) { return TCL_OK; }
int MapCmdViewAt(SimView *view, Tcl_Interp *interp, int argc, char **argv) { return TCL_OK; }

void map_command_init(void)
{
  int new;
#if 0
  extern int TileViewCmd(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv);

  Tcl_CreateCommand(tk_mainInterp, "mapview", TileViewCmd,
		    (ClientData)NULL/*MainWindow*/, (void (*)()) NULL);
#endif

  Tcl_InitHashTable(&MapCmds, TCL_STRING_KEYS);

#define MAP_CMD(name) HASHED_CMD(Map, name)

  MAP_CMD(configure);
  MAP_CMD(position);
  MAP_CMD(size);
  MAP_CMD(MapState);
  MAP_CMD(ShowEditors);
  MAP_CMD(PanStart);
  MAP_CMD(PanTo);
  MAP_CMD(Visible);
  MAP_CMD(ViewAt);
}
#endif

