/*
 * s_scan.h
 *
 *  Created on: Jun 3, 2018
 *      Author: Mike
 */

#ifndef SRC_SIM_HEADERS_S_SCAN_H_
#define SRC_SIM_HEADERS_S_SCAN_H_

#include "sim.h"
#include <stdint.h>
extern short NewMapFlags[NMAPS];
extern short NewMap;
extern short CCx, CCy, CCx2, CCy2;
extern short CrimeMaxX, CrimeMaxY;
extern short PolMaxX, PolMaxY;
extern int32_t DonDither;
void PTLScan(void);   	/* Does pollution, terrain, land value   */
void CrimeScan(void);
void PopDenScan(void);		/*  sets: PopDensity, , , ComRate  */
void FireAnalysis(void);		/* Make firerate map from firestation map  */




#endif /* SRC_SIM_HEADERS_S_SCAN_H_ */
