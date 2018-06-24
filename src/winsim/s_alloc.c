/* s_alloc.c
 *
 * Micropolis, Unix Version.  This game was released for the Unix platform
 * in or about 1990 and has been modified for inclusion in the One Laptop
 * Per Child program.  Copyright (C) 1989 - 2007 Electronic Arts Inc.  If
 * you need assistance with this program, you may contact:
 *   http://wiki.laptop.org/go/Micropolis  or email  micropolis@laptop.org.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.  You should have received a
 * copy of the GNU General Public License along with this program.  If
 * not, see <http://www.gnu.org/licenses/>.
 * 
 *             ADDITIONAL TERMS per GNU GPL Section 7
 * 
 * No trademark or publicity rights are granted.  This license does NOT
 * give you any right, title or interest in the trademark SimCity or any
 * other Electronic Arts trademark.  You may not distribute any
 * modification of this program using the trademark SimCity or claim any
 * affliation or association with Electronic Arts Inc. or its employees.
 * 
 * Any propagation or conveyance of this program must include this
 * copyright notice and these terms.
 * 
 * If you convey this program (or any modifications of it) and assume
 * contractual liability for the program to recipients of it, you agree
 * to indemnify Electronic Arts for any liability that those contractual
 * assumptions impose on Electronic Arts.
 * 
 * You may not misrepresent the origins of this program; modified
 * versions of the program must be marked as such and not identified as
 * the original program.
 * 
 * This disclaimer supplements the one included in the General Public
 * License.  TO THE FULLEST EXTENT PERMISSIBLE UNDER APPLICABLE LAW, THIS
 * PROGRAM IS PROVIDED TO YOU "AS IS," WITH ALL FAULTS, WITHOUT WARRANTY
 * OF ANY KIND, AND YOUR USE IS AT YOUR SOLE RISK.  THE ENTIRE RISK OF
 * SATISFACTORY QUALITY AND PERFORMANCE RESIDES WITH YOU.  ELECTRONIC ARTS
 * DISCLAIMS ANY AND ALL EXPRESS, IMPLIED OR STATUTORY WARRANTIES,
 * INCLUDING IMPLIED WARRANTIES OF MERCHANTABILITY, SATISFACTORY QUALITY,
 * FITNESS FOR A PARTICULAR PURPOSE, NONINFRINGEMENT OF THIRD PARTY
 * RIGHTS, AND WARRANTIES (IF ANY) ARISING FROM A COURSE OF DEALING,
 * USAGE, OR TRADE PRACTICE.  ELECTRONIC ARTS DOES NOT WARRANT AGAINST
 * INTERFERENCE WITH YOUR ENJOYMENT OF THE PROGRAM; THAT THE PROGRAM WILL
 * MEET YOUR REQUIREMENTS; THAT OPERATION OF THE PROGRAM WILL BE
 * UNINTERRUPTED OR ERROR-FREE, OR THAT THE PROGRAM WILL BE COMPATIBLE
 * WITH THIRD PARTY SOFTWARE OR THAT ANY ERRORS IN THE PROGRAM WILL BE
 * CORRECTED.  NO ORAL OR WRITTEN ADVICE PROVIDED BY ELECTRONIC ARTS OR
 * ANY AUTHORIZED REPRESENTATIVE SHALL CREATE A WARRANTY.  SOME
 * JURISDICTIONS DO NOT ALLOW THE EXCLUSION OF OR LIMITATIONS ON IMPLIED
 * WARRANTIES OR THE LIMITATIONS ON THE APPLICABLE STATUTORY RIGHTS OF A
 * CONSUMER, SO SOME OR ALL OF THE ABOVE EXCLUSIONS AND LIMITATIONS MAY
 * NOT APPLY TO YOU.
 */
#include "s_alloc.h"
#include "mac.h"
#include "sim.h"
#include <stdlib.h>

/* Allocate Stuff */


short SMapX, SMapY;
short CChr, CChr9;

short RoadTotal, RailTotal, FirePop;

short ResPop, ComPop, IndPop, TotalPop, LastTotalPop;
short ResZPop, ComZPop, IndZPop, TotalZPop;
short HospPop, ChurchPop, StadiumPop;
short PolicePop, FireStPop;
short CoalPop, NuclearPop, PortPop, APortPop;

short NeedHosp, NeedChurch;
short CrimeAverage, PolluteAverage, LVAverage;

int32_t CityTime;
short StartingYear;

short *Map[WORLD_X];

short ResHisMax, Res2HisMax;
short ComHisMax, Com2HisMax;
short IndHisMax, Ind2HisMax;
short CensusChanged;

short MessagePort;
short MesX, MesY;

int32_t RoadSpend, PoliceSpend, FireSpend;
int32_t RoadFund, PoliceFund, FireFund;
short RoadEffect, PoliceEffect, FireEffect;
int32_t TaxFund;
short CityTax, TaxFlag;
unsigned char tileSynch = 0x01;

uint8_t *PopDensity[HWLDX];
uint8_t *TrfDensity[HWLDX];
uint8_t *PollutionMem[HWLDX];
uint8_t *LandValueMem[HWLDX];
uint8_t *CrimeMem[HWLDX];
uint8_t *tem[HWLDX];
uint8_t *tem2[HWLDX];
uint8_t *TerrainMem[QWX];
uint8_t *Qtem[QWX];

short RateOGMem[SmX][SmY];
short FireStMap[SmX][SmY];
short PoliceMap[SmX][SmY];
short PoliceMapEffect[SmX][SmY];

short FireRate[SmX][SmY];
short ComRate[SmX][SmY];
short STem[SmX][SmY];

Ptr terrainBase;
Ptr qTemBase;
Ptr tem1Base;
Ptr tem2Base;

Ptr popPtr;
Ptr trfPtr;
Ptr polPtr;
Ptr landPtr;
Ptr crimePtr;
	
Ptr auxPopPtr;
Ptr auxTrfPtr;
Ptr auxPolPtr;
Ptr auxLandPtr;
Ptr auxCrimePtr;
Ptr brettPtr;

unsigned short *mapPtr;
short *ResHis;
short *ComHis;
short *IndHis;
short *MoneyHis;
short *PollutionHis;
short *CrimeHis;
short *MiscHis;
short *PowerMap;


void initMapArrays(void)
{
  short i;
  unsigned short *auxPtr, *temp1;

  if (!mapPtr)
    mapPtr = calloc(WORLD_X * WORLD_Y,sizeof(unsigned short));
  auxPtr = mapPtr;

  for (i = 0; i < WORLD_X; i++) {
    temp1 = auxPtr + i * WORLD_Y;
    Map[i] = (short *)temp1;
  }

  popPtr = calloc(HWLDX * HWLDY,1);
  trfPtr = calloc(HWLDX * HWLDY,1);
  polPtr = calloc(HWLDX * HWLDY,1);
  landPtr = calloc(HWLDX * HWLDY,1);
  crimePtr = calloc(HWLDX * HWLDY,1);

  tem1Base = calloc(HWLDX * HWLDY,1);
  tem2Base = calloc(HWLDX * HWLDY,1);

  auxPopPtr = popPtr;
  auxTrfPtr = trfPtr;
  auxPolPtr = polPtr;
  auxLandPtr = landPtr;
  auxCrimePtr = crimePtr;

  for (i = 0; i < HWLDX; i++) {
    PopDensity[i] = (Byte *) auxPopPtr + (i * HWLDY);
    TrfDensity[i] = (Byte *) auxTrfPtr + (i * HWLDY);
    PollutionMem[i] = (Byte *) auxPolPtr + (i * HWLDY);
    LandValueMem[i] = (Byte *) auxLandPtr + (i * HWLDY);
    CrimeMem[i] = (Byte *) auxCrimePtr + (i * HWLDY);

    tem[i] = (Byte *)tem1Base + (i * HWLDY);
    tem2[i] = (Byte *) tem2Base + (i * HWLDY);
  }

  brettPtr = (Ptr) &PopDensity[0][0];

  terrainBase = calloc(QWX * QWY,1);
  qTemBase = calloc(QWX * QWY,1);

  for (i = 0; i < QWX; i++) {
    TerrainMem[i] = (Byte *)terrainBase + (i * QWY);
    Qtem[i] = (Byte *)qTemBase + (i * QWY);
  }

  ResHis = (short *)calloc(HISTLEN,1);
  ComHis = (short *)calloc(HISTLEN,1);
  IndHis = (short *)calloc(HISTLEN,1);
  MoneyHis = (short *)calloc(HISTLEN,1);
  PollutionHis = (short *)calloc(HISTLEN,1);
  CrimeHis = (short *)calloc(HISTLEN,1);
  MiscHis = (short *)calloc(MISCHISTLEN,1);
  PowerMap = (short *)calloc(POWERMAPLEN,1);		/* power alloc */
}
