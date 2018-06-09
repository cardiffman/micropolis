/*
 * w_date.h
 *
 *  Created on: Jun 4, 2018
 *      Author: Mike
 */

#ifndef SRC_SIM_HEADERS_W_DATE_H_
#define SRC_SIM_HEADERS_W_DATE_H_

void date_command_init(void);
typedef struct SimDate SimDate;
void DoUpdateDate(SimDate *date);
void DoNewDate(SimDate *date);
void DoResizeDate(SimDate *date, int w, int h);




#endif /* SRC_SIM_HEADERS_W_DATE_H_ */
