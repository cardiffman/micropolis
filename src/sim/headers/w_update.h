/*
 * w_update.h
 *
 *  Created on: Jun 3, 2018
 *      Author: Mike
 */

#ifndef SRC_SIM_HEADERS_W_UPDATE_H_
#define SRC_SIM_HEADERS_W_UPDATE_H_

#include <stdint.h>

extern short MustUpdateOptions;
extern short MustUpdateFunds;
extern uint8_t LastCityTime;
extern uint8_t LastCityYear;
extern uint8_t LastCityMonth;
extern uint8_t LastFunds;
extern int32_t LastR, LastC, LastI;
extern char *dateStr[12];

void DoUpdateHeads(void);
void UpdateFunds(void);
void doTimeStuff(void);
void UpdateEvaluation(void);
void UpdateGraphs(void);
void UpdateEditors(void);
void UpdateMaps(void);
void UpdateHeads(void);




#endif /* SRC_SIM_HEADERS_W_UPDATE_H_ */
