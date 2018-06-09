/*
 * g_bigmap.h
 *
 *  Created on: Jun 7, 2018
 *      Author: Mike
 */

#ifndef SRC_SIM_HEADERS_G_BIGMAP_H_
#define SRC_SIM_HEADERS_G_BIGMAP_H_

typedef struct SimView SimView;
void WireDrawBeegMapRect(SimView *view, short x, short y, short w, short h);
void MemDrawBeegMapRect(SimView *view, int x, int y, int w, int h);



#endif /* SRC_SIM_HEADERS_G_BIGMAP_H_ */
