#include "cmsis_os.h"
#include "GPIO_LPC17xx.h"
#include <stdbool.h>
#include "buttons.h"
#include "bits.h"
#include "iap.h"
#include "leds_control.h"

#define TASK_LED ledGreen
#define	IMG_START_SECTOR	0x00010000 //Sector 16
uint8_t flashImage[IAP_FLASH_PAGE_SIZE_BYTES];

ButtonsState_t buttonsState;

extern int8_t utcOffset;
extern osThreadId displayThreadId;

void button1PressEvent(void);
void button2PressEvent(void);
void saveUtcOffsetToFlash(void);
void loadUtcOffsetFromFlash(void);

void buttonsInit()
{
	GPIO_SetDir(0, 0, GPIO_DIR_INPUT);
	GPIO_SetDir(0, 1, GPIO_DIR_INPUT);
	
	buttonsState.button1pressed = false;
	buttonsState.button2pressed = false;
	
	loadUtcOffsetFromFlash();
}

void checkButtonsState()
{
	if (!GPIO_PinRead(0, 0))
	{
		if (buttonsState.button1pressed == false)
		{
			//button pressed
			buttonsState.button1pressed = true;
			button1PressEvent();
		}
	}
	else
		buttonsState.button1pressed = false;

	if (!GPIO_PinRead(0, 1))
	{
		if (buttonsState.button2pressed == false)
		{
			//button pressed
			buttonsState.button2pressed = true;
			button2PressEvent();
		}
	}
	else
		buttonsState.button2pressed = false;
}

void button1PressEvent()
{
	utcOffset--;
	if (utcOffset < -12)
		utcOffset = 12;
	osSignalSet (displayThreadId, FLAG_UPDATE_DISPLAY);
	saveUtcOffsetToFlash();
}

void button2PressEvent()
{
	utcOffset++;
	if (utcOffset > 12)
		utcOffset = -12;
	osSignalSet (displayThreadId, FLAG_UPDATE_DISPLAY);
	saveUtcOffsetToFlash();
}

void saveUtcOffsetToFlash()
{
	flashImage[0] = utcOffset;

	__disable_irq();
	if(u32IAP_PrepareSectors(16, 16) == IAP_STA_CMD_SUCCESS)
	{
		u32IAP_EraseSectors(16, 16);
		u32IAP_PrepareSectors(16, 16);
		if(u32IAP_CopyRAMToFlash(IMG_START_SECTOR, (uint32_t)flashImage, IAP_FLASH_PAGE_SIZE_BYTES) == IAP_STA_CMD_SUCCESS)
		{
			if(u32IAP_Compare(IMG_START_SECTOR, (uint32_t)flashImage, IAP_FLASH_PAGE_SIZE_BYTES, 0) == IAP_STA_CMD_SUCCESS)
			{
				__enable_irq();
				ledSetState(TASK_LED, LedOn);
				return;
			}
		}
	}
	__enable_irq();
	ledSetState(TASK_LED, LedBlink);
}

void loadUtcOffsetFromFlash()
{
	utcOffset = *(int8_t *)IMG_START_SECTOR;
	if ((utcOffset < -12) || (utcOffset > 12))
	{
		utcOffset = 0;
		ledSetState(TASK_LED, LedBlink);
	}
}
