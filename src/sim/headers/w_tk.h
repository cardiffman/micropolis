/*
 * w_tk.h
 *
 *  Created on: Jun 3, 2018
 *      Author: Mike
 */

#ifndef SRC_SIM_HEADERS_W_TK_H_
#define SRC_SIM_HEADERS_W_TK_H_

#include <tk.h>
extern int UpdateDelayed;
typedef struct Tcl_Interp Tcl_Interp;
extern Tcl_Interp *tk_mainInterp;
extern Tk_Window MainWindow;
extern int NeedRest;
extern double FlushTime;
extern int PerformanceTiming;

void StopEarthquake(void);
void DoEarthQuake(void);
typedef struct SimView SimView;
void CancelRedrawView(SimView *view);
void tk_main(void);
void InvalidateEditors(void);
void InvalidateMaps(void);
int Eval(char *buf);
void Kick(void);
void StartMicropolisTimer(void);
void StopMicropolisTimer(void);
void EventuallyRedrawView(SimView *view);
void RedrawMaps(void);
void StopToolkit(void);
void RedrawEditors(void);
void DidStopPan(SimView *view);
int ConfigureTileView(Tcl_Interp *interp, SimView *view, int argc, char **argv, int flags);



#endif /* SRC_SIM_HEADERS_W_TK_H_ */
