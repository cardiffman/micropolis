/*
 * s_sim.h
 *
 *  Created on: Jun 3, 2018
 *      Author: Mike
 */

#ifndef SRC_SIM_HEADERS_S_SIM_H_
#define SRC_SIM_HEADERS_S_SIM_H_

extern short DisasterEvent;
extern short DisasterWait;
extern short DoInitialEval;
extern short ResCap, ComCap, IndCap;
extern short RValve, CValve, IValve;
extern short unPwrdZCnt;
extern short PwrdZCnt;
extern short ValveFlag;
extern short ScoreType;
extern short ScoreWait;
extern short MeltX, MeltY;

void SimFrame(void);
short Rand(short range);
void DoMeltdown(int SX, int SY);
int Rand16(void);
short Rand(short range);
int Rand16Signed(void);
void FireZone(int Xloc, int Yloc, int ch);
void RandomlySeedRand(void);
void SeedRand(int seed);
void DoSimInit(void);
void DoSPZone(short PwrOn);
void RepairZone(short ZCent, short zsize);
void UpdateFundEffects(void);



#endif /* SRC_SIM_HEADERS_S_SIM_H_ */
