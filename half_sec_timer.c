#include "cmsis_os.h"
#include "half_sec_timer.h"
#include "leds_control.h"
#include "rtc_control.h"
#include "bits.h"

#define NTP_FIRST_DELAY (2 * 10) // 10 sec
#define NTP_REQ_PERIOD (2 * 60 * 60 * 25) //25 hours
#define NTP_REREQ_DELAY (2 * 60 * 30) //30 min

//extern osThreadId ledsThreadId;
extern osThreadId netThreadId;
extern osThreadId displayThreadId;
uint32_t counterNtp;

void halfSecTimerInit()
{
	//pause before first NTP request
	counterNtp = NTP_REQ_PERIOD - NTP_FIRST_DELAY;
}

void halfSecTimerCB  (void const *arg)
{
	if (counterNtp == 0)
	{
		//request unix time
		osSignalSet (netThreadId, FLAG_NTP_REQ_START);
	}
	
	counterNtp++;
	if (counterNtp > NTP_REQ_PERIOD)
		counterNtp = 0;
	
	readRtcTime();
	osSignalSet (displayThreadId, FLAG_INVERSE_COLON);
	ledsPeriodicBlink();
}
void scheldueNTPRequest(void)
{
	//pause after failed NTP request
	counterNtp = NTP_REQ_PERIOD - NTP_REREQ_DELAY;
}
