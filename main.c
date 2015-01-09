#include "LPC17xx.h"
#include <stdio.h>
#include "cmsis_os.h"
#include "rl_net.h"
#include "net_task.h"
#include "display_task.h"
#include "leds_control.h"
#include "half_sec_timer.h"
#include "rtc_control.h"
#include "buttons.h"

#define TASK_LED ledGreen

osThreadDef (net_task, osPriorityNormal, 1, 400);
osThreadDef (display_task, osPriorityNormal, 1, 0);
osTimerDef (halfSecTimer, halfSecTimerCB);
osMutexDef (MutexI2C);
osMutexDef (MutexRTCtime);

osThreadId netThreadId;
osThreadId displayThreadId;

osMutexId mutexI2C0,
	mutexRTCtime;

int main(void)
{
	//timer def
	osTimerId timerId;
	osStatus  status;
  
	SystemCoreClockUpdate ();                 /* Update system core clock       */  
	SystemInit();
	SysTick_Config(SystemCoreClock / 1000);
	
	//giving time for network hardware or we'll can't reach DHCP server
	osDelay(1000);
	net_initialize();
	
	ledInit();
	I2CInit();
	buttonsInit();
	
	ledSetState(TASK_LED, LedOn);
	
  mutexI2C0 = osMutexCreate(osMutex (MutexI2C));
  if (mutexI2C0 == NULL)
	{
		ledSetState(TASK_LED, LedBlink);
  }
	
  mutexRTCtime = osMutexCreate(osMutex (MutexRTCtime));
  if (mutexRTCtime == NULL)
	{
		ledSetState(TASK_LED, LedBlink);
  }
	
	// Create periodic timer
	halfSecTimerInit();
  timerId = osTimerCreate (osTimer(halfSecTimer), osTimerPeriodic, NULL);
  if (timerId != NULL)
	{
		// Periodic timer created
		status = osTimerStart (timerId, 500); // start timer
		if (status != osOK)
		{
			// Timer could not be started
			ledSetState(TASK_LED, LedBlink);
		} 
	}
	else
		ledSetState(TASK_LED, LedBlink);

	netThreadId = osThreadCreate (osThread (net_task), NULL);
	if (netThreadId == NULL)
	{
		//set err state
		ledSetState(TASK_LED, LedBlink);
	}
	
	displayThreadId = osThreadCreate (osThread (display_task), NULL);
	if (displayThreadId == NULL)
	{
		//set err state
		ledSetState(TASK_LED, LedBlink);
	}
	
	while (1)
	{
		net_main();
		checkButtonsState();
		osThreadYield (); //task switch
	}
}
