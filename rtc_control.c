#include "cmsis_os.h"
#include "Driver_I2C.h"
#include <time.h>
#include "leds_control.h"
#include "rtc_control.h"
#include "bits.h"

#define TASK_LED ledYellow

#define RTC_ADDR 0x68
//#define RTC_ADDR 0xD0
//#define DS3231_ADDR_R	0xD1
//#define DS3231_ADDR_W	0xD0
//#define UTC_OFFSET 3

int8_t utcOffset; // = UTC_OFFSET;

typedef enum
{
	errOK = 0
} rtcErr;

time_t ntpEpochTime = 0;
struct tm localBrokenTime,
	rtcBrokenTime;

uint8_t i2cBuf[32];

/* I2C Driver */
extern ARM_DRIVER_I2C Driver_I2C0;
static ARM_DRIVER_I2C * I2Cdrv = &Driver_I2C0;

extern osMutexId mutexI2C0, mutexRTCtime;

uint8_t dec2bcd(uint8_t);
uint8_t bcd2dec(uint8_t);
void convertEpochToLocal(void);
rtcErr rtcWrite(void);
rtcErr rtcRead(void);

void I2CInit()
{
	ledSetState(TASK_LED, LedOn);
	
	osMutexWait(mutexI2C0, 0);
	
	I2Cdrv->Initialize(NULL);
	I2Cdrv->PowerControl(ARM_POWER_FULL);
	I2Cdrv->Control(ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_FAST);
	//I2Cdrv->Control(ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_STANDARD);
	I2Cdrv->Control(ARM_I2C_BUS_CLEAR, 0);
	
	osMutexRelease(mutexI2C0);
	
	ledSetState(TASK_LED, LedOff);
}

void I2CWait()
{
	while (I2Cdrv->GetStatus().busy)
		osThreadYield();
}

void updateRtcTime()
{
	if (ntpEpochTime == 0)
		return;
	
	ledSetState(TASK_LED, LedOn);
	
	convertEpochToLocal();
	rtcWrite();
	
	ledSetState(TASK_LED, LedOff);
}

void readRtcTime()
{
	ledSetState(TASK_LED, LedOn);
	
	rtcRead();
	
	ledSetState(TASK_LED, LedOff);
}

void convertEpochToLocal()
{
	time_t localEpoch;
	struct tm *pLocalBrokenTime;
	
	if (ntpEpochTime == 0)
		return;
	
	localEpoch = ntpEpochTime;// + utcOffset * 3600;
	//pLocalBrokenTime = gmtime(&localEpoch);
	pLocalBrokenTime = localtime(&localEpoch);
	localBrokenTime = *pLocalBrokenTime;
	//localBrokenTime.tm_year %= 100;
}

rtcErr rtcRead()
{
	time_t tmpUnixTime;
	
	i2cBuf[0] = 0;
	i2cBuf[1] = 0;
	i2cBuf[2] = 0;
	i2cBuf[3] = 0;
	i2cBuf[4] = 0;
	i2cBuf[5] = 0;
	i2cBuf[6] = 0;
	
	osMutexWait(mutexI2C0, 0);
	
	I2CWait();
	i2cBuf[0] = 0; //reading
	I2Cdrv->MasterTransmit (RTC_ADDR, &i2cBuf[0], 1, true);
  I2CWait();
	
	//reading firts 7 registers
	I2Cdrv->MasterReceive (RTC_ADDR, &i2cBuf[0], 7, false); //stop
  I2CWait();
	
	osMutexRelease(mutexI2C0);

	osMutexWait(mutexRTCtime, 0);

	rtcBrokenTime.tm_sec = bcd2dec(i2cBuf[RTC_SECONDS]);
	rtcBrokenTime.tm_min = bcd2dec(i2cBuf[RTC_MINUTES]);
	rtcBrokenTime.tm_hour = bcd2dec(i2cBuf[RTC_HOURS]);
	rtcBrokenTime.tm_wday = i2cBuf[RTC_DAY] - 1;
	rtcBrokenTime.tm_mday = bcd2dec(i2cBuf[RTC_DATE]);
	rtcBrokenTime.tm_mon = bcd2dec(i2cBuf[RTC_MONTH]) - 1;
	rtcBrokenTime.tm_year = bcd2dec(i2cBuf[RTC_YEAR]) + 100;

	tmpUnixTime = mktime(&rtcBrokenTime);
	tmpUnixTime += 3600 * utcOffset;
	rtcBrokenTime = *localtime(&tmpUnixTime);
	
	osMutexRelease(mutexRTCtime);

	return errOK;
}

rtcErr rtcWrite()
{
	volatile int32_t result;
	uint8_t idx = 0;
	
	osMutexWait(mutexI2C0, 0);

	I2CWait();
	i2cBuf[idx++] = 0; //writing
	//i2cBuf[idx++] = RTC_SECONDS;
	i2cBuf[idx++]		= dec2bcd((uint8_t)localBrokenTime.tm_sec);
	i2cBuf[idx++]		= dec2bcd((uint8_t)localBrokenTime.tm_min);
	i2cBuf[idx++]			= dec2bcd((uint8_t)localBrokenTime.tm_hour);
	i2cBuf[idx++]				= localBrokenTime.tm_wday + 1;
	i2cBuf[idx++]			= dec2bcd((uint8_t)localBrokenTime.tm_mday);
	i2cBuf[idx++]			= dec2bcd((uint8_t)localBrokenTime.tm_mon + 1);
	i2cBuf[idx++]			= dec2bcd((uint8_t)(localBrokenTime.tm_year % 100));
	result = I2Cdrv->MasterTransmit (RTC_ADDR, &i2cBuf[0], idx, false);
  I2CWait();
	
	osMutexRelease(mutexI2C0);

	return errOK;
}

uint8_t dec2bcd(uint8_t n)
{
	//fast transform
	return n + 6 * (n / 10);
}

uint8_t bcd2dec(uint8_t n)
{
	//fast transform
	return n - 6 * (n >> 4);
}
