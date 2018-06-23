/*
 * w_stubs.h
 *
 *  Created on: Jun 3, 2018
 *      Author: Mike
 */

#ifndef SRC_SIM_HEADERS_W_STUBS_H_
#define SRC_SIM_HEADERS_W_STUBS_H_

#include <stdint.h>

extern short UserSoundOn;
extern short ScenarioID;
extern short NoDisasters;
extern short PunishCnt;
extern short autoBulldoze, autoBudget;
extern short MesNum;
extern int32_t LastMesTime;
extern short flagBlink;
extern short SimSpeed;
extern short InitSimLoad;
extern short GameLevel;
extern int32_t TotalFunds;
extern short SimMetaSpeed;
extern char *CityName;
extern short EvalChanged;
extern int32_t TotalFunds;

void SetFunds(int dollars);
void DropFireBombs(void);
int32_t TickCount();
void Spend(int dollars);
void GameStarted(void);
void ReallyQuit(void);
void InitGame(void);


#endif /* SRC_SIM_HEADERS_W_STUBS_H_ */
