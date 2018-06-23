/*
 * myxbm.h
 *
 *  Created on: Jun 18, 2018
 *      Author: Mike
 */

#ifndef SRC_MYXBM_MYXBM_H_
#define SRC_MYXBM_MYXBM_H_

#include <stdint.h>

// New XPM code
typedef struct {
	char type;
	char* color;
	char* argument;
	char* mask;
} XpmFileColor;
typedef struct {
	int rows;
	int columns;
	int numColors;
	int charsDepth;
	XpmFileColor* colors;
	uint8_t* body;
} XpmFileData;
int ReadXpmFile(const char* name, XpmFileData* contents);
int ReadXpmFileW(LPWSTR name, XpmFileData* contents);



#endif /* SRC_MYXBM_MYXBM_H_ */
