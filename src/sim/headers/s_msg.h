/*
 * s_msg.h
 *
 *  Created on: Jun 3, 2018
 *      Author: Mike
 */

#ifndef SRC_SIM_HEADERS_S_MSG_H_
#define SRC_SIM_HEADERS_S_MSG_H_

extern short HaveLastMessage;
extern short autoGo;

void ClearMes(void);
void SendMesAt(short Mnum, short x, short y);
int SendMes(int Mnum);
void SendMessages(void);
void doMessage(void);



#endif /* SRC_SIM_HEADERS_S_MSG_H_ */
