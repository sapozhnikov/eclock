#include "leds_control.h"
#include "GPIO_LPC17xx.h"               // Keil::Device:GPIO

#define LED_STATE_ON 0
#define LED_STATE_OFF 1

typedef struct
{
	LedState green;
	LedState red;
	LedState blue;
	LedState yellow;
} LedCfg;

//LedState ledState;
LedCfg ledCfg;
uint32_t gpioBlinkPhase;

void ledInit()
{
	gpioBlinkPhase = LED_STATE_OFF;
	GPIO_SetDir(1, 18, GPIO_DIR_OUTPUT);
	GPIO_SetDir(1, 20, GPIO_DIR_OUTPUT);
	GPIO_SetDir(1, 21, GPIO_DIR_OUTPUT);
	GPIO_SetDir(1, 23, GPIO_DIR_OUTPUT);
	ledSetState(ledGreen, LedOff);
	ledSetState(ledRed, LedOff);
	ledSetState(ledBlue, LedOff);
	ledSetState(ledYellow, LedOff);
}

void ledSetState(LedColor color, LedState state)
{
	uint32_t gpioState;
	switch (state)
	{
		case LedOn:
			gpioState = LED_STATE_ON;
			break;
		case LedOff:
			gpioState = LED_STATE_OFF;
			break;
		case LedBlink:
			gpioState = gpioBlinkPhase;
			break;
	}
	
	switch (color)
	{
		case ledGreen:
			ledCfg.green = state;
			GPIO_PinWrite(1, 18, gpioState);
			break;
		case ledRed:
			ledCfg.red = state;
			GPIO_PinWrite(1, 20, gpioState);
			break;
		case ledBlue:
			ledCfg.blue = state;
			GPIO_PinWrite(1, 21, gpioState);
			break;
		case ledYellow:
			ledCfg.yellow = state;
			GPIO_PinWrite(1, 23, gpioState);
			break;
	}
}

void ledsPeriodicBlink(void)
{
	if (gpioBlinkPhase == LED_STATE_ON)
		gpioBlinkPhase = LED_STATE_OFF;
	else
		gpioBlinkPhase = LED_STATE_ON;
	
	//refreshing LEDs states with new phase
	if (ledCfg.green == LedBlink) 	ledSetState(ledGreen, LedBlink);
	if (ledCfg.red == LedBlink) 		ledSetState(ledRed, LedBlink);
	if (ledCfg.blue == LedBlink) 		ledSetState(ledBlue, LedBlink);
	if (ledCfg.yellow == LedBlink) 	ledSetState(ledYellow, LedBlink);
}
/*
void setLedGreen(uint8_t state)
{
	ledState.led_green = state;
	GPIO_PinWrite(1, 18, state);
}

void setLedRed(uint8_t state)
{
	ledState.led_red	= state;
	GPIO_PinWrite(1, 20, state);
}

void setLedBlue(uint8_t state)
{
	ledState.led_blue	= state;
	GPIO_PinWrite(1, 21, state);
}

void setLedYellow(uint8_t state)
{
	ledState.led_yellow	= state;
	GPIO_PinWrite(1, 23, state);
}

void setLedFromState()
{
	GPIO_PinWrite(1, 18, ledState.led_green);
	GPIO_PinWrite(1, 20, ledState.led_red);
	GPIO_PinWrite(1, 21, ledState.led_blue);
	GPIO_PinWrite(1, 23, ledState.led_yellow);
}
*/
