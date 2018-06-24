/*
 * win_sim_cmds.c
 *
 *  Created on: Jun 10, 2018
 *      Author: Mike
 */

#include <windows.h>
#include <tcl.h>
#include <stddef.h>
#include <stdio.h>
#include "sim.h"
#include "macros.h"
#include "win_view.h"
#include "s_scan.h"
#include "w_sprite.h"
#include "s_sim.h"
#include "w_win.h"
#include "s_disast.h"
#include "w_update.h"
#include "s_traf.h"
#include "s_alloc.h"
#include "s_eval.h"
#include "g_smmaps.h"
#include "w_editor.h"
#include "w_x.h"
#include "w_stubs.h"
#include "s_msg.h"
#include "w_util.h"
#include "w_budget.h"
#include "w_tool.h"
#include "s_fileio.h"
#include "s_gen.h"
#include "w_sound.h"

Tcl_HashTable SimCmds;


#define SIMCMD_CALL(proc) \
  int SimCmd##proc(Tcl_Interp *interp, int argc, char **argv) { proc(); return (TCL_OK); }

#define SIMCMD_CALL_KICK(proc) \
  int SimCmd##proc(Tcl_Interp *interp, int argc, char **argv) { proc(); Kick(); return (TCL_OK); }

#define SIMCMD_CALL_INT(proc) \
  int SimCmd##proc(Tcl_Interp *interp, int argc, char **argv) { \
    int val; \
    if (argc != 3) return (TCL_ERROR); \
    if ((Tcl_GetInt(interp, argv[2], &val) != TCL_OK)) return (TCL_ERROR); \
    proc(val); \
    return (TCL_OK); \
  }

#define SIMCMD_CALL_STR(proc) \
  int SimCmd##proc(Tcl_Interp *interp, int argc, char **argv) { \
    if (argc != 3) return (TCL_ERROR); \
    proc(argv[2]); \
    return (TCL_OK); \
  }

#define SIMCMD_CALL_TILEXY(proc) \
  int SimCmd##proc(Tcl_Interp *interp, int argc, char **argv) { \
    int x, y; \
    if (argc != 4) return (TCL_ERROR); \
    if ((Tcl_GetInt(interp, argv[2], &x) != TCL_OK) || \
	(x < 0) || (x >= WORLD_X)) return (TCL_ERROR); \
    if ((Tcl_GetInt(interp, argv[3], &y) != TCL_OK) || \
	(y < 0) || (y >= WORLD_Y)) return (TCL_ERROR); \
    proc(x, y); \
    return (TCL_OK); \
  }

#define SIMCMD_ACCESS_INT(var) \
  int SimCmd##var(Tcl_Interp *interp, int argc, char **argv) { \
    int val; \
    if ((argc != 2) && (argc != 3)) return (TCL_ERROR); \
    if (argc == 3) { \
      if (Tcl_GetInt(interp, argv[2], &val) != TCL_OK) return (TCL_ERROR); \
      var = val; \
    } \
    Tcl_SetObjResult(interp, Tcl_NewIntObj(var)); \
    return (TCL_OK); \
  }

#define SIMCMD_GET_INT(var) \
  int SimCmd##var(Tcl_Interp *interp, int argc, char **argv) { \
    Tcl_SetObjResult(interp, Tcl_NewIntObj(var)); \
    return (TCL_OK); \
  }

#define SIMCMD_GET_STR(var) \
  int SimCmd##var(Tcl_Interp *interp, int argc, char **argv) { \
    Tcl_SetResult(interp, var, TCL_STATIC); \
    return (TCL_OK); \
  }


SIMCMD_CALL_KICK(GameStarted)
SIMCMD_CALL_KICK(InitGame)
SIMCMD_CALL(SaveCity)
SIMCMD_CALL(ReallyQuit)
SIMCMD_CALL_KICK(UpdateHeads)
SIMCMD_CALL_KICK(UpdateMaps)
SIMCMD_CALL_KICK(UpdateEditors)
SIMCMD_CALL_KICK(RedrawMaps)
SIMCMD_CALL_KICK(RedrawEditors)
SIMCMD_CALL_KICK(UpdateGraphs)
SIMCMD_CALL_KICK(UpdateEvaluation)
SIMCMD_CALL_KICK(UpdateBudget)
SIMCMD_CALL_KICK(UpdateBudgetWindow)
SIMCMD_CALL_KICK(DoBudget)
SIMCMD_CALL_KICK(DoBudgetFromMenu)
SIMCMD_CALL_KICK(Pause)
SIMCMD_CALL_KICK(Resume)
SIMCMD_CALL(StartBulldozer)
SIMCMD_CALL(StopBulldozer)
SIMCMD_CALL(MakeFire)
SIMCMD_CALL(MakeFlood)
SIMCMD_CALL(MakeTornado)
SIMCMD_CALL(MakeEarthquake)
SIMCMD_CALL(MakeMonster)
SIMCMD_CALL(MakeMeltdown)
SIMCMD_CALL(FireBomb)
SIMCMD_CALL(SoundOff)
SIMCMD_CALL(GenerateNewCity)
SIMCMD_CALL_INT(GenerateSomeCity)
SIMCMD_ACCESS_INT(LakeLevel)
SIMCMD_ACCESS_INT(TreeLevel)
SIMCMD_ACCESS_INT(CurveLevel)
SIMCMD_ACCESS_INT(CreateIsland)
SIMCMD_CALL_KICK(SmoothTrees)
SIMCMD_CALL_KICK(SmoothWater)
SIMCMD_CALL_KICK(SmoothRiver)
SIMCMD_CALL_KICK(ClearMap)
SIMCMD_CALL_KICK(ClearUnnatural)
SIMCMD_CALL_INT(LoadScenario)
SIMCMD_CALL_STR(LoadCity)
SIMCMD_CALL_STR(SaveCityAs)
SIMCMD_CALL_TILEXY(MakeExplosion)
SIMCMD_CALL(EraseOverlay)
SIMCMD_ACCESS_INT(OverRide)
SIMCMD_ACCESS_INT(Expensive)
SIMCMD_ACCESS_INT(Players)
SIMCMD_ACCESS_INT(Votes)
SIMCMD_ACCESS_INT(BobHeight)
SIMCMD_ACCESS_INT(PendingTool)
SIMCMD_ACCESS_INT(PendingX)
SIMCMD_ACCESS_INT(PendingY)
SIMCMD_GET_STR(Displays)


int SimCmdCityName(Tcl_Interp *interp, int argc, char **argv)
{
  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    setCityName(argv[2]);
  }

  Tcl_SetResult(interp, CityName, TCL_STATIC);
  return (TCL_OK);
}


int SimCmdCityFileName(Tcl_Interp *interp, int argc, char **argv)
{
  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if (CityFileName != NULL) {
      ckfree(CityFileName);
      CityFileName = NULL;
    }
    if (argv[2][0] != '\0') {
      CityFileName = (char *)ckalloc(strlen(argv[0]) + 1);
      strcpy(CityFileName, argv[2]);
    }
  }

  Tcl_SetResult(interp, CityFileName ? CityFileName : "", TCL_STATIC);
  return (TCL_OK);
}


int SimCmdGameLevel(Tcl_Interp *interp, int argc, char **argv)
{
  int level;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &level) != TCL_OK) ||
	(level < 0) || (level > 2)) {
      return (TCL_ERROR);
    }
    SetGameLevelFunds(level);
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj(GameLevel));
  return (TCL_OK);
}


int SimCmdSpeed(Tcl_Interp *interp, int argc, char **argv)
{
  int speed;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &speed) != TCL_OK) ||
	(speed < 0) || (speed > 7)) {
      return (TCL_ERROR);
    }
    setSpeed(speed); Kick();
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj(SimSpeed));
  return (TCL_OK);
}


int SimCmdSkips(Tcl_Interp *interp, int argc, char **argv)
{
  int skips;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &skips) != TCL_OK) ||
	(skips < 0)) {
      return (TCL_ERROR);
    }
    setSkips(skips); Kick();
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj(sim_skips));

  return (TCL_OK);
}


int SimCmdSkip(Tcl_Interp *interp, int argc, char **argv)
{
  int skip;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &skip) != TCL_OK) ||
	(skip < 0)) {
      return (TCL_ERROR);
    }
    sim_skip = skip;
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj(sim_skip));

  return (TCL_OK);
}


int SimCmdDelay(Tcl_Interp *interp, int argc, char **argv)
{
  int delay;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &delay) != TCL_OK) ||
	(delay < 0)) {
      return (TCL_ERROR);
    }
    sim_delay = delay; Kick();
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj(sim_delay));
  return (TCL_OK);
}


int SimCmdWorldX(Tcl_Interp *interp, int argc, char **argv)
{
  int val;

  if (argc != 2) {
    return (TCL_ERROR);
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj(WORLD_X));
  return (TCL_OK);
}


int SimCmdWorldY(Tcl_Interp *interp, int argc, char **argv)
{
  int val;

  if (argc != 2) {
    return (TCL_ERROR);
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj(WORLD_Y));
  return (TCL_OK);
}


int SimCmdHeatSteps(Tcl_Interp *interp, int argc, char **argv)
{
  int steps;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &steps) != TCL_OK) ||
	(steps < 0)) {
      return (TCL_ERROR);
    }
    heat_steps = steps; Kick();
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj(heat_steps));
  return (TCL_OK);
}


int SimCmdHeatFlow(Tcl_Interp *interp, int argc, char **argv)
{
  int flow;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if (Tcl_GetInt(interp, argv[2], &flow) != TCL_OK) {
      return (TCL_ERROR);
    }
    heat_flow = flow;
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj(heat_flow));
  return (TCL_OK);
}


int SimCmdHeatRule(Tcl_Interp *interp, int argc, char **argv)
{
  int rule;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if (Tcl_GetInt(interp, argv[2], &rule) != TCL_OK) {
      return (TCL_ERROR);
    }
    heat_rule = rule;
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj(heat_rule));
  return (TCL_OK);
}


#ifdef CAM

int SimCmdJustCam(Tcl_Interp *interp, int argc, char **argv)
{
  int cam;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if (Tcl_GetInt(interp, argv[2], &cam) != TCL_OK) {
      return (TCL_ERROR);
    }
    sim_just_cam = cam;
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj(sim_just_cam));
  return (TCL_OK);
}

#endif


#ifdef NET

int SimCmdListenTo(Tcl_Interp *interp, int argc, char **argv)
{
  int port, sock;

  if (argc != 3) {
    return (TCL_ERROR);
  }

  if (Tcl_GetInt(interp, argv[2], &port) != TCL_OK) {
    return (TCL_ERROR);
  }

#ifdef NET
  sock = udp_listen(port);
#endif

  Tcl_SetObjResult(interp, Tcl_NewIntObj(sock));

  return (TCL_OK);
}


int SimCmdHearFrom(Tcl_Interp *interp, int argc, char **argv)
{
  int sock;

  if (argc != 3) {
    return (TCL_ERROR);
  }

  if ((argv[2][0] != 'f') ||
      (argv[2][1] != 'i') ||
      (argv[2][2] != 'l') ||
      (argv[2][3] != 'e') ||
      (Tcl_GetInt(interp, argv[2] + 4, &sock) != TCL_OK)) {
    return (TCL_ERROR);
  }

#ifdef NET
  udp_hear(sock);
#endif

  return (TCL_OK);
}

#endif /* NET */


int SimCmdFunds(Tcl_Interp *interp, int argc, char **argv)
{
  int funds;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &funds) != TCL_OK) ||
	(funds < 0)) {
      return (TCL_ERROR);
    }
    TotalFunds = funds;
    MustUpdateFunds = 1;
    Kick();
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj(TotalFunds));
  return (TCL_OK);
}


int SimCmdTaxRate(Tcl_Interp *interp, int argc, char **argv)
{
  int tax;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &tax) != TCL_OK) ||
	(tax < 0) || (tax > 20)) {
      return (TCL_ERROR);
    }
    CityTax = tax;
    drawBudgetWindow(); Kick();
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj(CityTax));
  return (TCL_OK);
}


int SimCmdFireFund(Tcl_Interp *interp, int argc, char **argv)
{
  int percent;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &percent) != TCL_OK) ||
	(percent < 0) || (percent > 100)) {
      return (TCL_ERROR);
    }
    firePercent = percent / 100.0;
    FireSpend = (fireMaxValue * percent) / 100;
    UpdateFundEffects(); Kick();
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj((int)(firePercent * 100.0)));
  return (TCL_OK);
}


int SimCmdPoliceFund(Tcl_Interp *interp, int argc, char **argv)
{
  int percent;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &percent) != TCL_OK) ||
	(percent < 0) || (percent > 100)) {
      return (TCL_ERROR);
    }
    policePercent = percent / 100.0;
    PoliceSpend = (policeMaxValue * percent) / 100;
    UpdateFundEffects(); Kick();
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj((int)(policePercent * 100.0)));
  return (TCL_OK);
}


int SimCmdRoadFund(Tcl_Interp *interp, int argc, char **argv)
{
  int percent;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &percent) != TCL_OK) ||
	(percent < 0) || (percent > 100)) {
      return (TCL_ERROR);
    }
    roadPercent = percent / 100.0;
    RoadSpend = (roadMaxValue * percent) / 100;
    UpdateFundEffects(); Kick();
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj((int)(roadPercent * 100.0)));
  return (TCL_OK);
}


int SimCmdYear(Tcl_Interp *interp, int argc, char **argv)
{
  int year;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &year) != TCL_OK)) {
      return (TCL_ERROR);
    }
    SetYear(year);
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj(CurrentYear()));
  return (TCL_OK);
}


int SimCmdAutoBudget(Tcl_Interp *interp, int argc, char **argv)
{
  int val;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &val) != TCL_OK) ||
	(val < 0) || (val > 1)) {
      return (TCL_ERROR);
    }
    autoBudget = val;
    MustUpdateOptions = 1; Kick();
    UpdateBudget();
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj(autoBudget));
  return (TCL_OK);
}


int SimCmdAutoGoto(Tcl_Interp *interp, int argc, char **argv)
{
  int val;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &val) != TCL_OK) ||
	(val < 0) || (val > 1)) {
      return (TCL_ERROR);
    }
    autoGo = val;
    MustUpdateOptions = 1; Kick();
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj(autoGo));
  return (TCL_OK);
}


int SimCmdAutoBulldoze(Tcl_Interp *interp, int argc, char **argv)
{
  int val;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &val) != TCL_OK) ||
	(val < 0) || (val > 1)) {
      return (TCL_ERROR);
    }
    autoBulldoze = val;
    MustUpdateOptions = 1; Kick();
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj(autoBulldoze));
  return (TCL_OK);
}


int SimCmdDisasters(Tcl_Interp *interp, int argc, char **argv)
{
  int val;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &val) != TCL_OK) ||
	(val < 0) || (val > 1)) {
      return (TCL_ERROR);
    }
    NoDisasters = val ? 0 : 1;
    MustUpdateOptions = 1; Kick();
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj(NoDisasters ? 0 : 1));
  return (TCL_OK);
}


int SimCmdSound(Tcl_Interp *interp, int argc, char **argv)
{
  int val;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &val) != TCL_OK) ||
	(val < 0) || (val > 1)) {
      return (TCL_ERROR);
    }
    UserSoundOn = val;
    MustUpdateOptions = 1; Kick();
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj(UserSoundOn));
  return (TCL_OK);
}


int SimCmdFlush(Tcl_Interp *interp, int argc, char **argv)
{
  int style;

  if (argc != 2) {
    return (TCL_ERROR);
  }

  return (TCL_OK);
}


int SimCmdFlushStyle(Tcl_Interp *interp, int argc, char **argv)
{
  int style;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &style) != TCL_OK) ||
	(style < 0)) {
      return (TCL_ERROR);
    }
    FlushStyle = style;
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj(FlushStyle));
  return (TCL_OK);
}


int SimCmdDonDither(Tcl_Interp *interp, int argc, char **argv)
{
  int dd;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &dd) != TCL_OK) ||
	(dd < 0)) {
      return (TCL_ERROR);
    }
    DonDither = dd;
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj(DonDither));
  return (TCL_OK);
}


int SimCmdDoOverlay(Tcl_Interp *interp, int argc, char **argv)
{
  int dd;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &dd) != TCL_OK) ||
	(dd < 0)) {
      return (TCL_ERROR);
    }
    DoOverlay = dd;
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj(DoOverlay));
  return (TCL_OK);
}


int SimCmdMonsterGoal(Tcl_Interp *interp, int argc, char **argv)
{
  SimSprite *sprite;
  int x, y;

  if (argc != 4) {
    return (TCL_ERROR);
  }

  if (Tcl_GetInt(interp, argv[2], &x) != TCL_OK) {
    return (TCL_ERROR);
  }
  if (Tcl_GetInt(interp, argv[3], &y) != TCL_OK) {
    return (TCL_ERROR);
  }
  if ((sprite = GetSprite(GOD)) == NULL) {
    MakeMonster();
    if ((sprite = GetSprite(GOD)) == NULL)
      return (TCL_ERROR);
  }
  sprite->dest_x = x;
  sprite->dest_y = y;
  sprite->control = -2;
  sprite->count = -1;

  return (TCL_OK);
}


int SimCmdHelicopterGoal(Tcl_Interp *interp, int argc, char **argv)
{
  int x, y;
  SimSprite *sprite;

  if (argc != 4) {
    return (TCL_ERROR);
  }

  if (Tcl_GetInt(interp, argv[2], &x) != TCL_OK) {
    return (TCL_ERROR);
  }
  if (Tcl_GetInt(interp, argv[3], &y) != TCL_OK) {
    return (TCL_ERROR);
  }

  if ((sprite = GetSprite(COP)) == NULL) {
    GenerateCopter(x, y);
    if ((sprite = GetSprite(COP)) == NULL) {
      return (TCL_ERROR);
    }
  }
  sprite->dest_x = x;
  sprite->dest_y = y;

  return (TCL_OK);
}


int SimCmdMonsterDirection(Tcl_Interp *interp, int argc, char **argv)
{
  int dir;
  SimSprite *sprite;

  if (argc != 3) {
    return (TCL_ERROR);
  }

  if ((Tcl_GetInt(interp, argv[2], &dir) != TCL_OK) ||
      (dir < -1) || (dir > 7)) {
    return (TCL_ERROR);
  }
  if ((sprite = GetSprite(GOD)) == NULL) {
    MakeMonster();
    if ((sprite = GetSprite(GOD)) == NULL) {
      return (TCL_ERROR);
    }
  }
  sprite->control = dir;

  return (TCL_OK);
}


int SimCmdTile(Tcl_Interp *interp, int argc, char **argv)
{
  int x, y, tile;

  if ((argc != 4) && (argc != 5)) {
    return (TCL_ERROR);
  }
  if ((Tcl_GetInt(interp, argv[2], &x) != TCL_OK) ||
      (x < 0) ||
      (x >= WORLD_X) ||
      (Tcl_GetInt(interp, argv[3], &y) != TCL_OK) ||
      (y < 0) ||
      (y >= WORLD_Y)) {
    return (TCL_ERROR);
  }
  if (argc == 5) {
    if (Tcl_GetInt(interp, argv[4], &tile) != TCL_OK) {
      return (TCL_ERROR);
    }
    Map[x][y] = tile;
  }
  Tcl_SetObjResult(interp, Tcl_NewIntObj(Map[x][y]));
  return (TCL_OK);
}


int SimCmdFill(Tcl_Interp *interp, int argc, char **argv)
{
  int tile, x, y;

  if (argc != 3) {
    return (TCL_ERROR);
  }
  if (Tcl_GetInt(interp, argv[2], &tile) != TCL_OK) {
    return (TCL_ERROR);
  }
  for (x = 0; x < WORLD_X; x++) {
    for (y = 0; y < WORLD_Y; y++) {
      Map[x][y] = tile;
    }
  }
  Tcl_SetObjResult(interp, Tcl_NewIntObj(tile));
  return (TCL_OK);
}


int SimCmdDynamicData(Tcl_Interp *interp, int argc, char **argv)
{
  int index, val;

  if ((argc != 3) && (argc != 4)) {
    return (TCL_ERROR);
  }

  if ((Tcl_GetInt(interp, argv[2], &index) != TCL_OK) ||
      (index < 0) ||
      (index >= 32)) {
    return (TCL_ERROR);
  }

  if (argc == 4) {
    int val;

    if (Tcl_GetInt(interp, argv[3], &val) != TCL_OK) {
      return (TCL_ERROR);
    }
    DynamicData[index] = val;
    NewMapFlags[DYMAP] = 1;
    Kick();
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj(DynamicData[index]));
  return (TCL_OK);
}


int SimCmdResetDynamic(Tcl_Interp *interp, int argc, char **argv)
{
  int i;

  for (i = 0; i < 16; i++) {
    DynamicData[i] = (i & 1) ? 99999 : -99999;
  }
  NewMapFlags[DYMAP] = 1;
  Kick();
  return (TCL_OK);
}


int SimCmdPerformance(Tcl_Interp *interp, int argc, char **argv)
{
  SimView *view;

  PerformanceTiming = 1;
  FlushTime = 0.0;
  for (view = sim->editor; view != NULL; view = view->next) {
    view->updates = 0;
    view->update_real = view->update_user = view->update_system = 0.0;
  }
  return (TCL_OK);
}


int SimCmdCollapseMotion(Tcl_Interp *interp, int argc, char **argv)
{
  int val;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &val) != TCL_OK)) {
      return (TCL_ERROR);
    }
#if 0
    tkCollapseMotion = val;
#else
    ;
#endif
  }

#if 0
  Tcl_SetObjResult(interp, Tcl_NewIntObj(tkCollapseMotion));
#else
  Tcl_SetObjResult(interp, Tcl_NewIntObj(val));
#endif
  return (TCL_OK);
}


int SimCmdUpdate(Tcl_Interp *interp, int argc, char **argv)
{
  sim_update();
  return (TCL_OK);
}


int SimCmdLandValue(Tcl_Interp *interp, int argc, char **argv)
{
  int val;

  if (argc != 2) {
    return (TCL_ERROR);
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj(LVAverage));
  return (TCL_OK);
}


int SimCmdTraffic(Tcl_Interp *interp, int argc, char **argv)
{
  int val;

  if (argc != 2) {
    return (TCL_ERROR);
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj(AverageTrf()));
  return (TCL_OK);
}


int SimCmdCrime(Tcl_Interp *interp, int argc, char **argv)
{
  int val;

  if (argc != 2) {
    return (TCL_ERROR);
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj(CrimeAverage));
  return (TCL_OK);
}


int SimCmdUnemployment(Tcl_Interp *interp, int argc, char **argv)
{
  int val;

  if (argc != 2) {
    return (TCL_ERROR);
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj(GetUnemployment()));
  return (TCL_OK);
}


int SimCmdFires(Tcl_Interp *interp, int argc, char **argv)
{
  int val;

  if (argc != 2) {
    return (TCL_ERROR);
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj(GetFire()));
  return (TCL_OK);
}


int SimCmdPollution(Tcl_Interp *interp, int argc, char **argv)
{
  int val;

  if (argc != 2) {
    return (TCL_ERROR);
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj(PolluteAverage));
  return (TCL_OK);
}


int SimCmdPolMaxX(Tcl_Interp *interp, int argc, char **argv)
{
  int val;

  if (argc != 2) {
    return (TCL_ERROR);
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj((PolMaxX <<4) + 8));
  return (TCL_OK);
}


int SimCmdPolMaxY(Tcl_Interp *interp, int argc, char **argv)
{
  int val;

  if (argc != 2) {
    return (TCL_ERROR);
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj((PolMaxY <<4) + 8));
  return (TCL_OK);
}


int SimCmdTrafMaxX(Tcl_Interp *interp, int argc, char **argv)
{
  int val;

  if (argc != 2) {
    return (TCL_ERROR);
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj(TrafMaxX));
  return (TCL_OK);
}


int SimCmdTrafMaxY(Tcl_Interp *interp, int argc, char **argv)
{
  int val;

  if (argc != 2) {
    return (TCL_ERROR);
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj(TrafMaxY));
  return (TCL_OK);
}


int SimCmdMeltX(Tcl_Interp *interp, int argc, char **argv)
{
  int val;

  if (argc != 2) {
    return (TCL_ERROR);
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj((MeltX <<4) + 8));
  return (TCL_OK);
}


int SimCmdMeltY(Tcl_Interp *interp, int argc, char **argv)
{
  int val;

  if (argc != 2) {
    return (TCL_ERROR);
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj((MeltY <<4) + 8));
  return (TCL_OK);
}


int SimCmdCrimeMaxX(Tcl_Interp *interp, int argc, char **argv)
{
  int val;

  if (argc != 2) {
    return (TCL_ERROR);
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj((CrimeMaxX <<4) + 8));
  return (TCL_OK);
}


int SimCmdCrimeMaxY(Tcl_Interp *interp, int argc, char **argv)
{
  int val;

  if (argc != 2) {
    return (TCL_ERROR);
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj((CrimeMaxY <<4) + 8));
  return (TCL_OK);
}


int SimCmdCenterX(Tcl_Interp *interp, int argc, char **argv)
{
  int val;

  if (argc != 2) {
    return (TCL_ERROR);
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj((CCx <<4) + 8));
  return (TCL_OK);
}


int SimCmdCenterY(Tcl_Interp *interp, int argc, char **argv)
{
  int val;

  if (argc != 2) {
    return (TCL_ERROR);
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj((CCy <<4) + 8));
  return (TCL_OK);
}


int SimCmdFloodX(Tcl_Interp *interp, int argc, char **argv)
{
  int val;

  if (argc != 2) {
    return (TCL_ERROR);
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj((FloodX <<4) + 8));
  return (TCL_OK);
}


int SimCmdFloodY(Tcl_Interp *interp, int argc, char **argv)
{
  int val;

  if (argc != 2) {
    return (TCL_ERROR);
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj((FloodY <<4) + 8));
  return (TCL_OK);
}


int SimCmdCrashX(Tcl_Interp *interp, int argc, char **argv)
{
  int val;

  if (argc != 2) {
    return (TCL_ERROR);
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj((CrashX <<4) + 8));
  return (TCL_OK);
}


int SimCmdCrashY(Tcl_Interp *interp, int argc, char **argv)
{
  int val;

  if (argc != 2) {
    return (TCL_ERROR);
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj((CrashY <<4) + 8));
  return (TCL_OK);
}


int SimCmdDollars(Tcl_Interp *interp, int argc, char **argv)
{
  int val;

  if (argc != 2) {
    return (TCL_ERROR);
  }
  char* t = Tcl_Alloc((strlen(argv[1])*4+3)/3);
  makeDollarDecimalStr(argv[1], t);
  Tcl_SetResult(interp, t, TCL_DYNAMIC);
  return (TCL_OK);
}


int SimCmdDoAnimation(Tcl_Interp *interp, int argc, char **argv)
{
  int val;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &val) != TCL_OK)) {
      return (TCL_ERROR);
    }
    DoAnimation = val;
    MustUpdateOptions = 1; Kick();
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj(DoAnimation));
  return (TCL_OK);
}


int SimCmdDoMessages(Tcl_Interp *interp, int argc, char **argv)
{
  int val;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &val) != TCL_OK)) {
      return (TCL_ERROR);
    }
    DoMessages = val;
    MustUpdateOptions = 1; Kick();
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj(DoMessages));
  return (TCL_OK);
}


int SimCmdDoNotices(Tcl_Interp *interp, int argc, char **argv)
{
  int val;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &val) != TCL_OK)) {
      return (TCL_ERROR);
    }
    DoNotices = val;
    MustUpdateOptions = 1; Kick();
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj(DoNotices));
  return (TCL_OK);
}


int SimCmdRand(Tcl_Interp *interp, int argc, char **argv)
{
  int val, r;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &val) != TCL_OK)) {
      return (TCL_ERROR);
    }
    r = Rand(val);
  } else {
    r = Rand16();
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj(r));
  return (TCL_OK);
}


int SimCmdPlatform(Tcl_Interp *interp, int argc, char **argv)
{

#ifdef MSDOS
  Tcl_SetResult(interp, "msdos", TCL_STATIC);
#else
  Tcl_SetResult(interp, "unix", TCL_STATIC);
#endif

  return (TCL_OK);
}


int SimCmdVersion(Tcl_Interp *interp, int argc, char **argv)
{
	Tcl_SetResult(interp, MicropolisVersion, TCL_STATIC);

  return (TCL_OK);
}


int SimCmdOpenWebBrowser(Tcl_Interp *interp, int argc, char **argv)
{
  int result = 1;
  char buf[512];

  if ((argc != 3) ||
      (strlen(argv[2]) > 255)) {
    return (TCL_ERROR);
  }

  sprintf(buf,
	  "netscape -no-about-splash '%s' &",
	  argv[2]);

  result = system(buf);

  Tcl_SetObjResult(interp, Tcl_NewIntObj(result));

  return (TCL_OK);
}


int SimCmdQuoteURL(Tcl_Interp *interp, int argc, char **argv)
{
  int result = 1;
  char buf[2048];
  char *from, *to;
  int ch;
  static char *hexDigits =
    "0123456789ABCDEF";

  if ((argc != 3) ||
      (strlen(argv[2]) > 255)) {
    return (TCL_ERROR);
  }

  from = argv[2];
  to = buf;

  while ((ch = *(from++)) != '\0') {
    if ((ch < 32) ||
	(ch >= 128) ||
	(ch == '+') ||
	(ch == '%') ||
	(ch == '&') ||
	(ch == '<') ||
	(ch == '>') ||
	(ch == '"') ||
	(ch == '\'')) {
      *to++ = '%';
      *to++ = hexDigits[(ch >> 4) & 0x0f];
      *to++ = hexDigits[ch & 0x0f];
    } else if (ch == 32) {
      *to++ = '+';
    } else {
      *to++ = ch;
    } // if
  } // while

  *to = '\0';

  Tcl_SetResult(interp, buf, TCL_VOLATILE);

  return (TCL_OK);
}


int SimCmdNeedRest(Tcl_Interp *interp, int argc, char **argv)
{
  int needRest;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if (Tcl_GetInt(interp, argv[2], &needRest) != TCL_OK) {
      return (TCL_ERROR);
    }
    NeedRest = needRest;
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj(NeedRest));
  return (TCL_OK);
}


int SimCmdMultiPlayerMode(Tcl_Interp *interp, int argc, char **argv)
{
  /* This is read-only because it's specified on
     the command line and effects how the user
     interface is initialized. */

  if (argc != 2) {
    return (TCL_ERROR);
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj(MultiPlayerMode));
  return (TCL_OK);
}


int SimCmdSugarMode(Tcl_Interp *interp, int argc, char **argv)
{
  /* This is read-only because it's specified on
     the command line and effects how the user
     interface is initialized. */

  if (argc != 2) {
    return (TCL_ERROR);
  }

  Tcl_SetObjResult(interp, Tcl_NewIntObj(SugarMode));
  return (TCL_OK);
}


/************************************************************************/

int
SimCmd(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv)
{
  Tcl_HashEntry *ent;
  int result = TCL_OK;
  int (*cmd)();

  if (argc < 2) {
    return TCL_ERROR;
  }

  if ((ent = Tcl_FindHashEntry(&SimCmds, argv[1]))) {
    cmd = (int (*)())ent->clientData;
    result = cmd(interp, argc, argv);
  } else {
    result = TCL_ERROR;
  }
  return result;
}


void sim_command_init(void)
{
  int new;

  Tcl_CreateCommand(tk_mainInterp, "sim", SimCmd,
		    (ClientData)MainWindow, (void (*)()) NULL);

  Tcl_InitHashTable(&SimCmds, TCL_STRING_KEYS);

#define SIM_CMD(name) HASHED_CMD(Sim, name)

  SIM_CMD(GameStarted);
  SIM_CMD(InitGame);
  SIM_CMD(SaveCity);
  SIM_CMD(ReallyQuit);
  SIM_CMD(UpdateHeads);
  SIM_CMD(UpdateMaps);
  SIM_CMD(RedrawEditors);
  SIM_CMD(RedrawMaps);
  SIM_CMD(UpdateEditors);
  SIM_CMD(UpdateGraphs);
  SIM_CMD(UpdateEvaluation);
  SIM_CMD(UpdateBudget);
  SIM_CMD(UpdateBudgetWindow);
  SIM_CMD(DoBudget);
  SIM_CMD(DoBudgetFromMenu);
  SIM_CMD(Pause);
  SIM_CMD(Resume);
  SIM_CMD(StartBulldozer);
  SIM_CMD(StopBulldozer);
  SIM_CMD(MakeFire);
  SIM_CMD(MakeFlood);
  SIM_CMD(MakeTornado);
  SIM_CMD(MakeEarthquake);
  SIM_CMD(MakeMonster);
  SIM_CMD(MakeMeltdown);
  SIM_CMD(FireBomb);
  SIM_CMD(SoundOff);
  SIM_CMD(GenerateNewCity);
  SIM_CMD(GenerateSomeCity);
  SIM_CMD(TreeLevel);
  SIM_CMD(LakeLevel);
  SIM_CMD(CurveLevel);
  SIM_CMD(CreateIsland);
  SIM_CMD(ClearMap);
  SIM_CMD(ClearUnnatural);
  SIM_CMD(SmoothTrees);
  SIM_CMD(SmoothWater);
  SIM_CMD(SmoothRiver);
  SIM_CMD(LoadScenario);
  SIM_CMD(LoadCity);
  SIM_CMD(SaveCityAs);
  SIM_CMD(MakeExplosion);
  SIM_CMD(CityName);
  SIM_CMD(CityFileName);
  SIM_CMD(GameLevel);
  SIM_CMD(Speed);
  SIM_CMD(Skips);
  SIM_CMD(Skip);
  SIM_CMD(WorldX);
  SIM_CMD(WorldY);
  SIM_CMD(Delay);
  SIM_CMD(HeatSteps);
  SIM_CMD(HeatFlow);
  SIM_CMD(HeatRule);
#ifdef CAM
  SIM_CMD(JustCam);
#endif
#ifdef NET
  SIM_CMD(ListenTo);
  SIM_CMD(HearFrom);
#endif
  SIM_CMD(Funds);
  SIM_CMD(TaxRate);
  SIM_CMD(FireFund);
  SIM_CMD(PoliceFund);
  SIM_CMD(RoadFund);
  SIM_CMD(Year);
  SIM_CMD(AutoBudget);
  SIM_CMD(AutoGoto);
  SIM_CMD(AutoBulldoze);
  SIM_CMD(Disasters);
  SIM_CMD(Sound);
  SIM_CMD(Flush);
  SIM_CMD(FlushStyle);
  SIM_CMD(DonDither);
  SIM_CMD(DoOverlay);
  SIM_CMD(MonsterGoal);
  SIM_CMD(HelicopterGoal);
  SIM_CMD(MonsterDirection);
  SIM_CMD(EraseOverlay);
  SIM_CMD(Tile);
  SIM_CMD(Fill);
  SIM_CMD(DynamicData);
  SIM_CMD(ResetDynamic);
  SIM_CMD(Performance);
  SIM_CMD(CollapseMotion);
  SIM_CMD(Update);
  SIM_CMD(OverRide);
  SIM_CMD(Expensive);
  SIM_CMD(Players);
  SIM_CMD(Votes);
  SIM_CMD(BobHeight);
  SIM_CMD(PendingTool);
  SIM_CMD(PendingX);
  SIM_CMD(PendingY);
  SIM_CMD(Displays);
  SIM_CMD(LandValue);
  SIM_CMD(Traffic);
  SIM_CMD(Crime);
  SIM_CMD(Unemployment);
  SIM_CMD(Fires);
  SIM_CMD(Pollution);
  SIM_CMD(PolMaxX);
  SIM_CMD(PolMaxY);
  SIM_CMD(TrafMaxX);
  SIM_CMD(TrafMaxY);
  SIM_CMD(MeltX);
  SIM_CMD(MeltY);
  SIM_CMD(CrimeMaxX);
  SIM_CMD(CrimeMaxY);
  SIM_CMD(CenterX);
  SIM_CMD(CenterY);
  SIM_CMD(FloodX);
  SIM_CMD(FloodY);
  SIM_CMD(CrashX);
  SIM_CMD(CrashY);
  SIM_CMD(Dollars);
  SIM_CMD(DoAnimation);
  SIM_CMD(DoMessages);
  SIM_CMD(DoNotices);
  SIM_CMD(Rand);
  SIM_CMD(Platform);
  SIM_CMD(Version);
  SIM_CMD(OpenWebBrowser);
  SIM_CMD(QuoteURL);
  SIM_CMD(NeedRest);
  SIM_CMD(MultiPlayerMode);
  SIM_CMD(SugarMode);
}
