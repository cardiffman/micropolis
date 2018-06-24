/*
 * w_x.h
 *
 *  Created on: Jun 3, 2018
 *      Author: Mike
 */

#ifndef SRC_SIM_HEADERS_W_X_H_
#define SRC_SIM_HEADERS_W_X_H_


extern struct XDisplay *XDisplays;
extern int FlushStyle;

void DoStopMicropolis(void);
void UpdateFlush(void);
void DoTimeoutListen(void);

typedef struct XDisplay XDisplay;
void IncRefDisplay(XDisplay *xd);
void DecRefDisplay(XDisplay *xd);
typedef struct SimView SimView;
void ViewToPixelCoords(SimView *view, int x, int y, int *outx, int *outy);
void ViewToTileCoords(SimView *view, int x, int y, int *outx, int *outy);
typedef struct Ink Ink;
void FreeInk(Ink *ink);
void AddInk(Ink *ink, int x, int y);
void StartInk(Ink *ink, int x, int y);

SimView *
InitNewView(SimView *view, char *title, int class, int w, int h);
typedef struct Sim Sim;
Sim * MakeNewSim(void);
void DestroyView(SimView *view);
void DoResizeView(SimView *view, int w, int h);

void DoPanBy(struct SimView *view, int dx, int dy);
void DoPanTo(struct SimView *view, int x, int y);
void EraseOverlay(void);



#endif /* SRC_SIM_HEADERS_W_X_H_ */
