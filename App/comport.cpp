/* comport.cpp
 *
 * Created on: Mar 7, 2022
 * Author: Martin Danek, martin@embedblog.eu
 */

#include "comport.h"

#include "proj_conf.h"
#include "hal_inc.h"
#include "main.h"

#include <ctype.h>

extern UART_HandleTypeDef huart2;
extern volatile Mode mode;
extern volatile bool isSendingRawData;
extern volatile bool isEepromSaveRequested;

static uint8_t rxData[16];

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if (huart->Instance == huart2.Instance)
	{
		switch(tolower(rxData[0]))
		{
			case 'f': mode = Mode::SEND_5HZ; break;
			case 'o': mode = Mode::SEND_1HZ; break;
			case 's': mode = Mode::STOP; break;
			case 'd': mode = Mode::SEND_SINGLE; break;
			case 'r': isSendingRawData = !isSendingRawData; break;
			case 'e': isEepromSaveRequested = true; break;
			default: break;
		}

		HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rxData, sizeof(rxData));
	}
}

void comport::init()
{
	MX_USART2_UART_Init();
	HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rxData, sizeof(rxData));
	printf("Starting device...\n");

	//TODO: print initial info
}

extern "C" int _write(int file, char* ptr, int len)
{
	auto status = HAL_UART_Transmit(&huart2, (uint8_t*)ptr, len, 100);

	if (status == HAL_OK)
		return len;
	else
		return 0;
}

void custom_assert(const char* file, uint32_t line)
{
	printf("ASSERT FAILED! File %s, line %u\n", file, (unsigned int)line);

	#if defined(DEBUG)
	__asm("bkpt 0");		//breakpoint instruction
	#endif
}
