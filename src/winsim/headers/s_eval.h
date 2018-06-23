/*
 * s_eval.h
 *
 *  Created on: Jun 3, 2018
 *      Author: Mike
 */

#ifndef SRC_SIM_HEADERS_S_EVAL_H_
#define SRC_SIM_HEADERS_S_EVAL_H_
#include <stdint.h>
extern int32_t CityPop, deltaCityPop;
extern short CityScore, deltaCityScore, AverageCityScore;
extern short TrafficAverage;
extern short CityClass;			/*  0..5  */
extern short CityYes, CityNo;
extern short ProblemVotes[];		/* these are the votes for each  */
extern short ProblemOrder[4];			/* sorted index to above  */
extern int32_t CityAssValue;
short GetUnemployment(void);
void CityEvaluation(void);
void EvalInit(void);
short GetFire(void);
short AverageTrf(void);




#endif /* SRC_SIM_HEADERS_S_EVAL_H_ */
