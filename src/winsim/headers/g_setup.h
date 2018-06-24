/*
 * g_setup.h
 *
 *  Created on: Jun 4, 2018
 *      Author: Mike
 */

#ifndef SRC_SIM_HEADERS_G_SETUP_H_
#define SRC_SIM_HEADERS_G_SETUP_H_

typedef struct SimView SimView;
void GetViewTiles(SimView *view);
typedef struct XDisplay XDisplay;
void GetPixmaps(XDisplay *xd);




#endif /* SRC_SIM_HEADERS_G_SETUP_H_ */
