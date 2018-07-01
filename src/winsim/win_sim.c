/*
 * win_sim.c
 *
 *  Created on: Jun 9, 2018
 *      Author: Mike
 */
#include <windowsx.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/stat.h>
#include "sim.h"
#include "w_util.h"
#include "w_win.h"
#include "w_stubs.h"
#include "s_init.h"
#include "s_gen.h"
#include "g_map.h"
#include "w_budget.h"
#include "w_graph.h"
#include "s_alloc.h"
#include "w_sound.h"
#include "w_eval.h"
#include "s_msg.h"
#include "w_update.h"
#include "w_resrc.h"
#include "win_view.h"
#include "s_scan.h"
#include "s_disast.h"
#include "w_map.h"
#include "w_editor.h"
#include "w_x.h"
#include "w_sim.h"
#include "w_date.h"
#include "w_sprite.h"
#include "s_sim.h"
#include "XpmFileData.h"

#define COPY(FROM, TO) \
  TO = malloc(strlen(FROM) + 1); \
  strcpy(TO, FROM);


#define TESTDIR(DIR, NAME) \
  if ((stat(DIR, &statbuf) == -1) || \
      !(S_ISDIR(statbuf.st_mode))) { \
    fprintf(stderr, \
	    "Can't find the directory \"%s\"!\n", DIR); \
    fprintf(stderr, \
	    "The environment variable \"%s\" should name a directory.\n", \
	    NAME); \
    lost = 1; \
  }

char *MicropolisVersion = "4.0";
Sim *sim = NULL;
int sim_loops = 0;
int sim_delay = 50;
int sim_skips = 0;
int sim_skip = 0;
int sim_paused = 0;
int sim_paused_speed = 3;
int sim_tty = 0;
#ifdef CAM
int sim_just_cam = 0;
#endif
int heat_steps = 0;
int heat_flow = -7;
int heat_rule = 0;
int heat_wrap = 3;
struct timeval start_time, now_time, beat_time, last_now_time;
char *CityFileName = NULL;
int Startup = 0;
int StartupGameLevel = 0;
char *StartupName = NULL;
int WireMode = 0;
int MultiPlayerMode = 0;
int SugarMode = 0;
int TilesAnimated = 0;
int DoAnimation = 1;
int DoMessages = 1;
int DoNotices = 1;
char *FirstDisplay = NULL;
int ExitReturn = 0;

static int triedToBailOnce = 0;

void
SignalExitHandler()
{
  if (triedToBailOnce) {
    exit(-1);
  } else {
    triedToBailOnce = 1;
    fprintf(stderr, "\nMicropolis has been terminated by a signal.\n");
    fprintf(stderr, "Pick a window -- you're leaving!\n\n");
    fflush(stderr);
    sim_really_exit(-1);
  }
}

void signal_init(void)
{
  signal(SIGHUP, (void (*)())SignalExitHandler);
  signal(SIGINT, (void (*)())SignalExitHandler);
  signal(SIGQUIT, (void (*)())SignalExitHandler);
  signal(SIGTERM, (void (*)())SignalExitHandler);
}

void
env_init(void)
{
#if 1
  char dir[256];
  char *s;
  struct stat statbuf;
  int lost = 0;

  if ((s = getenv("SIMHOME")) == NULL) {
    s = ".";
  }
  COPY(s, HomeDir);
  TESTDIR(HomeDir, "$SIMHOME");

  sprintf(dir, "%s/res/", HomeDir);
  COPY(dir, ResourceDir);
  //TESTDIR(ResourceDir, "$SIMHOME/res");

#if 0
  { extern char *TCL_Library, *TK_Library;
    TCL_Library = TK_Library = ResourceDir;
  }
#endif

  if (lost) {
    fprintf(stderr,
	    "Please check the environment or reinstall Micropolis and try again! Sorry!\n");
    sim_exit(1); // Just sets tkMustExit and ExitReturn
    return;
  }
#endif

  gettimeofday(&now_time, NULL);
  last_now_time = now_time;
}


void
sim_init(void)
{
  gettimeofday(&start_time, NULL);
  gettimeofday(&beat_time, NULL);

  signal_init();

  UserSoundOn = 1;
  MustUpdateOptions = 1;
  HaveLastMessage = 0;
  ScenarioID = 0;
  StartingYear = 1900;
  tileSynch = 0x01;
  sim_skips = sim_skip = 0;
  autoGo = 1;
  CityTax = 7;
  CityTime = 50;
  NoDisasters = 0;
  PunishCnt = 0;
  autoBulldoze = 1;
  autoBudget = 1;
  MesNum = 0;
  LastMesTime = 0;
  flagBlink = 1;
  SimSpeed = 3;
#ifdef HAVE_EVAL
  ChangeEval();
#endif
  MessagePort = 0;
  MesX = 0;
  MesY = 0;
  sim_paused = 0;
  sim_loops = 0;
  InitSimLoad = 2;
  //tkMustExit = 0;
  ExitReturn = 0;

#ifdef HAVE_SOUND
  InitializeSound();
#endif
  initMapArrays(); // s_alloc
#if 1 //ifdef HAVE_GRAPHS
  initGraphs();
#endif
#ifdef HAVE_BUDGET
  InitFundingLevel();
#endif
#if 0
  loadObjectData();
#endif
  setUpMapProcs(); // g_map
#ifdef HAVE_DISASTERS
  StopEarthquake(); // s_disast
#endif
  ResetMapState(); // s_init
  ResetEditorState(); // s_init
  ClearMap(); // s_gen
  InitWillStuff();
#ifdef HAVE_UPDATE
  SetFunds(5000);
#endif
  SetGameLevelFunds(StartupGameLevel); // util
  setSpeed(0); // util
  setSkips(0); // util
}
void sim_exit(int val)
{
  //tkMustExit = 1;
  ExitReturn = val;
}

void sim_really_exit(int val)
{
  DoStopMicropolis();

  exit(val);
}

void
sim_timeout_loop(short doSim)
{
  if (SimSpeed) {
    sim_loop(doSim);
  }
  DoTimeoutListen();
}

short *CellSrc = NULL;
short *CellDst = NULL;

#define SRCCOL (WORLD_Y + 2)
#define DSTCOL WORLD_Y

#define CLIPPER_LOOP_BODY(CODE) \
    src = CellSrc; dst = CellDst; \
    for (x = 0; x < WORLD_X;) { \
      short nw, n, ne, w, c, e, sw, s, se; \
      \
      src = CellSrc + (x * SRCCOL); dst = CellDst + (x * DSTCOL); \
      w = src[0]; c = src[SRCCOL]; e = src[2 * SRCCOL]; \
      sw = src[1]; s = src[SRCCOL + 1]; se = src[(2 * SRCCOL) + 1]; \
      \
      for (y = 0; y < WORLD_Y; y++) { \
        nw = w; w = sw; sw = src[2]; \
	n = c; c = s; s = src[SRCCOL + 2]; \
	ne = e; e = se; se = src[(2 * SRCCOL) + 2]; \
	{ CODE } \
	src++; dst++; \
      } \
      x++; /* src += SRCCOL - 3; dst += DSTCOL - 1; */ \
      src = CellSrc + ((x + 1) * SRCCOL) - 3; dst = CellDst + ((x + 1) * DSTCOL) - 1; \
      \
      nw = src[1]; n = src[SRCCOL + 1]; ne = src[(2 * SRCCOL) + 1]; \
      w = src[2]; c = src[SRCCOL + 2]; e = src[(2 * SRCCOL) + 2]; \
      \
      for (y = WORLD_Y - 1; y >= 0; y--) { \
        sw = w; w = nw; nw = src[0]; \
        s = c; c = n; n = src[SRCCOL]; \
        se = e; e = ne; ne = src[2 * SRCCOL]; \
	{ CODE } \
	src--; dst--; \
      } \
      x++; /* src += SRCCOL + 3; dst += DSTCOL + 1; */ \
    }

void
sim_heat(void)
{
  int x, y, l, r, u, d;
  static int a = 0;
  short *src, *dst;
  register int fl = heat_flow;

  if (CellSrc == NULL) {
    CellSrc = (short *)malloc((WORLD_X + 2) * (WORLD_Y + 2) * sizeof (short));
    CellDst = &Map[0][0];
  }

  src = CellSrc + SRCCOL + 1;
  dst = CellDst;

/*
 * Copy wrapping edges:
 *
 *	0	ff	f0 f1 ... fe ff		f0
 *
 *	1	0f	00 01 ... 0e 0f		00
 *	2	1f	10 11 ... 1e 1f		10
 *		..	.. ..     .. ..		..
 *		ef	e0 e1 ... ee ef		e0
 *	h	ff	f0 f1 ... fe ff		f0
 *
 *	h+1	0f	00 01 ... 0e 0f		00
 *
 * wrap value:	effect:
 *	0	no effect
 *	1	copy future=>past, no wrap
 *	2	no copy, wrap edges
 *	3	copy future=>past, wrap edges
 *	4	copy future=>past, same edges
 */

  switch (heat_wrap) {
  case 0:
    break;
  case 1:
    for (x = 0; x < WORLD_X; x++) {
      memcpy(src, dst, WORLD_Y * sizeof (short));
      src += SRCCOL;
      dst += DSTCOL;
    }
    break;
  case 2:
    for (x = 0; x < WORLD_X; x++) {
      src[-1] = src[WORLD_Y - 1];
      src[WORLD_Y] = src[0];
      src += SRCCOL;
      dst += DSTCOL;
    }
    memcpy(CellSrc,CellSrc + (SRCCOL * WORLD_X),
	  SRCCOL * sizeof (short));
    memcpy(CellSrc + SRCCOL * (WORLD_X + 1), CellSrc + SRCCOL,
	  SRCCOL * sizeof (short));
    break;
  case 3:
    for (x = 0; x < WORLD_X; x++) {
      memcpy(src, dst, WORLD_Y * sizeof (short));
      src[-1] = src[WORLD_Y - 1];
      src[WORLD_Y] = src[0];
      src += SRCCOL;
      dst += DSTCOL;
    }
    memcpy(CellSrc, CellSrc + (SRCCOL * WORLD_X),
	   SRCCOL * sizeof (short));
    memcpy(CellSrc + SRCCOL * (WORLD_X + 1), CellSrc + SRCCOL,
	   SRCCOL * sizeof (short));
    break;
  case 4:
    src[0] = dst[0];
    src[1 + WORLD_Y] = dst[WORLD_Y - 1];
    src[(1 + WORLD_X) * SRCCOL] = dst[(WORLD_X - 1) * DSTCOL];
    src[((2 + WORLD_X) * SRCCOL) - 1] = dst[(WORLD_X * WORLD_Y) - 1];
    for (x = 0; x < WORLD_X; x++) {
      memcpy(src, dst, WORLD_Y * sizeof (short));
      src[-1] = src[0];
      src[WORLD_Y] =  src[WORLD_Y - 1];
      src += SRCCOL;
      dst += DSTCOL;
    }
    memcpy(CellSrc + (SRCCOL * (WORLD_X + 1)), CellSrc + (SRCCOL * WORLD_X),
	   SRCCOL * sizeof (short));
    memcpy(CellSrc, CellSrc + SRCCOL,
	   SRCCOL * sizeof (short));
    break;
  }


  switch (heat_rule) {

  case 0:
#define HEAT \
	a += nw + n + ne + w + e + sw + s + se + fl; \
	dst[0] = ((a >> 3) & LOMASK) | \
		     (ANIMBIT | BURNBIT | BULLBIT); \
	a &= 7;

    CLIPPER_LOOP_BODY(HEAT);
    break;

  case 1:
#define ECOMASK 0x3fc
#define ECO \
      c -= fl; n -= fl; s -= fl; e -= fl; w -= fl; \
      ne -= fl; nw -= fl; se -= fl; sw -= fl; \
      \
      /* anneal */ \
      { int sum = (c&1) + (n&1) + (s&1) + (e&1) + (w&1) + \
		  (ne&1) + (nw&1) + (se&1) + (sw&1), cell; \
	if (((sum > 5) || (sum == 4))) { \
	  /* brian's brain */ \
	  cell = ((c <<1) & (0x3fc)) | \
		 (((((c >>1)&3) == 0) && \
		   (((n&2) + (s&2) + (e&2) + (w&2) + \
		     (ne&2) + (nw&2) + (se&2) + (sw&2)) == (2 <<1)) \
		  ) ? 2 : 0) | \
		 1; \
	} else { \
	  /* anti-life */ \
	  sum = ((n&2) + (s&2) + (e&2) + (w&2) + \
		 (ne&2) + (nw&2) + (se&2) + (sw&2)) >>1; \
	  cell = (((c ^ 2) <<1) & ECOMASK) | \
		 ((c&2) ? ((sum != 5) ? 2 : 0) \
			: (((sum != 5) && (sum != 6)) ? 2 : 0)); \
	} \
	dst[0] = ((fl + cell) & LOMASK) | \
		 (ANIMBIT | BURNBIT | BULLBIT); \
      } \
      c += fl; n += fl; s += fl; e += fl; w += fl; \
      ne += fl; nw += fl; se += fl; sw += fl;

    CLIPPER_LOOP_BODY(ECO);
    break;
  }
}

void sim_loop(int doSim)
{
#ifdef CAM
  if (!sim_just_cam) {
#endif
    if (heat_steps) {
      int j;

      for (j = 0; j < heat_steps; j++) {
	sim_heat();
      }

      MoveObjects();
/*
      InvalidateMaps();
*/
      NewMap = 1;
    } else {
      if (doSim) {
	SimFrame();
      }
      MoveObjects();
    }

    sim_loops++;
    sim_update();
#ifdef CAM
  } else {
    sim_update_cams();
    UpdateFlush();
    DoTimeoutListen();
  }
#endif
}


static void sim_update_maps(void) {
	SimView* view;
	int i;
	for (view = sim->map; view != NULL; view = view->next) {
		int mustUpdateMap = NewMapFlags[view->map_state] || NewMap || ShakeNow;
		if (mustUpdateMap) {
			view->invalid = 1;
		}
		if (view->invalid) {
#if 1
			if (mustUpdateMap) {
				//fprintf(stderr, "sim_update_maps mustUpdateMap\n");
				//	view->skip = 0;
			}
			if (DoUpdateMap(view)) {
				//          CancelRedrawView(view);
				//	  view->invalid = 1;
			}
#else
			EventuallyRedrawView(view);
#endif
		}
	}
	NewMap = 0;
	for (i = 0; i < NMAPS; i++) {
		NewMapFlags[i] = 0;
	}
}

void sim_update_editors(void)
{
  SimView *view;

  for (view = sim->editor; view != NULL; view = view->next) {
#if 1
    CancelRedrawView(view);
    view->invalid = 1;
    DoUpdateEditor(view);
#else
    EventuallyRedrawView(view);
#endif
  }

  DoUpdateHeads();
}

static void sim_update_graphs(void)
{
  graphDoer();
}


static void sim_update_budgets(void)
{
  if ((sim_skips != 0) &&
      (sim_skip != 0)) {
    return;
  }

  UpdateBudgetWindow();
}


static void sim_update_evaluations(void)
{
  if ((sim_skips != 0) &&
      (sim_skip != 0)) {
    return;
  }

  scoreDoer();
}


void
sim_update()
{
  gettimeofday(&now_time, NULL);

  flagBlink = (now_time.tv_usec < 500000) ? 1 : -1;

  if (SimSpeed && !heat_steps) {
    TilesAnimated = 0;
  }

  sim_update_editors();

  sim_update_maps();
  sim_update_graphs();
  sim_update_budgets();
  sim_update_evaluations();

#if 0
  UpdateFlush();
#endif
}


void Pause(void)
{
  if (!sim_paused) {
    sim_paused_speed = SimMetaSpeed;
    setSpeed(0);
    sim_paused = 1;
  }
}


void Resume(void)
{
  if (sim_paused) {
    sim_paused = 0;
    setSpeed(sim_paused_speed);
  }
}


void setSpeed(short speed)
{
  if (speed < 0) speed = 0;
  else if (speed > 3) speed = 3;

  SimMetaSpeed = speed;

  if (sim_paused) {
    sim_paused_speed = SimMetaSpeed;
    speed = 0;
  }

  SimSpeed = speed;

  if (speed == 0) {
    StopMicropolisTimer();
  } else {
    StartMicropolisTimer();
  }

#ifdef USE_TCL
  { char buf[256];
    sprintf(buf, "UISetSpeed %d", sim_paused ? 0 : SimMetaSpeed);
    Eval(buf);
  }
#endif
}
XpmFileData tiles;
struct NamedQuad {
	RGBQUAD rgb;
	const char* name;
} namedQuads[] = {
		{	{ 0 }, "black" },
		{  { 0, 0, 255, 0 }, "red" },
		{  { 0, 255, 255, 0 }, "yellow" },
		{  { 255, 0, 0, 0 }, "blue" },
		{  { 128,128,128,0 }, "gray50" },
		{  { 75*255/100,75*255/100,75*255/100, 0}, "gray75" },
		{  { 255, 255, 255, 0 }, "gray100" },
		{  { 0, 0, 0, 0 }, "None" },
		{{255, 250, 250, 0}, "snow"},
		{{248, 248, 255, 0}, "ghost white"},
		{{248, 248, 255, 0}, "GhostWhite"},
		{{245, 245, 245, 0}, "white smoke"},
		{{245, 245, 245, 0}, "WhiteSmoke"},
		{{220, 220, 220, 0}, "gainsboro"},
		{{255, 250, 240, 0}, "floral white"},
		{{255, 250, 240, 0}, "FloralWhite"},
		{{253, 245, 230, 0}, "old lace"},
		{{253, 245, 230, 0}, "OldLace"},
		{{250, 240, 230, 0}, "linen"},
		{{250, 235, 215, 0}, "antique white"},
		{{250, 235, 215, 0}, "AntiqueWhite"},
		{{255, 239, 213, 0}, "papaya whip"},
		{{255, 239, 213, 0}, "PapayaWhip"},
		{{255, 235, 205, 0}, "blanched almond"},
		{{255, 235, 205, 0}, "BlanchedAlmond"},
		{{255, 228, 196, 0}, "bisque"},
		{{255, 218, 185, 0}, "peach puff"},
		{{255, 218, 185, 0}, "PeachPuff"},
		{{255, 222, 173, 0}, "navajo white"},
		{{255, 222, 173, 0}, "NavajoWhite"},
		{{255, 228, 181, 0}, "moccasin"},
		{{255, 248, 220, 0}, "cornsilk"},
		{{255, 255, 240, 0}, "ivory"},
		{{255, 250, 205, 0}, "lemon chiffon"},
		{{255, 250, 205, 0}, "LemonChiffon"},
		{{255, 245, 238, 0}, "seashell"},
		{{240, 255, 240, 0}, "honeydew"},
		{{245, 255, 250, 0}, "mint cream"},
		{{245, 255, 250, 0}, "MintCream"},
		{{240, 255, 255, 0}, "azure"},
		{{240, 248, 255, 0}, "alice blue"},
		{{240, 248, 255, 0}, "AliceBlue"},
		{{230, 230, 250, 0}, "lavender"},
		{{255, 240, 245, 0}, "lavender blush"},
		{{255, 240, 245, 0}, "LavenderBlush"},
		{{255, 228, 225, 0}, "misty rose"},
		{{255, 228, 225, 0}, "MistyRose"},
		{{255, 255, 255, 0}, "white"},
		{{  0,   0,   0, 0}, "black"},
		{{ 47,  79,  79, 0}, "dark slate gray"},
		{{ 47,  79,  79, 0}, "DarkSlateGray"},
		{{ 47,  79,  79, 0}, "dark slate grey"},
		{{ 47,  79,  79, 0}, "DarkSlateGrey"},
		{{105, 105, 105, 0}, "dim gray"},
		{{105, 105, 105, 0}, "DimGray"},
		{{105, 105, 105, 0}, "dim grey"},
		{{105, 105, 105, 0}, "DimGrey"},
		{{112, 128, 144, 0}, "slate gray"},
		{{112, 128, 144, 0}, "SlateGray"},
		{{112, 128, 144, 0}, "slate grey"},
		{{112, 128, 144, 0}, "SlateGrey"},
		{{119, 136, 153, 0}, "light slate gray"},
		{{119, 136, 153, 0}, "LightSlateGray"},
		{{119, 136, 153, 0}, "light slate grey"},
		{{119, 136, 153, 0}, "LightSlateGrey"},
		{{190, 190, 190, 0}, "gray"},
		{{190, 190, 190, 0}, "grey"},
		{{211, 211, 211, 0}, "light grey"},
		{{211, 211, 211, 0}, "LightGrey"},
		{{211, 211, 211, 0}, "light gray"},
		{{211, 211, 211, 0}, "LightGray"},
		{{ 25,  25, 112, 0}, "midnight blue"},
		{{ 25,  25, 112, 0}, "MidnightBlue"},
		{{  0,   0, 128, 0}, "navy"},
		{{  0,   0, 128, 0}, "navy blue"},
		{{  0,   0, 128, 0}, "NavyBlue"},
		{{100, 149, 237, 0}, "cornflower blue"},
		{{100, 149, 237, 0}, "CornflowerBlue"},
		{{ 72,  61, 139, 0}, "dark slate blue"},
		{{ 72,  61, 139, 0}, "DarkSlateBlue"},
		{{106,  90, 205, 0}, "slate blue"},
		{{106,  90, 205, 0}, "SlateBlue"},
		{{123, 104, 238, 0}, "medium slate blue"},
		{{123, 104, 238, 0}, "MediumSlateBlue"},
		{{132, 112, 255, 0}, "light slate blue"},
		{{132, 112, 255, 0}, "LightSlateBlue"},
		{{  0,   0, 205, 0}, "medium blue"},
		{{  0,   0, 205, 0}, "MediumBlue"},
		{{ 65, 105, 225, 0}, "royal blue"},
		{{ 65, 105, 225, 0}, "RoyalBlue"},
		{{  0,   0, 255, 0}, "blue"},
		{{ 30, 144, 255, 0}, "dodger blue"},
		{{ 30, 144, 255, 0}, "DodgerBlue"},
		{{  0, 191, 255, 0}, "deep sky blue"},
		{{  0, 191, 255, 0}, "DeepSkyBlue"},
		{{135, 206, 235, 0}, "sky blue"},
		{{135, 206, 235, 0}, "SkyBlue"},
		{{135, 206, 250, 0}, "light sky blue"},
		{{135, 206, 250, 0}, "LightSkyBlue"},
		{{ 70, 130, 180, 0}, "steel blue"},
		{{ 70, 130, 180, 0}, "SteelBlue"},
		{{176, 196, 222, 0}, "light steel blue"},
		{{176, 196, 222, 0}, "LightSteelBlue"},
		{{173, 216, 230, 0}, "light blue"},
		{{173, 216, 230, 0}, "LightBlue"},
		{{176, 224, 230, 0}, "powder blue"},
		{{176, 224, 230, 0}, "PowderBlue"},
		{{175, 238, 238, 0}, "pale turquoise"},
		{{175, 238, 238, 0}, "PaleTurquoise"},
		{{  0, 206, 209, 0}, "dark turquoise"},
		{{  0, 206, 209, 0}, "DarkTurquoise"},
		{{ 72, 209, 204, 0}, "medium turquoise"},
		{{ 72, 209, 204, 0}, "MediumTurquoise"},
		{{ 64, 224, 208, 0}, "turquoise"},
		{{  0, 255, 255, 0}, "cyan"},
		{{224, 255, 255, 0}, "light cyan"},
		{{224, 255, 255, 0}, "LightCyan"},
		{{ 95, 158, 160, 0}, "cadet blue"},
		{{ 95, 158, 160, 0}, "CadetBlue"},
		{{102, 205, 170, 0}, "medium aquamarine"},
		{{102, 205, 170, 0}, "MediumAquamarine"},
		{{127, 255, 212, 0}, "aquamarine"},
		{{  0, 100,   0, 0}, "dark green"},
		{{  0, 100,   0, 0}, "DarkGreen"},
		{{ 85, 107,  47, 0}, "dark olive green"},
		{{ 85, 107,  47, 0}, "DarkOliveGreen"},
		{{143, 188, 143, 0}, "dark sea green"},
		{{143, 188, 143, 0}, "DarkSeaGreen"},
		{{ 46, 139,  87, 0}, "sea green"},
		{{ 46, 139,  87, 0}, "SeaGreen"},
		{{ 60, 179, 113, 0}, "medium sea green"},
		{{ 60, 179, 113, 0}, "MediumSeaGreen"},
		{{ 32, 178, 170, 0}, "light sea green"},
		{{ 32, 178, 170, 0}, "LightSeaGreen"},
		{{152, 251, 152, 0}, "pale green"},
		{{152, 251, 152, 0}, "PaleGreen"},
		{{  0, 255, 127, 0}, "spring green"},
		{{  0, 255, 127, 0}, "SpringGreen"},
		{{124, 252,   0, 0}, "lawn green"},
		{{124, 252,   0, 0}, "LawnGreen"},
		{{  0, 255,   0, 0}, "green"},
		{{127, 255,   0, 0}, "chartreuse"},
		{{  0, 250, 154, 0}, "medium spring green"},
		{{  0, 250, 154, 0}, "MediumSpringGreen"},
		{{173, 255,  47, 0}, "green yellow"},
		{{173, 255,  47, 0}, "GreenYellow"},
		{{ 50, 205,  50, 0}, "lime green"},
		{{ 50, 205,  50, 0}, "LimeGreen"},
		{{154, 205,  50, 0}, "yellow green"},
		{{154, 205,  50, 0}, "YellowGreen"},
		{{ 34, 139,  34, 0}, "forest green"},
		{{ 34, 139,  34, 0}, "ForestGreen"},
		{{107, 142,  35, 0}, "olive drab"},
		{{107, 142,  35, 0}, "OliveDrab"},
		{{189, 183, 107, 0}, "dark khaki"},
		{{189, 183, 107, 0}, "DarkKhaki"},
		{{240, 230, 140, 0}, "khaki"},
		{{238, 232, 170, 0}, "pale goldenrod"},
		{{238, 232, 170, 0}, "PaleGoldenrod"},
		{{250, 250, 210, 0}, "light goldenrod yellow"},
		{{250, 250, 210, 0}, "LightGoldenrodYellow"},
		{{255, 255, 224, 0}, "light yellow"},
		{{255, 255, 224, 0}, "LightYellow"},
		{{255, 255,   0, 0}, "yellow"},
		{{255, 215,   0, 0}, "gold"},
		{{238, 221, 130, 0}, "light goldenrod"},
		{{238, 221, 130, 0}, "LightGoldenrod"},
		{{218, 165,  32, 0}, "goldenrod"},
		{{184, 134,  11, 0}, "dark goldenrod"},
		{{184, 134,  11, 0}, "DarkGoldenrod"},
		{{188, 143, 143, 0}, "rosy brown"},
		{{188, 143, 143, 0}, "RosyBrown"},
		{{205,  92,  92, 0}, "indian red"},
		{{205,  92,  92, 0}, "IndianRed"},
		{{139,  69,  19, 0}, "saddle brown"},
		{{139,  69,  19, 0}, "SaddleBrown"},
		{{160,  82,  45, 0}, "sienna"},
		{{205, 133,  63, 0}, "peru"},
		{{222, 184, 135, 0}, "burlywood"},
		{{245, 245, 220, 0}, "beige"},
		{{245, 222, 179, 0}, "wheat"},
		{{244, 164,  96, 0}, "sandy brown"},
		{{244, 164,  96, 0}, "SandyBrown"},
		{{210, 180, 140, 0}, "tan"},
		{{210, 105,  30, 0}, "chocolate"},
		{{178,  34,  34, 0}, "firebrick"},
		{{165,  42,  42, 0}, "brown"},
		{{233, 150, 122, 0}, "dark salmon"},
		{{233, 150, 122, 0}, "DarkSalmon"},
		{{250, 128, 114, 0}, "salmon"},
		{{255, 160, 122, 0}, "light salmon"},
		{{255, 160, 122, 0}, "LightSalmon"},
		{{255, 165,   0, 0}, "orange"},
		{{255, 140,   0, 0}, "dark orange"},
		{{255, 140,   0, 0}, "DarkOrange"},
		{{255, 127,  80, 0}, "coral"},
		{{240, 128, 128, 0}, "light coral"},
		{{240, 128, 128, 0}, "LightCoral"},
		{{255,  99,  71, 0}, "tomato"},
		{{255,  69,   0, 0}, "orange red"},
		{{255,  69,   0, 0}, "OrangeRed"},
		{{255,   0,   0, 0}, "red"},
		{{255, 105, 180, 0}, "hot pink"},
		{{255, 105, 180, 0}, "HotPink"},
		{{255,  20, 147, 0}, "deep pink"},
		{{255,  20, 147, 0}, "DeepPink"},
		{{255, 192, 203, 0}, "pink"},
		{{255, 182, 193, 0}, "light pink"},
		{{255, 182, 193, 0}, "LightPink"},
		{{219, 112, 147, 0}, "pale violet red"},
		{{219, 112, 147, 0}, "PaleVioletRed"},
		{{176,  48,  96, 0}, "maroon"},
		{{199,  21, 133, 0}, "medium violet red"},
		{{199,  21, 133, 0}, "MediumVioletRed"},
		{{208,  32, 144, 0}, "violet red"},
		{{208,  32, 144, 0}, "VioletRed"},
		{{255,   0, 255, 0}, "magenta"},
		{{238, 130, 238, 0}, "violet"},
		{{221, 160, 221, 0}, "plum"},
		{{218, 112, 214, 0}, "orchid"},
		{{186,  85, 211, 0}, "medium orchid"},
		{{186,  85, 211, 0}, "MediumOrchid"},
		{{153,  50, 204, 0}, "dark orchid"},
		{{153,  50, 204, 0}, "DarkOrchid"},
		{{148,   0, 211, 0}, "dark violet"},
		{{148,   0, 211, 0}, "DarkViolet"},
		{{138,  43, 226, 0}, "blue violet"},
		{{138,  43, 226, 0}, "BlueViolet"},
		{{160,  32, 240, 0}, "purple"},
		{{147, 112, 219, 0}, "medium purple"},
		{{147, 112, 219, 0}, "MediumPurple"},
		{{216, 191, 216, 0}, "thistle"},
		{{255, 250, 250, 0}, "snow1"},
		{{238, 233, 233, 0}, "snow2"},
		{{205, 201, 201, 0}, "snow3"},
		{{139, 137, 137, 0}, "snow4"},
		{{255, 245, 238, 0}, "seashell1"},
		{{238, 229, 222, 0}, "seashell2"},
		{{205, 197, 191, 0}, "seashell3"},
		{{139, 134, 130, 0}, "seashell4"},
		{{255, 239, 219, 0}, "AntiqueWhite1"},
		{{238, 223, 204, 0}, "AntiqueWhite2"},
		{{205, 192, 176, 0}, "AntiqueWhite3"},
		{{139, 131, 120, 0}, "AntiqueWhite4"},
		{{255, 228, 196, 0}, "bisque1"},
		{{238, 213, 183, 0}, "bisque2"},
		{{205, 183, 158, 0}, "bisque3"},
		{{139, 125, 107, 0}, "bisque4"},
		{{255, 218, 185, 0}, "PeachPuff1"},
		{{238, 203, 173, 0}, "PeachPuff2"},
		{{205, 175, 149, 0}, "PeachPuff3"},
		{{139, 119, 101, 0}, "PeachPuff4"},
		{{255, 222, 173, 0}, "NavajoWhite1"},
		{{238, 207, 161, 0}, "NavajoWhite2"},
		{{205, 179, 139, 0}, "NavajoWhite3"},
		{{139, 121,	94, 0}, "NavajoWhite4"},
		{{255, 250, 205, 0}, "LemonChiffon1"},
		{{238, 233, 191, 0}, "LemonChiffon2"},
		{{205, 201, 165, 0}, "LemonChiffon3"},
		{{139, 137, 112, 0}, "LemonChiffon4"},
		{{255, 248, 220, 0}, "cornsilk1"},
		{{238, 232, 205, 0}, "cornsilk2"},
		{{205, 200, 177, 0}, "cornsilk3"},
		{{139, 136, 120, 0}, "cornsilk4"},
		{{255, 255, 240, 0}, "ivory1"},
		{{238, 238, 224, 0}, "ivory2"},
		{{205, 205, 193, 0}, "ivory3"},
		{{139, 139, 131, 0}, "ivory4"},
		{{240, 255, 240, 0}, "honeydew1"},
		{{224, 238, 224, 0}, "honeydew2"},
		{{193, 205, 193, 0}, "honeydew3"},
		{{131, 139, 131, 0}, "honeydew4"},
		{{255, 240, 245, 0}, "LavenderBlush1"},
		{{238, 224, 229, 0}, "LavenderBlush2"},
		{{205, 193, 197, 0}, "LavenderBlush3"},
		{{139, 131, 134, 0}, "LavenderBlush4"},
		{{255, 228, 225, 0}, "MistyRose1"},
		{{238, 213, 210, 0}, "MistyRose2"},
		{{205, 183, 181, 0}, "MistyRose3"},
		{{139, 125, 123, 0}, "MistyRose4"},
		{{240, 255, 255, 0}, "azure1"},
		{{224, 238, 238, 0}, "azure2"},
		{{193, 205, 205, 0}, "azure3"},
		{{131, 139, 139, 0}, "azure4"},
		{{131, 111, 255, 0}, "SlateBlue1"},
		{{122, 103, 238, 0}, "SlateBlue2"},
		{{105,  89, 205, 0}, "SlateBlue3"},
		{{ 71,  60, 139, 0}, "SlateBlue4"},
		{{ 72, 118, 255, 0}, "RoyalBlue1"},
		{{ 67, 110, 238, 0}, "RoyalBlue2"},
		{{ 58,  95, 205, 0}, "RoyalBlue3"},
		{{ 39,  64, 139, 0}, "RoyalBlue4"},
		{{  0,   0, 255, 0}, "blue1"},
		{{  0,   0, 238, 0}, "blue2"},
		{{  0,   0, 205, 0}, "blue3"},
		{{  0,   0, 139, 0}, "blue4"},
		{{ 30, 144, 255, 0}, "DodgerBlue1"},
		{{ 28, 134, 238, 0}, "DodgerBlue2"},
		{{ 24, 116, 205, 0}, "DodgerBlue3"},
		{{ 16,  78, 139, 0}, "DodgerBlue4"},
		{{ 99, 184, 255, 0}, "SteelBlue1"},
		{{ 92, 172, 238, 0}, "SteelBlue2"},
		{{ 79, 148, 205, 0}, "SteelBlue3"},
		{{ 54, 100, 139, 0}, "SteelBlue4"},
		{{  0, 191, 255, 0}, "DeepSkyBlue1"},
		{{  0, 178, 238, 0}, "DeepSkyBlue2"},
		{{  0, 154, 205, 0}, "DeepSkyBlue3"},
		{{  0, 104, 139, 0}, "DeepSkyBlue4"},
		{{135, 206, 255, 0}, "SkyBlue1"},
		{{126, 192, 238, 0}, "SkyBlue2"},
		{{108, 166, 205, 0}, "SkyBlue3"},
		{{ 74, 112, 139, 0}, "SkyBlue4"},
		{{176, 226, 255, 0}, "LightSkyBlue1"},
		{{164, 211, 238, 0}, "LightSkyBlue2"},
		{{141, 182, 205, 0}, "LightSkyBlue3"},
		{{ 96, 123, 139, 0}, "LightSkyBlue4"},
		{{198, 226, 255, 0}, "SlateGray1"},
		{{185, 211, 238, 0}, "SlateGray2"},
		{{159, 182, 205, 0}, "SlateGray3"},
		{{108, 123, 139, 0}, "SlateGray4"},
		{{202, 225, 255, 0}, "LightSteelBlue1"},
		{{188, 210, 238, 0}, "LightSteelBlue2"},
		{{162, 181, 205, 0}, "LightSteelBlue3"},
		{{110, 123, 139, 0}, "LightSteelBlue4"},
		{{191, 239, 255, 0}, "LightBlue1"},
		{{178, 223, 238, 0}, "LightBlue2"},
		{{154, 192, 205, 0}, "LightBlue3"},
		{{104, 131, 139, 0}, "LightBlue4"},
		{{224, 255, 255, 0}, "LightCyan1"},
		{{209, 238, 238, 0}, "LightCyan2"},
		{{180, 205, 205, 0}, "LightCyan3"},
		{{122, 139, 139, 0}, "LightCyan4"},
		{{187, 255, 255, 0}, "PaleTurquoise1"},
		{{174, 238, 238, 0}, "PaleTurquoise2"},
		{{150, 205, 205, 0}, "PaleTurquoise3"},
		{{102, 139, 139, 0}, "PaleTurquoise4"},
		{{152, 245, 255, 0}, "CadetBlue1"},
		{{142, 229, 238, 0}, "CadetBlue2"},
		{{122, 197, 205, 0}, "CadetBlue3"},
		{{ 83, 134, 139, 0}, "CadetBlue4"},
		{{  0, 245, 255, 0}, "turquoise1"},
		{{  0, 229, 238, 0}, "turquoise2"},
		{{  0, 197, 205, 0}, "turquoise3"},
		{{  0, 134, 139, 0}, "turquoise4"},
		{{  0, 255, 255, 0}, "cyan1"},
		{{  0, 238, 238, 0}, "cyan2"},
		{{  0, 205, 205, 0}, "cyan3"},
		{{  0, 139, 139, 0}, "cyan4"},
		{{151, 255, 255, 0}, "DarkSlateGray1"},
		{{141, 238, 238, 0}, "DarkSlateGray2"},
		{{121, 205, 205, 0}, "DarkSlateGray3"},
		{{ 82, 139, 139, 0}, "DarkSlateGray4"},
		{{127, 255, 212, 0}, "aquamarine1"},
		{{118, 238, 198, 0}, "aquamarine2"},
		{{102, 205, 170, 0}, "aquamarine3"},
		{{ 69, 139, 116, 0}, "aquamarine4"},
		{{193, 255, 193, 0}, "DarkSeaGreen1"},
		{{180, 238, 180, 0}, "DarkSeaGreen2"},
		{{155, 205, 155, 0}, "DarkSeaGreen3"},
		{{105, 139, 105, 0}, "DarkSeaGreen4"},
		{{ 84, 255, 159, 0}, "SeaGreen1"},
		{{ 78, 238, 148, 0}, "SeaGreen2"},
		{{ 67, 205, 128, 0}, "SeaGreen3"},
		{{ 46, 139,	 87, 0}, "SeaGreen4"},
		{{154, 255, 154, 0}, "PaleGreen1"},
		{{144, 238, 144, 0}, "PaleGreen2"},
		{{124, 205, 124, 0}, "PaleGreen3"},
		{{ 84, 139,	 84, 0}, "PaleGreen4"},
		{{  0, 255, 127, 0}, "SpringGreen1"},
		{{  0, 238, 118, 0}, "SpringGreen2"},
		{{  0, 205, 102, 0}, "SpringGreen3"},
		{{  0, 139,	 69, 0}, "SpringGreen4"},
		{{  0, 255,	  0, 0}, "green1"},
		{{  0, 238,	  0, 0}, "green2"},
		{{  0, 205,	  0, 0}, "green3"},
		{{  0, 139,	  0, 0}, "green4"},
		{{127, 255,	  0, 0}, "chartreuse1"},
		{{118, 238,	  0, 0}, "chartreuse2"},
		{{102, 205,	  0, 0}, "chartreuse3"},
		{{ 69, 139,	  0, 0}, "chartreuse4"},
		{{192, 255,	 62, 0}, "OliveDrab1"},
		{{179, 238,	 58, 0}, "OliveDrab2"},
		{{154, 205,	 50, 0}, "OliveDrab3"},
		{{105, 139,	 34, 0}, "OliveDrab4"},
		{{202, 255, 112, 0}, "DarkOliveGreen1"},
		{{188, 238, 104, 0}, "DarkOliveGreen2"},
		{{162, 205,	 90, 0}, "DarkOliveGreen3"},
		{{110, 139,	 61, 0}, "DarkOliveGreen4"},
		{{255, 246, 143, 0}, "khaki1"},
		{{238, 230, 133, 0}, "khaki2"},
		{{205, 198, 115, 0}, "khaki3"},
		{{139, 134,	 78, 0}, "khaki4"},
		{{255, 236, 139, 0}, "LightGoldenrod1"},
		{{238, 220, 130, 0}, "LightGoldenrod2"},
		{{205, 190, 112, 0}, "LightGoldenrod3"},
		{{139, 129,	 76, 0}, "LightGoldenrod4"},
		{{255, 255, 224, 0}, "LightYellow1"},
		{{238, 238, 209, 0}, "LightYellow2"},
		{{205, 205, 180, 0}, "LightYellow3"},
		{{139, 139, 122, 0}, "LightYellow4"},
		{{255, 255,	  0, 0}, "yellow1"},
		{{238, 238,	  0, 0}, "yellow2"},
		{{205, 205,	  0, 0}, "yellow3"},
		{{139, 139,	  0, 0}, "yellow4"},
		{{255, 215,	  0, 0}, "gold1"},
		{{238, 201,	  0, 0}, "gold2"},
		{{205, 173,	  0, 0}, "gold3"},
		{{139, 117,	  0, 0}, "gold4"},
		{{255, 193,	 37, 0}, "goldenrod1"},
		{{238, 180,	 34, 0}, "goldenrod2"},
		{{205, 155,	 29, 0}, "goldenrod3"},
		{{139, 105,	 20, 0}, "goldenrod4"},
		{{255, 185,	 15, 0}, "DarkGoldenrod1"},
		{{238, 173,	 14, 0}, "DarkGoldenrod2"},
		{{205, 149,	 12, 0}, "DarkGoldenrod3"},
		{{139, 101,	  8, 0}, "DarkGoldenrod4"},
		{{255, 193, 193, 0}, "RosyBrown1"},
		{{238, 180, 180, 0}, "RosyBrown2"},
		{{205, 155, 155, 0}, "RosyBrown3"},
		{{139, 105, 105, 0}, "RosyBrown4"},
		{{255, 106, 106, 0}, "IndianRed1"},
		{{238,  99,	 99, 0}, "IndianRed2"},
		{{205,  85,	 85, 0}, "IndianRed3"},
		{{139,  58,	 58, 0}, "IndianRed4"},
		{{255, 130,	 71, 0}, "sienna1"},
		{{238, 121,	 66, 0}, "sienna2"},
		{{205, 104,	 57, 0}, "sienna3"},
		{{139,  71,	 38, 0}, "sienna4"},
		{{255, 211, 155, 0}, "burlywood1"},
		{{238, 197, 145, 0}, "burlywood2"},
		{{205, 170, 125, 0}, "burlywood3"},
		{{139, 115,	 85, 0}, "burlywood4"},
		{{255, 231, 186, 0}, "wheat1"},
		{{238, 216, 174, 0}, "wheat2"},
		{{205, 186, 150, 0}, "wheat3"},
		{{139, 126, 102, 0}, "wheat4"},
		{{255, 165,	 79, 0}, "tan1"},
		{{238, 154,	 73, 0}, "tan2"},
		{{205, 133,	 63, 0}, "tan3"},
		{{139,  90,	 43, 0}, "tan4"},
		{{255, 127,	 36, 0}, "chocolate1"},
		{{238, 118,	 33, 0}, "chocolate2"},
		{{205, 102,	 29, 0}, "chocolate3"},
		{{139,  69,	 19, 0}, "chocolate4"},
		{{255,  48,	 48, 0}, "firebrick1"},
		{{238,  44,	 44, 0}, "firebrick2"},
		{{205,  38,	 38, 0}, "firebrick3"},
		{{139,  26,	 26, 0}, "firebrick4"},
		{{255,  64,	 64, 0}, "brown1"},
		{{238,  59,	 59, 0}, "brown2"},
		{{205,  51,	 51, 0}, "brown3"},
		{{139,  35,	 35, 0}, "brown4"},
		{{255, 140, 105, 0}, "salmon1"},
		{{238, 130,	 98, 0}, "salmon2"},
		{{205, 112,	 84, 0}, "salmon3"},
		{{139,  76,	 57, 0}, "salmon4"},
		{{255, 160, 122, 0}, "LightSalmon1"},
		{{238, 149, 114, 0}, "LightSalmon2"},
		{{205, 129,	 98, 0}, "LightSalmon3"},
		{{139,  87,	 66, 0}, "LightSalmon4"},
		{{255, 165,	  0, 0}, "orange1"},
		{{238, 154,	  0, 0}, "orange2"},
		{{205, 133,	  0, 0}, "orange3"},
		{{139,  90,	  0, 0}, "orange4"},
		{{255, 127,	  0, 0}, "DarkOrange1"},
		{{238, 118,	  0, 0}, "DarkOrange2"},
		{{205, 102,	  0, 0}, "DarkOrange3"},
		{{139,  69,	  0, 0}, "DarkOrange4"},
		{{255, 114,	 86, 0}, "coral1"},
		{{238, 106,	 80, 0}, "coral2"},
		{{205,  91,	 69, 0}, "coral3"},
		{{139,  62,	 47, 0}, "coral4"},
		{{255,  99,	 71, 0}, "tomato1"},
		{{238,  92,	 66, 0}, "tomato2"},
		{{205,  79,	 57, 0}, "tomato3"},
		{{139,  54,	 38, 0}, "tomato4"},
		{{255,  69,	  0, 0}, "OrangeRed1"},
		{{238,  64,	  0, 0}, "OrangeRed2"},
		{{205,  55,	  0, 0}, "OrangeRed3"},
		{{139,  37,	  0, 0}, "OrangeRed4"},
		{{255,   0,	  0, 0}, "red1"},
		{{238,   0,	  0, 0}, "red2"},
		{{205,   0,	  0, 0}, "red3"},
		{{139,   0,	  0, 0}, "red4"},
		{{255,  20, 147, 0}, "DeepPink1"},
		{{238,  18, 137, 0}, "DeepPink2"},
		{{205,  16, 118, 0}, "DeepPink3"},
		{{139,  10,	 80, 0}, "DeepPink4"},
		{{255, 110, 180, 0}, "HotPink1"},
		{{238, 106, 167, 0}, "HotPink2"},
		{{205,  96, 144, 0}, "HotPink3"},
		{{139,  58,  98, 0}, "HotPink4"},
		{{255, 181, 197, 0}, "pink1"},
		{{238, 169, 184, 0}, "pink2"},
		{{205, 145, 158, 0}, "pink3"},
		{{139,  99, 108, 0}, "pink4"},
		{{255, 174, 185, 0}, "LightPink1"},
		{{238, 162, 173, 0}, "LightPink2"},
		{{205, 140, 149, 0}, "LightPink3"},
		{{139,  95, 101, 0}, "LightPink4"},
		{{255, 130, 171, 0}, "PaleVioletRed1"},
		{{238, 121, 159, 0}, "PaleVioletRed2"},
		{{205, 104, 137, 0}, "PaleVioletRed3"},
		{{139,  71,	 93, 0}, "PaleVioletRed4"},
		{{255,  52, 179, 0}, "maroon1"},
		{{238,  48, 167, 0}, "maroon2"},
		{{205,  41, 144, 0}, "maroon3"},
		{{139,  28,	 98, 0}, "maroon4"},
		{{255,  62, 150, 0}, "VioletRed1"},
		{{238,  58, 140, 0}, "VioletRed2"},
		{{205,  50, 120, 0}, "VioletRed3"},
		{{139,  34,	 82, 0}, "VioletRed4"},
		{{255,   0, 255, 0}, "magenta1"},
		{{238,   0, 238, 0}, "magenta2"},
		{{205,   0, 205, 0}, "magenta3"},
		{{139,   0, 139, 0}, "magenta4"},
		{{255, 131, 250, 0}, "orchid1"},
		{{238, 122, 233, 0}, "orchid2"},
		{{205, 105, 201, 0}, "orchid3"},
		{{139,  71, 137, 0}, "orchid4"},
		{{255, 187, 255, 0}, "plum1"},
		{{238, 174, 238, 0}, "plum2"},
		{{205, 150, 205, 0}, "plum3"},
		{{139, 102, 139, 0}, "plum4"},
		{{224, 102, 255, 0}, "MediumOrchid1"},
		{{209,  95, 238, 0}, "MediumOrchid2"},
		{{180,  82, 205, 0}, "MediumOrchid3"},
		{{122,  55, 139, 0}, "MediumOrchid4"},
		{{191,  62, 255, 0}, "DarkOrchid1"},
		{{178,  58, 238, 0}, "DarkOrchid2"},
		{{154,  50, 205, 0}, "DarkOrchid3"},
		{{104,  34, 139, 0}, "DarkOrchid4"},
		{{155,  48, 255, 0}, "purple1"},
		{{145,  44, 238, 0}, "purple2"},
		{{125,  38, 205, 0}, "purple3"},
		{{ 85,  26, 139, 0}, "purple4"},
		{{171, 130, 255, 0}, "MediumPurple1"},
		{{159, 121, 238, 0}, "MediumPurple2"},
		{{137, 104, 205, 0}, "MediumPurple3"},
		{{ 93,  71, 139, 0}, "MediumPurple4"},
		{{255, 225, 255, 0}, "thistle1"},
		{{238, 210, 238, 0}, "thistle2"},
		{{205, 181, 205, 0}, "thistle3"},
		{{139, 123, 139, 0}, "thistle4"},
		{{  0,   0,   0, 0}, "gray0"},
		{{  0,   0,   0, 0}, "grey0"},
		{{  3,   3,   3, 0}, "gray1"},
		{{  3,   3,   3, 0}, "grey1"},
		{{  5,   5,   5, 0}, "gray2"},
		{{  5,   5,   5, 0}, "grey2"},
		{{  8,   8,   8, 0}, "gray3"},
		{{  8,   8,   8, 0}, "grey3"},
		{{ 10,  10,  10, 0}, "gray4"},
		{{ 10,  10,  10, 0}, "grey4"},
		{{ 13,  13,  13, 0}, "gray5"},
		{{ 13,  13,  13, 0}, "grey5"},
		{{ 15,  15,  15, 0}, "gray6"},
		{{ 15,  15,  15, 0}, "grey6"},
		{{ 18,  18,  18, 0}, "gray7"},
		{{ 18,  18,  18, 0}, "grey7"},
		{{ 20,  20,  20, 0}, "gray8"},
		{{ 20,  20,  20, 0}, "grey8"},
		{{ 23,  23,  23, 0}, "gray9"},
		{{ 23,  23,  23, 0}, "grey9"},
		{{ 26,  26,  26, 0}, "gray10"},
		{{ 26,  26,  26, 0}, "grey10"},
		{{ 28,  28,  28, 0}, "gray11"},
		{{ 28,  28,  28, 0}, "grey11"},
		{{ 31,  31,  31, 0}, "gray12"},
		{{ 31,  31,  31, 0}, "grey12"},
		{{ 33,  33,  33, 0}, "gray13"},
		{{ 33,  33,  33, 0}, "grey13"},
		{{ 36,  36,  36, 0}, "gray14"},
		{{ 36,  36,  36, 0}, "grey14"},
		{{ 38,  38,  38, 0}, "gray15"},
		{{ 38,  38,  38, 0}, "grey15"},
		{{ 41,  41,  41, 0}, "gray16"},
		{{ 41,  41,  41, 0}, "grey16"},
		{{ 43,  43,  43, 0}, "gray17"},
		{{ 43,  43,  43, 0}, "grey17"},
		{{ 46,  46,  46, 0}, "gray18"},
		{{ 46,  46,  46, 0}, "grey18"},
		{{ 48,  48,  48, 0}, "gray19"},
		{{ 48,  48,  48, 0}, "grey19"},
		{{ 51,  51,  51, 0}, "gray20"},
		{{ 51,  51,  51, 0}, "grey20"},
		{{ 54,  54,  54, 0}, "gray21"},
		{{ 54,  54,  54, 0}, "grey21"},
		{{ 56,  56,  56, 0}, "gray22"},
		{{ 56,  56,  56, 0}, "grey22"},
		{{ 59,  59,  59, 0}, "gray23"},
		{{ 59,  59,  59, 0}, "grey23"},
		{{ 61,  61,  61, 0}, "gray24"},
		{{ 61,  61,  61, 0}, "grey24"},
		{{ 64,  64,  64, 0}, "gray25"},
		{{ 64,  64,  64, 0}, "grey25"},
		{{ 66,  66,  66, 0}, "gray26"},
		{{ 66,  66,  66, 0}, "grey26"},
		{{ 69,  69,  69, 0}, "gray27"},
		{{ 69,  69,  69, 0}, "grey27"},
		{{ 71,  71,  71, 0}, "gray28"},
		{{ 71,  71,  71, 0}, "grey28"},
		{{ 74,  74,  74, 0}, "gray29"},
		{{ 74,  74,  74, 0}, "grey29"},
		{{ 77,  77,  77, 0}, "gray30"},
		{{ 77,  77,  77, 0}, "grey30"},
		{{ 79,  79,  79, 0}, "gray31"},
		{{ 79,  79,  79, 0}, "grey31"},
		{{ 82,  82,  82, 0}, "gray32"},
		{{ 82,  82,  82, 0}, "grey32"},
		{{ 84,  84,  84, 0}, "gray33"},
		{{ 84,  84,  84, 0}, "grey33"},
		{{ 87,  87,  87, 0}, "gray34"},
		{{ 87,  87,  87, 0}, "grey34"},
		{{ 89,  89,  89, 0}, "gray35"},
		{{ 89,  89,  89, 0}, "grey35"},
		{{ 92,  92,  92, 0}, "gray36"},
		{{ 92,  92,  92, 0}, "grey36"},
		{{ 94,  94,  94, 0}, "gray37"},
		{{ 94,  94,  94, 0}, "grey37"},
		{{ 97,  97,  97, 0}, "gray38"},
		{{ 97,  97,  97, 0}, "grey38"},
		{{ 99,  99,  99, 0}, "gray39"},
		{{ 99,  99,  99, 0}, "grey39"},
		{{102, 102, 102, 0}, "gray40"},
		{{102, 102, 102, 0}, "grey40"},
		{{105, 105, 105, 0}, "gray41"},
		{{105, 105, 105, 0}, "grey41"},
		{{107, 107, 107, 0}, "gray42"},
		{{107, 107, 107, 0}, "grey42"},
		{{110, 110, 110, 0}, "gray43"},
		{{110, 110, 110, 0}, "grey43"},
		{{112, 112, 112, 0}, "gray44"},
		{{112, 112, 112, 0}, "grey44"},
		{{115, 115, 115, 0}, "gray45"},
		{{115, 115, 115, 0}, "grey45"},
		{{117, 117, 117, 0}, "gray46"},
		{{117, 117, 117, 0}, "grey46"},
		{{120, 120, 120, 0}, "gray47"},
		{{120, 120, 120, 0}, "grey47"},
		{{122, 122, 122, 0}, "gray48"},
		{{122, 122, 122, 0}, "grey48"},
		{{125, 125, 125, 0}, "gray49"},
		{{125, 125, 125, 0}, "grey49"},
		{{127, 127, 127, 0}, "gray50"},
		{{127, 127, 127, 0}, "grey50"},
		{{130, 130, 130, 0}, "gray51"},
		{{130, 130, 130, 0}, "grey51"},
		{{133, 133, 133, 0}, "gray52"},
		{{133, 133, 133, 0}, "grey52"},
		{{135, 135, 135, 0}, "gray53"},
		{{135, 135, 135, 0}, "grey53"},
		{{138, 138, 138, 0}, "gray54"},
		{{138, 138, 138, 0}, "grey54"},
		{{140, 140, 140, 0}, "gray55"},
		{{140, 140, 140, 0}, "grey55"},
		{{143, 143, 143, 0}, "gray56"},
		{{143, 143, 143, 0}, "grey56"},
		{{145, 145, 145, 0}, "gray57"},
		{{145, 145, 145, 0}, "grey57"},
		{{148, 148, 148, 0}, "gray58"},
		{{148, 148, 148, 0}, "grey58"},
		{{150, 150, 150, 0}, "gray59"},
		{{150, 150, 150, 0}, "grey59"},
		{{153, 153, 153, 0}, "gray60"},
		{{153, 153, 153, 0}, "grey60"},
		{{156, 156, 156, 0}, "gray61"},
		{{156, 156, 156, 0}, "grey61"},
		{{158, 158, 158, 0}, "gray62"},
		{{158, 158, 158, 0}, "grey62"},
		{{161, 161, 161, 0}, "gray63"},
		{{161, 161, 161, 0}, "grey63"},
		{{163, 163, 163, 0}, "gray64"},
		{{163, 163, 163, 0}, "grey64"},
		{{166, 166, 166, 0}, "gray65"},
		{{166, 166, 166, 0}, "grey65"},
		{{168, 168, 168, 0}, "gray66"},
		{{168, 168, 168, 0}, "grey66"},
		{{171, 171, 171, 0}, "gray67"},
		{{171, 171, 171, 0}, "grey67"},
		{{173, 173, 173, 0}, "gray68"},
		{{173, 173, 173, 0}, "grey68"},
		{{176, 176, 176, 0}, "gray69"},
		{{176, 176, 176, 0}, "grey69"},
		{{179, 179, 179, 0}, "gray70"},
		{{179, 179, 179, 0}, "grey70"},
		{{181, 181, 181, 0}, "gray71"},
		{{181, 181, 181, 0}, "grey71"},
		{{184, 184, 184, 0}, "gray72"},
		{{184, 184, 184, 0}, "grey72"},
		{{186, 186, 186, 0}, "gray73"},
		{{186, 186, 186, 0}, "grey73"},
		{{189, 189, 189, 0}, "gray74"},
		{{189, 189, 189, 0}, "grey74"},
		{{191, 191, 191, 0}, "gray75"},
		{{191, 191, 191, 0}, "grey75"},
		{{194, 194, 194, 0}, "gray76"},
		{{194, 194, 194, 0}, "grey76"},
		{{196, 196, 196, 0}, "gray77"},
		{{196, 196, 196, 0}, "grey77"},
		{{199, 199, 199, 0}, "gray78"},
		{{199, 199, 199, 0}, "grey78"},
		{{201, 201, 201, 0}, "gray79"},
		{{201, 201, 201, 0}, "grey79"},
		{{204, 204, 204, 0}, "gray80"},
		{{204, 204, 204, 0}, "grey80"},
		{{207, 207, 207, 0}, "gray81"},
		{{207, 207, 207, 0}, "grey81"},
		{{209, 209, 209, 0}, "gray82"},
		{{209, 209, 209, 0}, "grey82"},
		{{212, 212, 212, 0}, "gray83"},
		{{212, 212, 212, 0}, "grey83"},
		{{214, 214, 214, 0}, "gray84"},
		{{214, 214, 214, 0}, "grey84"},
		{{217, 217, 217, 0}, "gray85"},
		{{217, 217, 217, 0}, "grey85"},
		{{219, 219, 219, 0}, "gray86"},
		{{219, 219, 219, 0}, "grey86"},
		{{222, 222, 222, 0}, "gray87"},
		{{222, 222, 222, 0}, "grey87"},
		{{224, 224, 224, 0}, "gray88"},
		{{224, 224, 224, 0}, "grey88"},
		{{227, 227, 227, 0}, "gray89"},
		{{227, 227, 227, 0}, "grey89"},
		{{229, 229, 229, 0}, "gray90"},
		{{229, 229, 229, 0}, "grey90"},
		{{232, 232, 232, 0}, "gray91"},
		{{232, 232, 232, 0}, "grey91"},
		{{235, 235, 235, 0}, "gray92"},
		{{235, 235, 235, 0}, "grey92"},
		{{237, 237, 237, 0}, "gray93"},
		{{237, 237, 237, 0}, "grey93"},
		{{240, 240, 240, 0}, "gray94"},
		{{240, 240, 240, 0}, "grey94"},
		{{242, 242, 242, 0}, "gray95"},
		{{242, 242, 242, 0}, "grey95"},
		{{245, 245, 245, 0}, "gray96"},
		{{245, 245, 245, 0}, "grey96"},
		{{247, 247, 247, 0}, "gray97"},
		{{247, 247, 247, 0}, "grey97"},
		{{250, 250, 250, 0}, "gray98"},
		{{250, 250, 250, 0}, "grey98"},
		{{252, 252, 252, 0}, "gray99"},
		{{252, 252, 252, 0}, "grey99"},
		{{255, 255, 255, 0}, "gray100"},
		{{255, 255, 255, 0}, "grey100"},
		{{169, 169, 169, 0}, "dark grey"},
		{{169, 169, 169, 0}, "DarkGrey"},
		{{169, 169, 169, 0}, "dark gray"},
		{{169, 169, 169, 0}, "DarkGray"},
		{{0,     0, 139, 0}, "dark blue"},
		{{0,     0, 139, 0}, "DarkBlue"},
		{{0,   139, 139, 0}, "dark cyan"},
		{{0,   139, 139, 0}, "DarkCyan"},
		{{139,   0, 139, 0}, "dark magenta"},
		{{139,   0, 139, 0}, "DarkMagenta"},
		{{139,   0,   0, 0}, "dark red"},
		{{139,   0,   0, 0}, "DarkRed"},
		{{144, 238, 144, 0}, "light green"},
		{{144, 238, 144, 0}, "LightGreen"},
		{ 0 }
};

RGBQUAD toQuad(int type, const char* argument) {
	RGBQUAD q;
	q.rgbRed = 255;
	q.rgbGreen =0;
	q.rgbBlue = 0;
	unsigned cval = 0xFF0000;
	int64_t cval2;
	switch (type) {
	case 'c': if (argument[0]=='#') {
		switch (strlen(argument)) {
		case 4:
			cval = strtoul(argument+1,NULL,16);
			q.rgbRed = 255*(cval&0xF00)/0xF00;
			q.rgbGreen = 255*(cval&0xF0)/0xF0;
			q.rgbBlue = 255*(cval&0xF)/0xF;
			break;
		case 7:
			cval = strtoul(argument+1,NULL,16);
			q.rgbRed = (cval&0xFF0000)>>16;
			q.rgbGreen = (cval&0x00FF00)>>8;
			q.rgbBlue = cval&0xFF;
			//printf("argument+1 %s cval %06x r %02x g %02x b %02x\n", argument+1, cval, q.rgbRed, q.rgbGreen, q.rgbBlue);
			break;
		case 13:
			cval2 = strtoll(argument+1,NULL,16);
			q.rgbRed = (cval2&0xFFFF00000000)>>40;
			q.rgbGreen = (cval2&0x0000FFFF0000)>>24;
			q.rgbBlue = (cval2&0xFFFF)>>8;
			break;
		}
	} else {
		for (int i=0; namedQuads[i].name; i++) {
			if (strcmp(namedQuads[i].name,argument)==0) {
				return namedQuads[i].rgb;
			}
		}
	}
	}
	//printf("type %c argument %s cval %06x r %02x g %02x b %02x\n", type, argument, cval, q.rgbRed, q.rgbGreen, q.rgbBlue);
	return q;
}
int pixelToIndex(char* pixel) {
	switch (tiles.charsDepth) {
	case 2:
		for (int i=0; i<tiles.numColors; ++i) {
			if (pixel[0]==tiles.colors[i].color[0] && pixel[1]==tiles.colors[i].color[1]) {
				return i;
			}
		}
		break;
	case 1:
		for (int i=0; i<tiles.numColors; ++i) {
			if (pixel[0]==tiles.colors[i].color[0]) {
				return i;
			}
		}
		break;
	}

	return 0;
}

struct myBITMAPINFO {
	BITMAPINFOHEADER bmiHeader;;
	RGBQUAD bmiColors[256];
};
struct Dib {
	struct myBITMAPINFO info;
	char* bits;
	void* pointer;
	HBITMAP tilebitmap;
};
struct Dib tileDib;
void makeDibFromXpmData() {
	if (tileDib.bits)
		return;
	int rowBytes = 4*((tiles.columns+3)/4);
#if 0
	char* bits = malloc(tiles.rows*rowBytes);
	struct myBITMAPINFO {
		BITMAPINFOHEADER bmiHeader;;
		RGBQUAD bmiColors[256];
	} info = {0};
#endif
	tileDib.bits = malloc(tiles.rows*rowBytes);
	tileDib.info.bmiHeader.biBitCount = 8;
	tileDib.info.bmiHeader.biHeight = -tiles.rows;
	tileDib.info.bmiHeader.biWidth = tiles.columns;
	tileDib.info.bmiHeader.biPlanes = 1;
	tileDib.info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	tileDib.info.bmiHeader.biSizeImage = 0;
	tileDib.info.bmiHeader.biCompression = BI_RGB;
	tileDib.info.bmiHeader.biClrUsed = tiles.numColors;
	tileDib.info.bmiHeader.biClrImportant = tiles.numColors;
	//info.bmiColors = malloc(image.numColors);
	for (int color = 0; color<tiles.numColors; color++) {
		tileDib.info.bmiColors[color] = toQuad(tiles.colors[color].type, tiles.colors[color].argument);
		//printf("color: %d %d %d", info.bmiColors[color].rgbRed,info.bmiColors[color].rgbGreen,info.bmiColors[color].rgbBlue);
	}
	switch (tiles.charsDepth)
	{
	case 1:
	{
		for (int y=0; y<tiles.rows; y++)
			for (int x=0; x<tiles.columns; x++)
				tileDib.bits[y*rowBytes+x] = pixelToIndex(&tiles.body[(y*tiles.columns+x)]);
	}
		break;
	case 2:
	{
		for (int y=0; y<tiles.rows; y++)
			for (int x=0; x<tiles.columns; x++)
				tileDib.bits[y*rowBytes+x] = pixelToIndex(&tiles.body[2*(y*tiles.columns+x)]);
	}
		break;
	}
	HDC hdc = GetDC(NULL);
	tileDib.tilebitmap = CreateDIBSection(hdc, (const BITMAPINFO*)&tileDib.info, DIB_RGB_COLORS, &tileDib.pointer, NULL, 0);
	memcpy(tileDib.pointer, tileDib.bits, tiles.rows*rowBytes);
	ReleaseDC(NULL, hdc);
}

void DrawTileAt(HDC hdc, int tile, RECT dest) {
	makeDibFromXpmData();

	if ((tile & LOMASK) >= TILE_COUNT)
		tile -= TILE_COUNT;

	/* Blink lightning bolt in unpowered zone center */
	int blink=0;
	if (blink && (tile & ZONEBIT) && !(tile & PWRBIT)) {
		tile = LIGHTNINGBOLT;
	} else {
		tile &= LOMASK;
	} // if

#if 0
	int dynamicFilter(int col, int row);
	if ((tile > 63) //&& (view->dynamic_filter != 0)
			&& (dynamicFilter(dest.left/16, dest.top/16) == 0)) {
		tile = 0;
	} // if
#endif
	HDC dc2 = CreateCompatibleDC(hdc);
	HBITMAP xb = SelectBitmap(dc2, tileDib.tilebitmap);
	StretchBlt(hdc, dest.left, dest.top, dest.right-dest.left, dest.bottom-dest.top,
			dc2, 0, tile*16,16,16, SRCCOPY);
	SelectBitmap(dc2,xb);
	DeleteDC(dc2);
}

void DrawMagTileAt(HDC hdc, int mousex, int mousey, const RECT mousebox, int caption) {
	char captionText[256];
	//printf("D: %d,%d\n", mousex,mousey);
	uint16_t tile = Map[mousex/16][mousey/16];
#define B(x,y) ((x&y)?1:0)
	sprintf(captionText, "%d,%d %04X=%d/%d/%d/%d/%d/%d/%03X", mousex/16,mousey/16, tile
			,B(tile,32768),B(tile,16384),B(tile,8192),B(tile,4096),B(tile,2048),B(tile,1024)
			, tile&1023);
	ExtTextOutA(hdc,mousebox.left,mousebox.bottom/*+caption*/,ETO_OPAQUE,NULL,captionText,lstrlenA(captionText),NULL);
	DrawTileAt(hdc, tile, mousebox);
}
static int mousex = 0;
static int mousey = 0;
RECT timerArea = { 300, 0, 396, 48 };
static int timerCount = 0;
void CALLBACK timerFunc(HWND hwnd, UINT msg, UINT_PTR event, DWORD time) {
	InvalidateRect(hwnd, &timerArea, FALSE);
	timerCount++;
}
LRESULT simCityWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		RECT mousebox = { 0, 0, 48, 48 };
		HDC hdc = BeginPaint(hWnd, &ps);
#if 1
		for (int y=0; y<120; y++) {
#define FACTOR 16
			if (y*FACTOR >= ps.rcPaint.top && y*FACTOR <= ps.rcPaint.bottom) {
				RECT cell; cell.top = y*FACTOR; cell.bottom=y*FACTOR+FACTOR;
				for (int x=0; x<120; x++) {
					if (x*FACTOR >= ps.rcPaint.left && x*FACTOR <= ps.rcPaint.right) {
						cell.left = x*FACTOR; cell.right = cell.left+FACTOR;
						DrawTileAt(hdc, /*x+120*y*/Map[x][y],cell);
					}
				}
			}
		}
#endif
		DrawMagTileAt(hdc, mousex,mousey,mousebox,16);
		EndPaint(hWnd, &ps);
		return 0;
	}
	case WM_MOUSEMOVE: {
		mousex = GET_X_LPARAM(lParam);
		mousey = GET_Y_LPARAM(lParam);
		RECT mousebox = { 0, 0, 48, 48 };
		RECT captionbox = {0, 48, 2048, 64};
		InvalidateRect(hWnd, &mousebox, FALSE);
		InvalidateRect(hWnd, &captionbox, FALSE);
		//InvalidateRect(hWnd, NULL, FALSE);
	}
		break;

	}
	return DefWindowProcW(hWnd, msg, wParam, lParam);
}

#define MSGF_SLEEPMSG 0x5300
#define MWFMO_WAITANY 0
BOOL SleepMsg(DWORD dwTimeout)
{
 DWORD dwStart = GetTickCount();
 DWORD dwElapsed;
 while ((dwElapsed = GetTickCount() - dwStart) < dwTimeout) {
  DWORD dwStatus = MsgWaitForMultipleObjectsEx(0, NULL,
                    dwTimeout - dwElapsed, QS_ALLINPUT,
                    MWFMO_WAITANY | MWMO_INPUTAVAILABLE);
  if (dwStatus == WAIT_OBJECT_0) {
   MSG msg;
   while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
    if (msg.message == WM_QUIT) {
     PostQuitMessage((int)msg.wParam);
     return FALSE; // abandoned due to WM_QUIT
    }
    if (!CallMsgFilter(&msg, MSGF_SLEEPMSG)) {
     TranslateMessage(&msg);
     DispatchMessage(&msg);
    }
   }
  }
 }
 return TRUE; // timed out
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hModule, LPSTR cmdline, int cmdShow) {
	const uint16_t CLASS_NAME[] = L"SimCityClass";
	WNDCLASSEXW wc = { };
	wc.cbSize = sizeof(wc);
	wc.lpfnWndProc = simCityWndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
	RegisterClassExW(&wc);
	(void)hModule;

	HWND hwnd = CreateWindowExW(0,                     // Optional window styles.
			CLASS_NAME,                     // Window class
			L"SimCity Windows 2018",        // Window text
			WS_OVERLAPPEDWINDOW,            // Window style

			// Size and position
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

			NULL,       // Parent window
			NULL,       // Menu
			hInstance,  // Instance handle
			NULL        // Additional application data
			);
	if (hwnd == NULL) {
		return 0;
	}

	ShowWindow(hwnd, cmdShow);

    env_init();

    char path[1024];
    sprintf(path, "%s/images/tiles.xpm", HomeDir);
    int x = ReadXpmFile(path, &tiles);
    printf("ReadXpmFile: %d\n", x);
    if (x)
    	return 1;

    sim = MakeNewSim();


    sim_init();

    extern void LoadScenario(int);
    LoadScenario(1);
    fflush(stdout);

    SetTimer(hwnd, 0, 100, timerFunc);
	MSG msg = {};
	printf("Looping\n");
#if 0
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
#else
	while (1) {
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			//printf("Peek done\n");
			if (msg.message == WM_QUIT) {
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			// idle here
			WaitMessage();
		}
	}
#endif

	sim_exit(0); // Just sets tkMustExit and ExitReturn

	return 0;
}


