#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "leds_task.h"
#include "leds_control.h"
#include "bits.h"

void leds_task (void const *arg)
{
	uint8_t seq = 0;

	ledInit();

	osSignalClear(osThreadGetId(), FLAG_DISPLAY_UPDATE);
	
	while (1)
	{
	switch (seq)
		{
			case 0:
				setLedGreen(LED_STATE_ON);
				setLedRed(LED_STATE_OFF);
				setLedBlue(LED_STATE_OFF);
				setLedYellow(LED_STATE_OFF);
				break;
			case 1:
				setLedGreen(LED_STATE_OFF);
				setLedRed(LED_STATE_ON);
				setLedBlue(LED_STATE_OFF);
				setLedYellow(LED_STATE_OFF);
				break;
			case 2:
				setLedGreen(LED_STATE_OFF);
				setLedRed(LED_STATE_OFF);
				setLedBlue(LED_STATE_ON);
				setLedYellow(LED_STATE_OFF);
				break;
			case 3:
				setLedGreen(LED_STATE_OFF);
				setLedRed(LED_STATE_OFF);
				setLedBlue(LED_STATE_OFF);
				setLedYellow(LED_STATE_ON);
				break;
			default:
				break;
		}
		//osDelay(1000);
		seq = (seq + 1) % 4;
		osSignalWait(FLAG_DISPLAY_UPDATE, osWaitForever);
	}
}
