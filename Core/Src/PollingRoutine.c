/*
 * PollingRoutine.c
 *
 *  Created on: Oct 24, 2023
 *      Author: karl.yamashita
 *
 *
 *      Template for projects.
 *
 */


#include "main.h"

extern UART_HandleTypeDef hlpuart1;

extern TimerCallbackStruct timerCallback;

extern I2C_DataStruct tmp102_data_1;

UART_DMA_QueueStruct uart1_msg =
{
	.huart = &hlpuart1,
	.rx.queueSize = UART_DMA_QUEUE_SIZE,
	.tx.queueSize = UART_DMA_QUEUE_SIZE
};


void PollingInit(void)
{
	TimerCallbackRegisterOnly(&timerCallback, BlinkLED);
	//TimerCallbackTimerStart(&timerCallback, BlinkLED, 500, TIMER_REPEAT);

	TMP102_Init(&tmp102_data_1);

	UART_DMA_EnableRxInterrupt(&uart1_msg);

	UART_DMA_NotifyUser(&uart1_msg, "STM32 Ready", strlen("STM32 Ready"), true);
}

void PollingRoutine(void)
{
	TimerCallbackCheck(&timerCallback);

	UART_Parse(&uart1_msg);
}

void UART_Parse(UART_DMA_QueueStruct *msg)
{
	int status = NO_ERROR;
	char *ptr;
	char retStr[64] = "ok";
	char msgCpy = {0};

	if(UART_DMA_MsgRdy(msg))
	{
		ptr = (char*)msg->rx.msgToParse->data;
		memcpy(&msgCpy, ptr, strlen(ptr) - 2); // less CR/LF
		RemoveSpaces(ptr);
		ToLower(ptr);

		if(strncmp(ptr, "get", strlen("get")) == 0)
		{
			ptr += strlen("get");
			status = GetCommand(ptr, retStr);
		}
		else if(strncmp(ptr, "set", strlen("set")) == 0)
		{
			ptr += strlen("set");
			status = SetCommand(ptr);
		}
		else
		{
			sprintf(retStr, "Command Unknown");
		}

		if(status == NO_ACK)
		{
			return;
		}
		else if(status != NO_ERROR)
		{
			PrintError(&msgCpy, status);
		}
		else
		{
			PrintReply(&msgCpy, retStr);
		}
	}
}

int GetCommand(char *msg, char *retStr)
{
	int status = NO_ERROR;

	if(strncmp(msg, "tmp102:", strlen("tmp102:")) == 0)
	{
		msg += strlen("tmp102:");
		status = TMP102_GetRegisterHandle(msg, retStr);
	}
	else
	{
		return COMMAND_UNKNOWN;
	}

	return status;
}

int SetCommand(char *msg)
{
	int status = NO_ERROR;

	if(strncmp(msg, "tmp102:", strlen("tmp102:")) == 0)
	{
		msg += strlen("tmp102:");
		status = TMP102_SetRegisterHandle(msg);
	}
	else
	{
		return COMMAND_UNKNOWN;
	}

	return status;
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if(huart == uart1_msg.huart)
	{
		RingBuff_Ptr_Input(&uart1_msg.rx.ptr, uart1_msg.rx.queueSize);
		UART_DMA_EnableRxInterrupt(&uart1_msg);
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == uart1_msg.huart)
	{
		uart1_msg.tx.txPending = false;
		UART_DMA_SendMessage(&uart1_msg);
	}
}

void BlinkLED(void)
{
	HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
}

void PrintError(char *msgCpy, uint32_t error)
{
	char str[64] = {0};

	GetErrorString(error, str);

	strcat(msgCpy, "=");
	strcat(msgCpy, str);

	UART_DMA_NotifyUser(&uart1_msg, msgCpy, strlen(msgCpy), true);
}

void PrintReply(char *msgCpy, char *msg2)
{
	strcat(msgCpy, "=");
	strcat(msgCpy, msg2);
	UART_DMA_NotifyUser(&uart1_msg, msgCpy, strlen(msgCpy), true);
}



