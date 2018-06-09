/*
 * w_map.h
 *
 *  Created on: Jun 3, 2018
 *      Author: Mike
 */

#ifndef SRC_SIM_HEADERS_W_MAP_H_
#define SRC_SIM_HEADERS_W_MAP_H_

typedef struct SimView SimView;
int DoUpdateMap(SimView *view);
void map_command_init(void);
void DoNewMap(SimView *view);




#endif /* SRC_SIM_HEADERS_W_MAP_H_ */
