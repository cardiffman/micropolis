/*
 * w_budget.h
 *
 *  Created on: Jun 3, 2018
 *      Author: Mike
 */

#ifndef SRC_SIM_HEADERS_W_BUDGET_H_
#define SRC_SIM_HEADERS_W_BUDGET_H_

#include <stdint.h>

extern float roadPercent;
extern float policePercent;
extern float firePercent;

extern int32_t roadMaxValue;
extern int32_t policeMaxValue;
extern int32_t fireMaxValue;

void InitFundingLevel(void);
void UpdateBudgetWindow(void);
void DoBudget(void);
void drawCurrPercents(void);
void drawBudgetWindow(void);
void DoBudgetFromMenu(void);
void UpdateBudget(void);




#endif /* SRC_SIM_HEADERS_W_BUDGET_H_ */
