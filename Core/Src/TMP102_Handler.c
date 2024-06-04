/*
 * TMP102_Handler.c
 *
 *  Created on: Apr 24, 2024
 *      Author: karl.yamashita
 */

#include "main.h"

extern I2C_HandleTypeDef hi2c1;
extern UART_DMA_QueueStruct uart1_msg;

uint8_t tmp102_dataRx[3]; // REG and 2 data bytes
uint8_t tmp102_dataTx[3]; // REG and 2 data bytes

I2C_DataStruct tmp102_data_1 =
{
	.instance = &hi2c1,
	.slaveAddr = (uint8_t) (TMP102_SLAVE_ADDRESS_00 << 1),
	.cs.port = CS1_GPIO_Port,
	.cs.pin = CS1_Pin,
	.Rx.data = tmp102_dataRx,
	.Tx.data = tmp102_dataTx,
	.Rx.timeout = 100, // timeout used in polling.
	.Tx.timeout = 100
};

/*
 * Description: initialize TMP102 sensor
 *
 * Input:
 * Output:
 */
int TMP102_Init(I2C_DataStruct *msg)
{
    int status = NO_ERROR;
    TMP102_Configuration TMP102_Configuration = {0};

    TMP102_Configuration.Status.SD = 0;
    TMP102_Configuration.Status.TM = 1; // interrupt mode
    TMP102_Configuration.Status.POL = 0;
    TMP102_Configuration.Status.F0_1 = 2;
    TMP102_Configuration.Status.R0 = 1;
    TMP102_Configuration.Status.R1 = 1;
    TMP102_Configuration.Status.OS_ALERT = 0;
    // = 0x70

    msg->Tx.regSize = 1;
    msg->Tx.registerAddr = (uint16_t) TMP102_CONFIGURATION_REGISTER;
    msg->Tx.dataSize = 2;
    msg->Tx.data[0] = TMP102_Configuration.Byte.byte0;
    msg->Tx.data[1] = TMP102_Configuration.Byte.byte1;

    status = I2C_Mem_Write_Generic(msg);
    if(status != NO_ERROR)
    {
        return status;
    }

    // 0x0F = 1�C;
    // c to f = (c * 1.8) + 32
    // f to c = (f -32) / 1.8

    return NO_ERROR;
}

/*
 * Description:
 *
 */
int TMP102_GetRegisterHandle(char *msg, char *retStr)
{
	int status = NO_ERROR;
	char *token;
	char *rest = msg;
	char delim[] =",\r";
	bool noAck = false;

	token = strtok_r(rest, delim, &rest);

	if(strncmp(token, "temp", strlen("temp")) == 0)
	{
		tmp102_data_1.Rx.registerAddr = TMP102_TEMPERATURE_REGISTER; // register address to read
		tmp102_data_1.Rx.callback = TMP102_TemperatureCallback; // assign callback
		noAck = true;
	}
	else if(strncmp(token, "config", strlen("config")) == 0)
	{
		tmp102_data_1.Rx.registerAddr = TMP102_CONFIGURATION_REGISTER;
		tmp102_data_1.Rx.callback = TMP102_ConfigCallback;
		noAck = true;
	}
	else if(strncmp(token, "low", strlen("low")) == 0)
	{
		tmp102_data_1.Rx.registerAddr = TMP102_TEMP_LOW_REGISTER;
		tmp102_data_1.Rx.callback = TMP102_LowCallback;
		noAck = true;
	}
	else if(strncmp(token, "high", strlen("high")) == 0)
	{
		tmp102_data_1.Rx.registerAddr = TMP102_TEMP_HIGH_REGISTER;
		tmp102_data_1.Rx.callback = TMP102_HighCallback;
		noAck = true;
	}
	else
	{
		return VALUE_NOT_VALID;
	}

	tmp102_data_1.Rx.regSize = 1;
	tmp102_data_1.Rx.dataSize = 2;

	status = I2C_Mem_Read_Generic_IT(&tmp102_data_1);
	if(status != NO_ERROR)
	{
		return status;
	}

	if(noAck)
	{
		return NO_ACK;
	}

	return status;
}

/*
 * Description: Uses I2C_Mem_Write_Generic so be sure to assign callback function
 *
 */
int TMP102_SetRegisterHandle(char *msg)
{
	int status = NO_ERROR;
	char *token;
	char *token2;
	char *rest = msg;
	char delim[] =",\r";
	float fValue;
	uint16_t regValue;
	bool noAck = false;

	token = strtok_r(rest, delim, &rest); // register
	token2 = strtok_r(rest, delim, &rest); // data value

	if(strncmp(token, "temp", strlen("temp")) == 0)
	{
		tmp102_data_1.Tx.registerAddr = TMP102_TEMPERATURE_REGISTER;
	}
	else if(strncmp(token, "config", strlen("config")) == 0)
	{
		tmp102_data_1.Tx.registerAddr = TMP102_CONFIGURATION_REGISTER;
	}
	else if(strncmp(token, "low", strlen("low")) == 0)
	{
		tmp102_data_1.Tx.registerAddr = TMP102_TEMP_LOW_REGISTER;
		noAck = true;
	}
	else if(strncmp(token, "high", strlen("high")) == 0)
	{
		tmp102_data_1.Tx.registerAddr = TMP102_TEMP_HIGH_REGISTER;
	}
	else
	{
		return VALUE_NOT_VALID;
	}

	fValue = atof(token2); // string to float

	regValue = (uint16_t)(fValue * 16);

	tmp102_data_1.Tx.dataSize = 2;
	tmp102_data_1.Tx.data[0] = regValue >> 8;
	tmp102_data_1.Tx.data[1] = regValue;

	status = I2C_Mem_Write_Generic(&tmp102_data_1);
	if(status != NO_ERROR)
	{
		return status;
	}

	if(noAck)
	{
		return NO_ACK;
	}

	return status;
}



/*
 * Description:
 *
 * Input:
 * Output:
 */
int TMP102_SetThermostatMode(char *msg)
{
    int status = NO_ERROR;
    char *token;
    char *rest = msg;
    char delim[] = ",\r";
    TMP102_Configuration TMP102_Configuration = {0};

    token = strtok_r(rest, delim, &rest);

    // set to read register
    tmp102_data_1.Rx.regSize = 1;
    tmp102_data_1.Rx.registerAddr = TMP102_CONFIGURATION_REGISTER;
    tmp102_data_1.Rx.dataSize = 2;
    // Read register
    status = I2C_Mem_Read_Generic(&tmp102_data_1);
    if (status != NO_ERROR)
    {
    	return status;
    }

    // modify bit
    TMP102_Configuration.Byte.byte0 = tmp102_data_1.Rx.data[0];
    TMP102_Configuration.Status.TM = atoi(token);
    tmp102_data_1.Tx.dataSize = 1;
    tmp102_data_1.Tx.data[0] = TMP102_Configuration.Byte.byte0;

    // set to write data
    tmp102_data_1.Tx.regSize = 1;
	tmp102_data_1.Tx.registerAddr = TMP102_CONFIGURATION_REGISTER;
	tmp102_data_1.Tx.dataSize = 2;
	tmp102_data_1.Tx.data[0] = TMP102_Configuration.Byte.byte0;
	tmp102_data_1.Tx.data[1] = TMP102_Configuration.Byte.byte1;
	// write data
    status = I2C_Master_Transmit_Generic(&tmp102_data_1);
    if(status != NO_ERROR)
    {
        return status;
    }

    return status;
}

/*
 * Description: Get string format of temperature Celsius/Fahrenheit values
 *
 * Input: The leading string and the temperature value to convert to string
 * Output: The full char string
 */
void TMP101_GetTemperatureString(uint8_t *tmp101Value, char *retStr)
{
    char str2[32] = {0};
    uint16_t u16Value;
    bool isNeg = false;

    TMP102_SwapBytes(tmp101Value); // need to first swap bytes

    memcpy(&u16Value, tmp101Value, sizeof(uint16_t));

    u16Value = u16Value >> 4; // need to shift to right 4 bits.

    if(u16Value >= 0x800) // test for negative bit.
    {
    	u16Value ^= 0x800; // invert bit back to zero
        isNeg = true;
    }

    if(isNeg)
    {
        strcat(retStr, "-");
    }

    sprintf(str2, "%.1f C", (float)(u16Value / 16));
    strcat(retStr, str2);

    if(isNeg)
    {
        strcat(retStr, " -");
    }
    else
    {
        strcat(retStr, ",");
    }

    sprintf(str2, "%.1f F", ((u16Value / 16) * 1.8) + 32 ); // get Fahrenheit
    strcat(retStr, str2);
}

/*
 * Description:
 *
 * Input:
 * Output:
 */
void TMP102_SwapBytes(uint8_t *data)
{
    uint8_t tmp;

    tmp = data[0];
    data[0] = data[1];
    data[1] = tmp;
}


/*
 * Description: USER can create callbacks in this section
 *
 */

void TMP102_TemperatureCallback(void)
{
	char str[64] = {0};
	char str2[32] = {0};

	TMP101_GetTemperatureString(tmp102_data_1.Rx.data, str2);
	sprintf(str, "get tmp102:temp=%s", str2);

	UART_DMA_NotifyUser(&uart1_msg, str, strlen(str), true);
}

void TMP102_ConfigCallback(void)
{
	char str[64] = {0};
	TMP102_Configuration config = {0};

	config.Byte.byte0 = tmp102_data_1.Rx.data[0];
	config.Byte.byte1 = tmp102_data_1.Rx.data[1];

	sprintf(str, "get tmp102:config=CR0_1:%d, Fault:%d", config.Status.CR0_1, config.Status.F0_1);

	UART_DMA_NotifyUser(&uart1_msg, str, strlen(str), true);
}

void TMP102_LowCallback(void)
{
	char str[64] = {0};
	char str2[32] = {0};

	TMP101_GetTemperatureString(tmp102_data_1.Rx.data, str2);
	sprintf(str, "get tmp102:low=%s", str2);

	UART_DMA_NotifyUser(&uart1_msg, str, strlen(str), true);
}

void TMP102_HighCallback(void)
{
	char str[64] = {0};
	char str2[32] = {0};

	TMP101_GetTemperatureString(tmp102_data_1.Rx.data, str2);
	sprintf(str, "get tmp102:high=%s", str2);

	UART_DMA_NotifyUser(&uart1_msg, str, strlen(str), true);
}


