/*
 * s_zone.h
 *
 *  Created on: Jun 3, 2018
 *      Author: Mike
 */

#ifndef SRC_SIM_HEADERS_S_ZONE_H_
#define SRC_SIM_HEADERS_S_ZONE_H_

int DoFreePop (void);
int RZPop(int Ch9);
int CZPop(int);
int IZPop(int Ch9);
int SetZPower(void);		/* set bit in MapWord depending on powermap  */
void DoZone(void);




#endif /* SRC_SIM_HEADERS_S_ZONE_H_ */
