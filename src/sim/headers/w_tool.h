/*
 * w_tool.h
 *
 *  Created on: Jun 3, 2018
 *      Author: Mike
 */

#ifndef SRC_SIM_W_TOOL_H_
#define SRC_SIM_W_TOOL_H_
#include <stdint.h>

extern int OverRide;
extern int PendingTool;
extern int PendingX;
extern int PendingY;
extern int Votes;
extern int Players;
extern int Expensive;
extern short toolSize[];
extern short toolOffset[];
extern int32_t toolColors[];

typedef struct SimView SimView;
void setWandState(SimView *view, short state);
void ChalkStart(SimView *view, int x, int y, int color);
void ChalkTo(SimView *view, int x, int y);
int ToolUp(SimView *view, int x, int y);
void ToolDown(SimView *view, int x, int y);
void ToolDrag(SimView *view, int px, int py);
void DoTool(SimView *view, short tool, short x, short y);
short
tally(short tileValue);
int
bulldozer_tool(SimView *view, short x, short y);



#endif /* SRC_SIM_W_TOOL_H_ */
