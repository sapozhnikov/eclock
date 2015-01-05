typedef struct
{
	volatile bool button1pressed;
	volatile bool button2pressed;
} ButtonsState_t;

void buttonsInit(void);
void checkButtonsState(void);
