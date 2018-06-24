/*
 * s_power.h
 *
 *  Created on: Jun 3, 2018
 *      Author: Mike
 */

#ifndef SRC_SIM_HEADERS_S_POWER_H_
#define SRC_SIM_HEADERS_S_POWER_H_

extern int PowerStackNum;

void PushPowerStack(void);
void DoPowerScan(void);
int MoveMapSim (short MDir);



#endif /* SRC_SIM_HEADERS_S_POWER_H_ */
