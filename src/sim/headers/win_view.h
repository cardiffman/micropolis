/*
 * win_view.h
 *
 *  Created on: Jun 9, 2018
 *      Author: Mike
 */

#ifndef SRC_SIM_HEADERS_WIN_VIEW_H_
#define SRC_SIM_HEADERS_WIN_VIEW_H_
#include <windows.h>
//##include "tclhash.h"
#include "tcl.h"
#include <sys/time.h>

#define X_Mem_View 1
#define X_Wire_View 2

#define Editor_Class 0
#define Map_Class 1

#define VIEW_REDRAW_PENDING 1

typedef struct Ink {
  struct Ink *next;
  int x, y;
  int color;
  int length;
  int maxlength;
  POINT *points;
  int left, top, right, bottom;
  int last_x, last_y;
} Ink;


typedef struct XDisplay {
  struct XDisplay *next;
  int references;
  char *display;
#if 0
  TkDisplay *tkDisplay;
  Display *dpy;
  Screen *screen;
  Window root;
  Visual *visual;
#endif
  int depth;
  int color;
#if 0
  Colormap colormap;
#endif
  int *pixels;
#if 0
  GC gc;
#endif
  int shared;
  unsigned long last_request_read;
  unsigned long request;
#if 0
  XImage *big_tile_image;
  XImage *small_tile_image;
  Pixmap big_tile_pixmap;
  Pixmap **objects;
  GC overlay_gc;
  Pixmap gray25_stipple;
  Pixmap gray50_stipple;
  Pixmap gray75_stipple;
  Pixmap vert_stipple;
  Pixmap horiz_stipple;
  Pixmap diag_stipple;
#endif
} XDisplay;


typedef struct SimView {
  struct SimView *next;
  char *title;
  int type;
  int class;

/* graphics stuff */
  int *pixels;
  int line_bytes;
  int pixel_bytes;
  int depth;
  unsigned char *data;
  int line_bytes8;
  unsigned char *data8;
  int visible;
  int invalid;
  int skips;
  int skip;
  int update;

/* map stuff */
  unsigned char *smalltiles;
  short map_state;
  int show_editors;

/* editor stuff */
  unsigned char *bigtiles;
  short power_type;
  short tool_showing;
  short tool_mode;
  short tool_x, tool_y;
  short tool_x_const, tool_y_const;
  short tool_state;
  short tool_state_save;
  short super_user;
  short show_me;
  short dynamic_filter;
#if 0
  Tk_TimerToken auto_scroll_token;
#endif
  LONG tool_event_time;
  LONG tool_last_event_time;

/* scrolling */
  int w_x, w_y;					/* view window position */
  int w_width, w_height;			/* view window size */
  int m_width, m_height;			/* memory buffer size */
  int i_width, i_height;			/* ideal whole size */
  int pan_x, pan_y;				/* centered in window */
  int tile_x, tile_y, tile_width, tile_height;	/* visible tiles */
  int screen_x, screen_y, screen_width, screen_height; /* visible pixels */

/* tracking */
  int orig_pan_x, orig_pan_y;
  int last_x, last_y;
  int last_button;
  char *track_info;
  char *message_var;

/* window system */
  HWND tkwin;
  Tcl_Interp *interp;
  int flags;

  XDisplay *x;
#ifdef USE_X11
  XShmSegmentInfo *shminfo;
#endif
  short **tiles;
  short **other_tiles;
#if 0
  XImage *image;
  XImage *other_image;
#endif
  unsigned char *other_data;
#if 0
  Pixmap pixmap;
  Pixmap pixmap2;
  Pixmap overlay_pixmap;
  Pixmap overlay_valid;
  XFontStruct *fontPtr;
#endif

/* timing */
  int updates;
  double update_real;
  double update_user;
  double update_system;
  int update_context;

/* auto goto */
  int auto_goto;
  int auto_going;
  int auto_x_goal, auto_y_goal;
  int auto_speed;
  struct SimSprite *follow;

/* sound */
  int sound;

/* configuration */
  int width, height;

/* overlay */
  int show_overlay;
  int overlay_mode;
  struct timeval overlay_time;
} SimView;


typedef struct SimGraph {
  struct SimGraph *next;
  int range;
  int mask;
  HWND tkwin;
  Tcl_Interp *interp;
  int flags;
  XDisplay *x;
  int visible;
  int w_x, w_y;
  int w_width, w_height;
  HBITMAP pixmap;
  int *pixels;
  HFONT fontPtr;
#if 0
  Tk_3DBorder border;
#endif
  int borderWidth;
  int relief;
#if 0
  Tk_TimerToken draw_graph_token;
#endif
} SimGraph;


typedef struct SimDate {
  struct SimDate *next;
  int reset;
  int month;
  int year;
  int lastmonth;
  int lastyear;
#if 0
  Tk_Window tkwin;
  Tcl_Interp *interp;
#endif
  int flags;
  XDisplay *x;
  int visible;
  int w_x, w_y;
  int w_width, w_height;
#if 0
  Pixmap pixmap;
#endif
  int *pixels;
#if 0
  XFontStruct *fontPtr;
  Tk_3DBorder border;
#endif
  int borderWidth;
  int padX;
  int padY;
  int width;
  int monthTab;
  int monthTabX;
  int yearTab;
  int yearTabX;
#if 0
  Tk_TimerToken draw_date_token;
#endif
} SimDate;


typedef struct SimSprite {
  struct SimSprite *next;
  char *name;
  int type;
  int frame;
  int x, y;
  int width, height;
  int x_offset, y_offset;
  int x_hot, y_hot;
  int orig_x, orig_y;
  int dest_x, dest_y;
  int count, sound_count;
  int dir, new_dir;
  int step, flag, control;
  int turn;
  int accel;
  int speed;
} SimSprite;


#ifdef CAM
#include "cam.h"
#endif


typedef struct Person {
  int id;
  char *name;
} Person;


typedef struct Sim {
  int editors;
  SimView *editor;
  int maps;
  SimView *map;
  int graphs;
  SimGraph *graph;
  int dates;
  SimDate *date;
  int sprites;
  SimSprite *sprite;
#ifdef CAM
  int scams;
  SimCam *scam;
#endif
  Ink *overlay;
} Sim;


typedef struct Cmd {
  char *name;
  int (*cmd)();
} Cmd;





#endif /* SRC_SIM_HEADERS_WIN_VIEW_H_ */
