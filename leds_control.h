#include <stdint.h>
//#define LED_STATE_ON 0
//#define LED_STATE_OFF 1
//typedef struct
//{
//	uint8_t led_green;
//	uint8_t led_red;
//	uint8_t led_blue;
//	uint8_t led_yellow;
//} LedState;

typedef enum
{
	ledGreen,
	ledRed,
	ledBlue,
	ledYellow
} LedColor;

typedef enum
{
	LedOn,
	LedOff,
	LedBlink
} LedState;

void ledInit(void);
//void setLedGreen(uint8_t state);
//void setLedRed(uint8_t state);
//void setLedBlue(uint8_t state);
//void setLedYellow(uint8_t state);
void ledSetState(LedColor, LedState);
void ledsPeriodicBlink(void);
