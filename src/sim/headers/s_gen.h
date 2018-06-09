/*
 * s_gen.h
 *
 *  Created on: Jun 3, 2018
 *      Author: Mike
 */

#ifndef SRC_SIM_HEADERS_S_GEN_H_
#define SRC_SIM_HEADERS_S_GEN_H_

extern int TreeLevel;		/* level for tree creation */
extern int LakeLevel;		/* level for lake creation */
extern int CurveLevel;		/* level for river curviness */
extern int CreateIsland;		/* -1 => 10%, 0 => never, 1 => always */

void ClearMap(void);
void ClearUnnatural(void);
void SmoothRiver(void);
void SmoothWater(void);
void SmoothTrees(void);
void GenerateNewCity(void);
void GenerateSomeCity(int r);




#endif /* SRC_SIM_HEADERS_S_GEN_H_ */
