/* sim.h:  Main include file
 *
 * Micropolis, Unix Version.  This game was released for the Unix platform
 * in or about 1990 and has been modified for inclusion in the One Laptop
 * Per Child program.  Copyright (C) 1989 - 2007 Electronic Arts Inc.  If
 * you need assistance with this program, you may contact:
 *   http://wiki.laptop.org/go/Micropolis  or email  micropolis@laptop.org.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.  You should have received a
 * copy of the GNU General Public License along with this program.  If
 * not, see <http://www.gnu.org/licenses/>.
 * 
 *             ADDITIONAL TERMS per GNU GPL Section 7
 * 
 * No trademark or publicity rights are granted.  This license does NOT
 * give you any right, title or interest in the trademark SimCity or any
 * other Electronic Arts trademark.  You may not distribute any
 * modification of this program using the trademark SimCity or claim any
 * affliation or association with Electronic Arts Inc. or its employees.
 * 
 * Any propagation or conveyance of this program must include this
 * copyright notice and these terms.
 * 
 * If you convey this program (or any modifications of it) and assume
 * contractual liability for the program to recipients of it, you agree
 * to indemnify Electronic Arts for any liability that those contractual
 * assumptions impose on Electronic Arts.
 * 
 * You may not misrepresent the origins of this program; modified
 * versions of the program must be marked as such and not identified as
 * the original program.
 * 
 * This disclaimer supplements the one included in the General Public
 * License.  TO THE FULLEST EXTENT PERMISSIBLE UNDER APPLICABLE LAW, THIS
 * PROGRAM IS PROVIDED TO YOU "AS IS," WITH ALL FAULTS, WITHOUT WARRANTY
 * OF ANY KIND, AND YOUR USE IS AT YOUR SOLE RISK.  THE ENTIRE RISK OF
 * SATISFACTORY QUALITY AND PERFORMANCE RESIDES WITH YOU.  ELECTRONIC ARTS
 * DISCLAIMS ANY AND ALL EXPRESS, IMPLIED OR STATUTORY WARRANTIES,
 * INCLUDING IMPLIED WARRANTIES OF MERCHANTABILITY, SATISFACTORY QUALITY,
 * FITNESS FOR A PARTICULAR PURPOSE, NONINFRINGEMENT OF THIRD PARTY
 * RIGHTS, AND WARRANTIES (IF ANY) ARISING FROM A COURSE OF DEALING,
 * USAGE, OR TRADE PRACTICE.  ELECTRONIC ARTS DOES NOT WARRANT AGAINST
 * INTERFERENCE WITH YOUR ENJOYMENT OF THE PROGRAM; THAT THE PROGRAM WILL
 * MEET YOUR REQUIREMENTS; THAT OPERATION OF THE PROGRAM WILL BE
 * UNINTERRUPTED OR ERROR-FREE, OR THAT THE PROGRAM WILL BE COMPATIBLE
 * WITH THIRD PARTY SOFTWARE OR THAT ANY ERRORS IN THE PROGRAM WILL BE
 * CORRECTED.  NO ORAL OR WRITTEN ADVICE PROVIDED BY ELECTRONIC ARTS OR
 * ANY AUTHORIZED REPRESENTATIVE SHALL CREATE A WARRANTY.  SOME
 * JURISDICTIONS DO NOT ALLOW THE EXCLUSION OF OR LIMITATIONS ON IMPLIED
 * WARRANTIES OR THE LIMITATIONS ON THE APPLICABLE STATUTORY RIGHTS OF A
 * CONSUMER, SO SOME OR ALL OF THE ABOVE EXCLUSIONS AND LIMITATIONS MAY
 * NOT APPLY TO YOU.
 */
#ifndef INCLUDED_SIM_SIM_H
#define INCLUDED_SIM_SIM_H

/* workaround sun c compiler junk */

#ifdef sun

#define _TTYCHARS_
#define CTRL(c) (#c[0]&037)

#ifndef SOLARIS2
#define __sys_ioccom_h
#define _IOCPARM_MASK   0xff            /* parameters must be < 256 bytes */
#define _IOC_VOID       0x20000000      /* no parameters */
#define _IOC_OUT        0x40000000      /* copy out parameters */
#define _IOC_IN         0x80000000      /* copy in parameters */
#define _IOC_INOUT      (_IOC_IN|_IOC_OUT)
#define _IO(x,y)        (_IOC_VOID|(#x[0]<<8)|y)
#define _IOR(x,y,t)     (_IOC_OUT|((sizeof(t)&_IOCPARM_MASK)<<16)|(#x[0]<<8)|y)
#define _IORN(x,y,t)    (_IOC_OUT|(((t)&_IOCPARM_MASK)<<16)|(#x[0]<<8)|y)
#define _IOW(x,y,t)     (_IOC_IN|((sizeof(t)&_IOCPARM_MASK)<<16)|(#x[0]<<8)|y)
#define _IOWN(x,y,t)    (_IOC_IN|(((t)&_IOCPARM_MASK)<<16)|(#x[0]<<8)|y)
#define _IOWR(x,y,t)    (_IOC_INOUT|((sizeof(t)&_IOCPARM_MASK)<<16)|(#x[0]<<8)|y)
#define _IOWRN(x,y,t)   (_IOC_INOUT|(((t)&_IOCPARM_MASK)<<16)|(#x[0]<<8)|y)
#endif

#endif



/* Constants */

#define TRUE		 1
#define FALSE		 0

#ifdef MEGA
#define SimWidth	(120 * MEGA)
#define SimHeight	(100 * MEGA)
#else
#define SimWidth	120
#define SimHeight	100
#endif

#define WORLD_X		SimWidth
#define WORLD_Y		SimHeight
#define HWLDX		(SimWidth >>1)
#define HWLDY		(SimHeight >>1)
#define QWX		(SimWidth >>2)
#define QWY		(SimHeight >>2)
#define SmX		(SimWidth >>3)
#define SmY		((SimHeight + 7) >>3)

#define EDITOR_W (WORLD_X * 16)
#define EDITOR_H (WORLD_Y * 16)
#define MAP_W (WORLD_X * 3)
#define MAP_H (WORLD_Y * 3)

#define NIL		0
#define HORIZ		1
#define VERT		0

#define PROBNUM 10

#define HISTLEN		480
#define MISCHISTLEN	240

#define POWERMAPROW		((WORLD_X + 15) / 16)

#ifdef MEGA
#define POWERMAPLEN		((QUAD)(2 * POWERMAPROW * WORLD_Y))
#define POWERWORD(x, y)		((((QUAD)(x)) >>4) + (((QUAD)(y)) * POWERMAPROW))
#else
#define POWERMAPLEN		1700 /* ??? PWRMAPSIZE */
#define POWERWORD(x, y)		(((x) >>4) + ((y) <<3))
#endif

#define SETPOWERBIT(x, y)	PowerMap[POWERWORD((x), (y))] |= 1 << ((x) & 15)
#define PWRMAPSIZE		(POWERMAPROW * WORLD_Y)
#define PWRSTKSIZE		((WORLD_X * WORLD_Y) / 4)

#define ALMAP 0 /* all */
#define REMAP 1 /* residential */
#define COMAP 2 /* commercial */
#define INMAP 3 /* industrial */

#define PRMAP 4 /* power */
#define RDMAP 5 /* road */

#define PDMAP 6 /* population density */
#define RGMAP 7 /* rate of growth */

#define TDMAP 8 /* traffic density */
#define PLMAP 9 /* pollution */
#define CRMAP 10 /* crime */
#define LVMAP 11 /* land value */

#define FIMAP 12 /* fire radius */
#define POMAP 13 /* police radius */
#define DYMAP 14 /* dynamic */

#define NMAPS 15

/* These adjust frequency in Simulate() */

#define VALVERATE 2
#define CENSUSRATE 4
#define TAXFREQ 48

/* These are names of the 16 colors */
#define COLOR_WHITE		0
#define COLOR_YELLOW		1
#define COLOR_ORANGE		2
#define COLOR_RED		3
#define COLOR_DARKRED		4
#define COLOR_DARKBLUE		5
#define COLOR_LIGHTBLUE		6
#define COLOR_BROWN		7
#define COLOR_LIGHTGREEN	8
#define COLOR_DARKGREEN		9
#define COLOR_OLIVE		10
#define COLOR_LIGHTBROWN	11
#define COLOR_LIGHTGRAY		12
#define COLOR_MEDIUMGRAY	13
#define COLOR_DARKGRAY		14
#define COLOR_BLACK		15

/* Status Bits */

#define PWRBIT		32768	/*20	bit 15	*/
#define CONDBIT		16384	/*10	bit 14	*/
#define BURNBIT		8192	/*8	bit 13	*/
#define BULLBIT		4096	/*4	bit 12	*/
#define ANIMBIT		2048	/*2	bit 11	*/
#define ZONEBIT		1024	/*1	bit 10	*/
#define ALLBITS		64512	/*  mask for upper 6 bits	*/
#define LOMASK		1023	/*	mask for low 10 bits	*/

#define BLBNBIT		(BULLBIT+BURNBIT)
#define BLBNCNBIT	(BULLBIT+BURNBIT+CONDBIT)
#define BNCNBIT		(BURNBIT+CONDBIT)

/* Object & Sound Numbers */

#define TRA		1
#define COP		2
#define AIR		3
#define SHI		4
#define GOD		5
#define TOR		6
#define EXP		7
#define BUS		8

/* Max # of Objects */

#define OBJN		9

/* Graph Histories */
#define RES_HIST	0
#define COM_HIST	1
#define IND_HIST	2
#define MONEY_HIST	3
#define CRIME_HIST	4
#define POLLUTION_HIST	5
#define HISTORIES	6
#define ALL_HISTORIES	((1 <<HISTORIES) - 1)

/* Character Mapping */

enum {
	DIRT	=	0,
	RIVER	=	2,
	REDGE	=	3,
	CHANNEL	=	4,
	FIRSTRIVEDGE=	5,
	LASTRIVEDGE=	20,
	TREEBASE=	21,
	LASTTREE=	36,
	WOODS	=	37,
	UNUSED_TRASH1=	38,
	UNUSED_TRASH2=	39,
	WOODS2	=	40,
	WOODS3	=	41,
	WOODS4	=	42,
	WOODS5	=	43,
	RUBBLE	=	44,
	LASTRUBBLE=	47,
	FLOOD	=	48,
	LASTFLOOD=	51,
	RADTILE	=	52,
	UNUSED_TRASH3=	53,
	UNUSED_TRASH4=	54,
	UNUSED_TRASH5=	55,
	FIRE	=	56,
	FIREBASE=	56,
	LASTFIRE=	63,
	ROADBASE=	64,
	HBRIDGE	=	64,
	VBRIDGE	=	65,
	ROADS	=	66,
	INTERSECTION=	76,
	HROADPOWER=	77,
	VROADPOWER=	78,
	BRWH	=	79,
	LTRFBASE=	80,
	BRWV	=	95,
	BRWXXX1	=	111,
	BRWXXX2	=	127,
	BRWXXX3	=	143,
	HTRFBASE=	144,
	BRWXXX4	=	159,
	BRWXXX5	=	175,
	BRWXXX6	=	191,
	LASTROAD=	206,
	BRWXXX7	=	207,
	POWERBASE=	208,
	HPOWER	=	208,
	VPOWER	=	209,
	LHPOWER	=	210,
	LVPOWER	=	211,
	RAILHPOWERV=	221,
	RAILVPOWERH=	222,
	LASTPOWER=	222,
	UNUSED_TRASH6=	223,
	RAILBASE=	224,
	HRAIL	=	224,
	VRAIL	=	225,
	LHRAIL	=	226,
	LVRAIL	=	227,
	HRAILROAD=	237,
	VRAILROAD=	238,
	LASTRAIL=	238,
	ROADVPOWERH=	239, /* bogus? */
	RESBASE	=	240, // F0
	FREEZ	=	244, // F4
	HOUSE	=	249, // F9
	LHTHR	=	249, // F9
	HHTHR	=	260, // 104
	RZB		=265,
	HOSPITAL=	409,
	CHURCH	=	418,
	COMBASE	=	423,
	COMCLR	=	427,
	CZB		=436,
	INDBASE	=	612,
	INDCLR	=	616,
	LASTIND	=	620,
	IND1	=	621,
	IZB		=625,
	IND2	=	641,
	IND3	=	644,
	IND4	=	649,
	IND5	=	650,
	IND6	=	676,
	IND7	=	677,
	IND8	=	686,
	IND9	=	689,
	PORTBASE=	693,
	PORT	=	698,
	LASTPORT=	708,
	AIRPORTBASE=	709,
	RADAR	=	711,
	AIRPORT	=	716,
	COALBASE=	745,
	POWERPLANT=	750,
	LASTPOWERPLANT=	760,
	FIRESTBASE=	761,
	FIRESTATION=	765,
	POLICESTBASE=	770,
	POLICESTATION=	774,
	STADIUMBASE=	779,
	STADIUM	=	784,
	FULLSTADIUM=	800,
	NUCLEARBASE=	811,
	NUCLEAR	=	816,
	LASTZONE=	826,
	LIGHTNINGBOLT=	827,
	HBRDG0	=	828,
	HBRDG1	=	829,
	HBRDG2	=	830,
	HBRDG3	=	831,
	RADAR0	=	832,
	RADAR1	=	833,
	RADAR2	=	834,
	RADAR3	=	835,
	RADAR4	=	836,
	RADAR5	=	837,
	RADAR6	=	838,
	RADAR7	=	839,
	FOUNTAIN=	840,
	INDBASE2=	844,
	TELEBASE=	844,
	TELELAST=	851,
	SMOKEBASE=	852,
	TINYEXP	=	860,
	SOMETINYEXP=	864,
	LASTTINYEXP=	867,
	COALSMOKE1=	916,
	COALSMOKE2=	920,
	COALSMOKE3=	924,
	COALSMOKE4=	928,
	FOOTBALLGAME1=	932,
	FOOTBALLGAME2=	940,
	VBRDG0	=	948,
	VBRDG1	=	949,
	VBRDG2	=	950,
	VBRDG3	=	951,

	TILE_COUNT=	960 // 0x3C0 960-768=192
};
/* 
 * These describe the wand values, the object dragged around on the screen.
 */

#define residentialState 0
#define commercialState 1
#define industrialState 2
#define fireState 3
#define queryState 4
#define policeState 5
#define wireState 6
#define dozeState 7
#define rrState 8
#define roadState 9
#define chalkState 10
#define eraserState 11
#define stadiumState 12
#define parkState 13
#define seaportState  14
#define powerState 15
#define nuclearState 16
#define airportState 17
#define networkState 18


#define firstState residentialState
#define lastState networkState



#define STATE_CMD 0
#define STATE_TILES 1
#define STATE_OVERLAYS 2
#define STATE_GRAPHS 3


extern char *MicropolisVersion;
extern char *CityFileName;
extern char *StartupName;
extern int TilesAnimated;
extern int DoAnimation;
extern int DoMessages;
extern int DoNotices;
extern int Startup;
extern struct timeval start_time, now_time, beat_time, last_now_time;
typedef struct Sim Sim;
extern Sim *sim;
void sim_update_editors(void);

extern int MultiPlayerMode;
extern int SugarMode;
extern int sim_delay;
extern int sim_skips;
extern int sim_skip;
extern int sim_paused;
extern int sim_paused_speed;
extern int sim_tty;
extern int heat_steps;
extern int heat_flow;
extern int heat_rule;
extern char *Displays;
extern char *FirstDisplay;
void sim_exit(int val);
void sim_init(void);
void sim_really_exit(int val);
void sim_update();
void sim_loop(int doSim);


#endif

