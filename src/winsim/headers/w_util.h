/*
 * w_util.h
 *
 *  Created on: Jun 3, 2018
 *      Author: Mike
 */

#ifndef SRC_SIM_HEADERS_W_UTIL_H_
#define SRC_SIM_HEADERS_W_UTIL_H_

void SetGameLevelFunds(short level);
void setSpeed(short speed);
void setSkips(int skips);
void DoNewGame(void);
void SetGameLevel(short level);
void setAnyCityName(char *name);
void setCityName(char *name);
void UpdateGameLevel(void);
int CurrentYear(void);
void makeDollarDecimalStr(char *numStr, char *dollarStr);
void Pause(void);
void Resume(void);
void SetYear(int year);
typedef struct SimView SimView;
void DoSetMapState(SimView *view, short state);




#endif /* SRC_SIM_HEADERS_W_UTIL_H_ */
