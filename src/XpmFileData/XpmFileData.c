/*
 * myxbm.c
 *
 *  Created on: Jun 18, 2018
 *      Author: Mike
 */
#include <windows.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "XpmFileData.h"

int ReadXpmFileF(FILE* f, XpmFileData* contents) {
	memset(contents, 0, sizeof(XpmFileData));
	char* line=NULL;
	size_t size=0;
	getline(&line,&size,f);
	if (strncmp(line, "/* XPM */",9)!=0) {
		free(line);
		fclose(f);
		puts("XPM magic missing");
		return -EINVAL;
	}
	puts("XPM magic read");
	while (1) {
		getline(&line,&size,f);
		if (line[0]=='"') {
			char* t = strtok(line+1," ");
			contents->columns = atoi(t);
			t = strtok(NULL," ");
			contents->rows = atoi(t);
			t = strtok(NULL," ");
			contents->numColors = atoi(t);
			t = strtok(NULL," ");
			contents->charsDepth = atoi(t);
			break;
		}
	}
	puts("Header read");
	contents->colors = calloc(contents->numColors, sizeof(XpmFileColor));
	contents->body = malloc(contents->rows*contents->columns*contents->charsDepth);
	for (int color=0; color<contents->numColors; ++color) {
		do {
			getline(&line,&size,f);
		} while(line[0]!='"');
		// Color is
		// "c+ t v+"
		// Where c+ is the character sequence that will represent
		// a pixel's color in the body.
		// where t is the type of color notation used.
		// where v+ is the character sequence representing the RGB.
		// v+ can be a name or an HTML/CSS hex notation.
		// In 256-color images c+ might be two characters. It will
		// be consistently two characters and one or both could be
		// a space.
		char* t = line+1;//strtok(line+1, " ");
		contents->colors[color].color = malloc(contents->charsDepth+1);
		memcpy(contents->colors[color].color,t,contents->charsDepth);
		contents->colors[color].color[contents->charsDepth]=0;
		t = t+contents->charsDepth+1;
		contents->colors[color].type = *t++;
		t++; // skip the space between the type field and the value field
		char* q = strchr(t, '"');
		*q = 0;
		contents->colors[color].argument = strdup(t);
		printf("color %d: %c %s %s\n",color, contents->colors[color].type, contents->colors[color].color, contents->colors[color].argument);
	}
	puts("XPM magic missing");
	getline(&line,&size,f);
	while (line[0]!='"')
		getline(&line,&size,f);
	for (int row=0; row<contents->rows; row++) {
		if (row==0) {
			printf("A row has %d characters\n", strlen(line));
			printf("%d between quotes\n", strchr(line+1, '"')-line-1);
		}
		memcpy(contents->body+row*contents->columns*contents->charsDepth, line+1, contents->columns*contents->charsDepth);
		getline(&line,&size,f);
		if (line[0]!='"' && line[0]!='\n')
		{
			printf("Bad line line[0]=='%c'\n", line[0]);
			break;
		}
		if (line[0]=='\n')
			getline(&line,&size,f);
	}
	fclose(f);
	printf("image.rows %d\n", contents->rows);
	printf("image.columns %d\n", contents->columns);
	printf("image.charsDepth %d\n", contents->charsDepth);
	printf("image.numColors %d\n", contents->numColors);
	return 0;
}

int ReadXpmFileW(LPWSTR name, XpmFileData* contents) {
	UINT cc = WideCharToMultiByte(CP_UTF8, 0, name, -1, NULL, 0, NULL, NULL);
	char* mbname = malloc(cc);
	WideCharToMultiByte(CP_UTF8, 0, name, -1, mbname, cc, NULL, NULL);
	FILE* f = fopen(mbname, "rb");
	if (f == NULL) {
		int rc = -errno;
		perror(mbname);
		free(mbname);
		return rc;
	}
	free(mbname);
	return ReadXpmFileF(f, contents);
}
int ReadXpmFile(const char* name, XpmFileData* contents) {
	FILE* f = fopen(name, "r");
	if (f == NULL) {
		int rc = -errno;
		perror(name);
		return rc;
	}
	return ReadXpmFileF(f, contents);
}
