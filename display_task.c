#include <time.h>
#include "Driver_SPI.h"
#include "cmsis_os.h"
#include "leds_control.h"
#include "display_task.h"
#include "bits.h"

#define TASK_LED ledRed

extern osThreadId displayThreadId;

extern ARM_DRIVER_SPI Driver_SPI1;
ARM_DRIVER_SPI* SPIdrv = &Driver_SPI1;

extern struct tm rtcBrokenTime;
extern osMutexId mutexRTCtime;

typedef enum
{
	errOk = 0,
	errTimeout
} displayTaskErr;

uint8_t buf[16];
uint8_t colonState;
uint8_t prevDig1 = 255, prevDig2 = 255, prevDig3 = 255, prevDig4 = 255;

uint8_t digitToChar(uint8_t dig);
void displayTime(void);

displayTaskErr setRegister(uint8_t addr, uint8_t data)
{
	osEvent evt;
	
	buf[1] = addr;
	buf[0] = data;
	
	/* Set CS line   */
	//SPIdrv->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);
	SPIdrv->Send(&buf[0], 2);
	/* Clear CS line */
	//SPIdrv->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_ACTIVE);
	
	evt = osSignalWait(FLAG_SPI_TRANSFER_COMPLETE, 100);
	if (evt.status == osEventTimeout)
	{
		return errTimeout;
	}
	
	return errOk;
}

void mySPI_callback(uint32_t event)
{
    switch (event)
    {
    case ARM_SPI_EVENT_TRANSFER_COMPLETE:
                          /* Success: Wakeup Thread */
        osSignalSet(displayThreadId, FLAG_SPI_TRANSFER_COMPLETE);
        break;
    case ARM_SPI_EVENT_DATA_LOST:
        /*  Occurs in slave mode when data is requested/sent by master
            but send/receive/transfer operation has not been started
            and indicates that data is lost. */
                    __breakpoint(0);  /* Error: Call debugger or replace with custom error handling */
        break;
    case ARM_SPI_EVENT_MODE_FAULT:
        /*  Occurs in master mode when Slave Select is deactivated and
            indicates Master Mode Fault. */
                    __breakpoint(0);  /* Error: Call debugger or replace with custom error handling */
        break;
    }
}

void display_task (void const *arg)
{
	osEvent event;
	
	ledSetState(TASK_LED, LedOn);
	
	osDelay(500);
	
	/* Initialize the SPI driver */
	SPIdrv->Initialize(mySPI_callback);
	/* Power up the SPI peripheral */
	SPIdrv->PowerControl(ARM_POWER_FULL);
	/* Configure the SPI to Master, 8-bit mode @10000 kBits/sec */
	SPIdrv->Control(ARM_SPI_MODE_MASTER | ARM_SPI_CPOL0_CPHA0 | ARM_SPI_MSB_LSB | ARM_SPI_SS_MASTER_HW_OUTPUT | ARM_SPI_DATA_BITS(16), 10000000);

	/* Set CS line   */
	SPIdrv->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);
	/* Clear CS line */
	SPIdrv->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_ACTIVE);

	//code B for digits 0-3
	setRegister(REG_DECODE_MODE, BIT_0 | BIT_1 | BIT_2 | BIT_3);
	setRegister(REG_INTENSITY, 0x03);
	setRegister(REG_SCAN_LIMIT, 0x04); //4 digits and colon
	setRegister(REG_SHUTDOWN, BIT_0); //normal operation
	setRegister(REG_DISPLAY_TEST, 0); //normal operation
	
	setRegister(REG_NOOP, 0);
	ledSetState(TASK_LED, LedOff);
	
	while (1)
	{
		//event = osSignalWait(/*FLAG_UPDATE_DISPLAY |*/ FLAG_INVERSE_COLON, osWaitForever);
		event = osSignalWait(0, osWaitForever);
		ledSetState(TASK_LED, LedOn);
		
		if (event.status == osEventSignal)
		{
			//just update digits
			if (event.value.signals & FLAG_UPDATE_DISPLAY)
			{
				displayTime();
			}

			//reverse colon & update digits
			if (event.value.signals & FLAG_INVERSE_COLON)
			{
				if (colonState == COLON_OFF)
				{
					colonState = COLON_ON;
					setRegister(REG_DIGIT_4, SEGMENT_A);
				}
				else
				{
					colonState = COLON_OFF;
					setRegister(REG_DIGIT_4, 0);
				}
				displayTime();
			}
		}
		ledSetState(TASK_LED, LedOff);    
	}
}

void displayTime()
{
	uint8_t dig1, dig2, dig3, dig4;
	
	osMutexWait(mutexRTCtime, 0);
	dig1 = digitToChar(rtcBrokenTime.tm_hour / 10);
	if (dig1 == CHAR_0)
		dig1 = CHAR_BLANK;
	dig2 = digitToChar(rtcBrokenTime.tm_hour % 10);
	dig3 = digitToChar(rtcBrokenTime.tm_min / 10);
	dig4 = digitToChar(rtcBrokenTime.tm_min % 10);
	osMutexRelease(mutexRTCtime);
	
	if (dig1 != prevDig1)
	{
		setRegister(REG_DIGIT_0, dig1);
		prevDig1 = dig1;
	}
	if (dig2 != prevDig2)
	{
		setRegister(REG_DIGIT_1, dig2);
		prevDig2 = dig2;
	}
	if (dig3 != prevDig3)
	{
		setRegister(REG_DIGIT_2, dig3);
		prevDig3 = dig3;
	}
	if (dig4 != prevDig4)
	{
		setRegister(REG_DIGIT_3, dig4);
		prevDig4 = dig4;
	}
}

uint8_t digitToChar(uint8_t dig)
{
	switch (dig)
	{
		case 0:
			return CHAR_0;
		case 1:
			return CHAR_1;
		case 2:
			return CHAR_2;
		case 3:
			return CHAR_3;
		case 4:
			return CHAR_4;
		case 5:
			return CHAR_5;
		case 6:
			return CHAR_6;
		case 7:
			return CHAR_7;
		case 8:
			return CHAR_8;
		case 9:
			return CHAR_9;
		default:
			return CHAR_BLANK;
	}
}
