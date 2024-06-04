/*
 * PollingRoutine.h
 *
 *  Created on: Oct 24, 2023
 *      Author: karl.yamashita
 *
 *
 *      Template
 */

#ifndef INC_POLLINGROUTINE_H_
#define INC_POLLINGROUTINE_H_


/*

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <ctype.h>
#define Nop() asm(" NOP ")

*/


void PollingInit(void);
void PollingRoutine(void);

void UART_Parse(UART_DMA_QueueStruct *msg);

int GetCommand(char *msg, char *retStr);
int SetCommand(char *msg);

void BlinkLED(void);

void PrintError(char *msgCpy, uint32_t error);
void PrintReply(char *msgCpy, char *msg2);

#endif /* INC_POLLINGROUTINE_H_ */
