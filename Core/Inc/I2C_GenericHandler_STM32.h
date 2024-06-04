/*
 * I2C_GenericHandler.h
 *
 *  Created on: Apr 24, 2024
 *      Author: karl.yamashita
 */

#ifndef INC_I2C_GENERICHANDLER_STM32_H_
#define INC_I2C_GENERICHANDLER_STM32_H_



typedef void (*I2C_Callback)(void);// no data is passed

typedef struct
{
	uint32_t pin;
	GPIO_TypeDef *port; // port base
}GPIO_CS_t; // chip select

typedef struct
{
	I2C_HandleTypeDef *instance;
	uint8_t slaveAddr;
	GPIO_CS_t cs;
	struct
	{
		uint8_t *data;
		uint32_t dataSize;
		uint16_t registerAddr;
		uint32_t regSize;
		uint32_t timeout;
		I2C_Callback callback;
	}Rx;
	struct
	{
		uint8_t *data;
		uint32_t dataSize;
		uint16_t registerAddr;
		uint32_t regSize;
		uint32_t timeout;
		I2C_Callback callback;
	}Tx;
}I2C_DataStruct;


int I2C_Master_Receive_Generic(I2C_DataStruct *msg);
int I2C_Master_Transmit_Generic(I2C_DataStruct *msg);
int I2C_Master_Receive_Generic_IT(I2C_DataStruct *msg);
int I2C_Master_Transmit_Generic_IT(I2C_DataStruct *msg);

int I2C_Mem_Read_Generic(I2C_DataStruct *msg);
int I2C_Mem_Write_Generic(I2C_DataStruct *msg);
int I2C_Mem_Read_Generic_IT(I2C_DataStruct *msg);
int I2C_Mem_Write_Generic_IT(I2C_DataStruct *msg);

#endif /* INC_I2C_GENERICHANDLER_STM32_H_ */
