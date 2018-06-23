/*
 * win_sim.c
 *
 *  Created on: Jun 9, 2018
 *      Author: Mike
 */
#include <windows.h>
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
  TESTDIR(ResourceDir, "$SIMHOME/res");

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
  ChangeEval();
  MessagePort = 0;
  MesX = 0;
  MesY = 0;
  sim_paused = 0;
  sim_loops = 0;
  InitSimLoad = 2;
  //tkMustExit = 0;
  ExitReturn = 0;

  InitializeSound();
  initMapArrays();
  initGraphs();
  InitFundingLevel();
#if 0
  loadObjectData();
#endif
  setUpMapProcs();
  StopEarthquake();
  ResetMapState();
  ResetEditorState();
  ClearMap();
  InitWillStuff();
  SetFunds(5000);
  SetGameLevelFunds(StartupGameLevel);
  setSpeed(0);
  setSkips(0);
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



LRESULT simCityWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW+1));
		for (int y=0; y<120; y++) {
#define FACTOR 20
			RECT cell; cell.top = y*FACTOR; cell.bottom=y*FACTOR+FACTOR;
			for (int x=0; x<120; x++) {
				cell.left = x*FACTOR; cell.right = cell.left+FACTOR;
				HBRUSH hbr = CreateSolidBrush(RGB(255*((Map[x][y]&0xF800)>>11)/31,255*((Map[x][y]&0x07E0)>>5)/63,255*(Map[x][y]&0x1F)/31));
				FillRect(hdc, &cell, hbr);
				DeleteObject(hbr);
			}
		}
		EndPaint(hWnd, &ps);
		return 0;
	}
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

    sim = MakeNewSim();


    sim_init();

    extern void LoadScenario(int);
    LoadScenario(1);
    fflush(stdout);

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
			printf("Peek done\n");
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


