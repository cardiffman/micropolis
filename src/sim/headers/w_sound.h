/*
 * w_sound.h
 *
 *  Created on: Jun 3, 2018
 *      Author: Mike
 */

#ifndef SRC_SIM_HEADERS_W_SOUND_H_
#define SRC_SIM_HEADERS_W_SOUND_H_

void InitializeSound(void);
void MakeSound(char *channel, char *id);
typedef struct SimView SimView;
void MakeSoundOn(SimView *view, char *channel, char *id);
void SoundOff(void);
void StartBulldozer(void);
void StopBulldozer(void);



#endif /* SRC_SIM_HEADERS_W_SOUND_H_ */
