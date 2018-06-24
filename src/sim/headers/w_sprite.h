/*
 * w_sprite.h
 *
 *  Created on: Jun 3, 2018
 *      Author: Mike
 */

#ifndef SRC_SIM_W_SPRITE_H_
#define SRC_SIM_W_SPRITE_H_

extern short CrashX, CrashY;

void GenerateCopter(int x, int y);
void GeneratePlane(int x, int y);
void GenerateTrain(int x, int y);
void GenerateShip(void);
typedef struct SimSprite SimSprite;
SimSprite *GetSprite(int type);
void MoveObjects(void);
void MakeMonster(void);
void MakeExplosion(int x, int y);
void DestroyAllSprites(void);
void MakeExplosionAt(int x, int y);
void MakeTornado(void);
void sprite_command_init(void);
typedef struct SimView SimView;
void DrawObjects(SimView *view);



#endif /* SRC_SIM_W_SPRITE_H_ */
