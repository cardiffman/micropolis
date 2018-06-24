/*
 * s_alloc.h
 *
 *  Created on: Jun 3, 2018
 *      Author: Mike
 */

#ifndef SRC_SIM_HEADERS_S_ALLOC_H_
#define SRC_SIM_HEADERS_S_ALLOC_H_

#include "sim.h"

#include <stdint.h>

extern short StartingYear;
extern unsigned char tileSynch;
extern short CityTax;
extern short CChr, CChr9;
extern int32_t CityTime;
extern short MessagePort;
extern short MesX, MesY;
extern short *Map[/*WORLD_X*/];		/* Main Map 120 x 100  */
extern short CrimeAverage, PolluteAverage, LVAverage;
extern short SMapX, SMapY;
extern short RoadEffect, PoliceEffect, FireEffect;
extern short ResPop, ComPop, IndPop, TotalPop, LastTotalPop;
extern short RoadTotal, RailTotal, FirePop;
extern short PolicePop, FireStPop;
extern short HospPop, ChurchPop, StadiumPop;
extern short CoalPop, NuclearPop, PortPop, APortPop;
extern short ResZPop, ComZPop, IndZPop, TotalZPop;
extern uint8_t *LandValueMem[];
extern uint8_t *TrfDensity[];
extern int32_t RoadFund, PoliceFund, FireFund;
extern short CityTax, TaxFlag;
extern uint8_t *PopDensity[];
extern uint8_t *TrfDensity[];
extern uint8_t *PollutionMem[];
extern uint8_t *CrimeMem[];
extern uint8_t *TerrainMem[];
extern uint8_t *tem[];
extern uint8_t *tem2[];
extern uint8_t *Qtem[];
extern short *PowerMap;
extern short *ResHis;
extern short *ComHis;
extern short *IndHis;
extern short *MoneyHis;
extern short *CrimeHis;
extern short *PollutionHis;
extern short *MiscHis;
extern int32_t TaxFund;
extern short ResHisMax, Res2HisMax;
extern short ComHisMax, Com2HisMax;
extern short IndHisMax, Ind2HisMax;
extern short NeedHosp, NeedChurch;
extern int32_t RoadSpend, PoliceSpend, FireSpend;
extern short CensusChanged;

extern short RateOGMem[SmX][SmY];
extern short FireStMap[SmX][SmY];
extern short FireRate[SmX][SmY];
extern short ComRate[SmX][SmY];
extern short PoliceMap[SmX][SmY];
extern short PoliceMapEffect[SmX][SmY];
extern short STem[SmX][SmY];

void initMapArrays(void);


#endif /* SRC_SIM_HEADERS_S_ALLOC_H_ */
