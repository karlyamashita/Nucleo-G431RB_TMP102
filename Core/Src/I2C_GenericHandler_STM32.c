/*
 * I2C_GenericHandler.c
 *
 *  Created on: Apr 24, 2024
 *      Author: karl.yamashita
 */


#include "main.h"

extern I2C_DataStruct tmp102_data_1;


/*
 * Instructions: User should create a variable of I2C_DataStruct.
 * 				User should also create an Rx and Tx buffer and assign to Rx.data and Tx.data
 * 				Example below

uint8_t tmp102_dataRx[3]; // REG and 2 data bytes
uint8_t tmp102_dataTx[3]; // REG and 2 data bytes

I2C_DataStruct tmp102_data_1 =
{
	.instance = &hi2c1, // Assign i2c instance
	.slaveAddr = (uint8_t) (TMP102_SLAVE_ADDRESS_00 << 1), // be sure to left shift Slave address
	.cs.port = CS1_GPIO_Port, // Assign PORT
	.cs.pin = CS1_Pin, // Assign Port pin
	.Rx.data = tmp102_dataRx, // Assign Rx data array
	.Tx.data = tmp102_dataTx, // Assign Tx data array
	.Rx.timeout = 100, // used in HAL polling timeout
	.Tx.timeout = 100 // used in HAL polling timeout
};

 *
 *
 */


/*
 * Description: Receive some data with polling
 *
 */
int I2C_Master_Receive_Generic(I2C_DataStruct *msg)
{
	if(HAL_I2C_Master_Receive(msg->instance, msg->slaveAddr, msg->Rx.data, msg->Rx.dataSize, msg->Rx.timeout) != HAL_OK)
	{
		return I2C_ERROR_READ;
	}

	return 0;
}

/*
 * Description: Transmit some data with polling
 *
 */
int I2C_Master_Transmit_Generic(I2C_DataStruct *msg)
{
	if(HAL_I2C_Master_Transmit(msg->instance, msg->slaveAddr, msg->Tx.data, msg->Tx.dataSize, msg->Tx.timeout) != HAL_OK)
	{
		return I2C_ERROR_WRITE;
	}

	return 0;
}

/*
 * Description: Receive some data with interrupt
 *
 */
int I2C_Master_Receive_Generic_IT(I2C_DataStruct *msg)
{
	if(HAL_I2C_Master_Receive_IT(msg->instance, msg->slaveAddr, msg->Rx.data, msg->Rx.dataSize) != HAL_OK)
	{
		return I2C_ERROR_READ;
	}

	return 0;
}

/*
 * Description: Transmit some data with interrupt
 *
 */
int I2C_Master_Transmit_Generic_IT(I2C_DataStruct *msg)
{
	if(HAL_I2C_Master_Transmit_IT(msg->instance, msg->slaveAddr, msg->Tx.data, msg->Tx.dataSize) != HAL_OK)
	{
		return I2C_ERROR_WRITE;
	}

	return 0;
}

/*
 * Description: Read data from memory/register using polling
 *
 */
int I2C_Mem_Read_Generic(I2C_DataStruct *msg)
{
	if(msg->cs.port != NULL && msg->cs.pin != 0) // use CS
	{
		HAL_GPIO_WritePin(msg->cs.port, msg->cs.pin, GPIO_PIN_RESET); // pull CS pin low

		if(HAL_I2C_Mem_Read(msg->instance, msg->slaveAddr, msg->Rx.registerAddr, msg->Rx.regSize, msg->Rx.data, msg->Rx.dataSize, msg->Rx.timeout) != HAL_OK)
		{
			return I2C_ERROR_READ;
		}

		HAL_GPIO_WritePin(msg->cs.port, msg->cs.pin, GPIO_PIN_SET); // set CS pin high
	}
	else // no CS, so just read
	{
		if(HAL_I2C_Mem_Read(msg->instance, msg->slaveAddr, msg->Rx.registerAddr, msg->Rx.regSize, msg->Rx.data, msg->Rx.dataSize, msg->Rx.timeout) != HAL_OK)
		{
			return I2C_ERROR_READ;
		}
	}

	return 0;
}

/*
 * Description: Write data to memory/register using polling
 *
 */
int I2C_Mem_Write_Generic(I2C_DataStruct *msg)
{
	if(msg->cs.port != NULL && msg->cs.pin != 0) // use CS
	{
		HAL_GPIO_WritePin(msg->cs.port, msg->cs.pin, GPIO_PIN_RESET); // pull CS pin low

		if(HAL_I2C_Mem_Write(msg->instance, msg->slaveAddr, msg->Tx.registerAddr, msg->Tx.regSize, msg->Tx.data, msg->Tx.dataSize, msg->Tx.timeout) != HAL_OK)
		{
			return I2C_ERROR_WRITE;
		}

		HAL_GPIO_WritePin(msg->cs.port, msg->cs.pin, GPIO_PIN_SET); // set CS pin high
	}
	else // no CS, so just write
	{
		if(HAL_I2C_Mem_Write(msg->instance, msg->slaveAddr, msg->Tx.registerAddr, msg->Tx.regSize, msg->Tx.data, msg->Tx.dataSize, msg->Tx.timeout) != HAL_OK)
		{
			return I2C_ERROR_WRITE;
		}
	}

	return 0;
}

/*
 * Description: Read memory/register in interrupt mode. Be sure to assign a callback function.
 */
int I2C_Mem_Read_Generic_IT(I2C_DataStruct *msg)
{
	// enable CS
	if(msg->cs.port != NULL && msg->cs.pin != 0) // use CS
	{
		HAL_GPIO_WritePin(msg->cs.port, msg->cs.pin, GPIO_PIN_RESET); // pull CS pin low
	}

	// Read memory
	if(HAL_I2C_Mem_Read_IT(msg->instance, msg->slaveAddr, msg->Rx.registerAddr, msg->Rx.regSize, msg->Rx.data, msg->Rx.dataSize) != HAL_OK)
	{
		return I2C_ERROR_READ;
	}

	// Disable CS
	if(msg->cs.port != NULL && msg->cs.pin != 0)
	{
		HAL_GPIO_WritePin(msg->cs.port, msg->cs.pin, GPIO_PIN_SET); // set CS pin high
	}

	return 0;
}

/*
 * Description: Write memory/register in interrupt mode
 */
int I2C_Mem_Write_Generic_IT(I2C_DataStruct *msg)
{
	// enable CS
	if(msg->cs.port != NULL && msg->cs.pin != 0) // use CS
	{
		HAL_GPIO_WritePin(msg->cs.port, msg->cs.pin, GPIO_PIN_RESET); // pull CS pin low
	}

	if(HAL_I2C_Mem_Write_IT(msg->instance, msg->slaveAddr, msg->Tx.registerAddr, msg->Tx.regSize, msg->Tx.data, msg->Tx.dataSize) != HAL_OK)
	{
		return I2C_ERROR_READ;
	}

	// Disable CS
	if(msg->cs.port != NULL && msg->cs.pin != 0)
	{
		HAL_GPIO_WritePin(msg->cs.port, msg->cs.pin, GPIO_PIN_SET); // set CS pin high
	}

	return 0;
}

/*
 * Description: Callback after memory/register has been Read.
 */
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	if(hi2c == tmp102_data_1.instance)
	{
		if(tmp102_data_1.Rx.callback != NULL)
		{
			tmp102_data_1.Rx.callback();
		}
	}
}

/*
 * Description: Callback after memory/register has been written.
 */
void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	if(hi2c == tmp102_data_1.instance)
	{
		if(tmp102_data_1.Tx.callback != NULL)
		{
			tmp102_data_1.Tx.callback();
		}
	}
}

/*
 * Description: Callback after Read.
 */
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	if(hi2c == tmp102_data_1.instance)
	{
		if(tmp102_data_1.Rx.callback != NULL)
		{
			tmp102_data_1.Rx.callback();
		}
	}
}

/*
 * Description: Callback after Write.
 */
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	if(hi2c == tmp102_data_1.instance)
	{
		if(tmp102_data_1.Tx.callback != NULL)
		{
			tmp102_data_1.Tx.callback();
		}
	}
}


