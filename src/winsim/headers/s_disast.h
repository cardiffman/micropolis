/*
 * s_disast.h
 *
 *  Created on: Jun 3, 2018
 *      Author: Mike
 */

#ifndef SRC_SIM_HEADERS_S_DISAST_H_
#define SRC_SIM_HEADERS_S_DISAST_H_

extern short ShakeNow;
extern short FloodX, FloodY;

void DoDisasters(void);
void DoFlood(void);
void MakeEarthquake(void);
void MakeFire(void);
void MakeFlood(void);
void FireBomb(void);
void MakeMeltdown(void);



#endif /* SRC_SIM_HEADERS_S_DISAST_H_ */
