#define REG_NOOP					0x00
#define REG_DIGIT_0				0x01
#define REG_DIGIT_1				0x02
#define REG_DIGIT_2				0x03
#define REG_DIGIT_3				0x04
#define REG_DIGIT_4				0x05
#define REG_DIGIT_5				0x06
#define REG_DIGIT_6				0x07
#define REG_DIGIT_7				0x08
#define REG_DECODE_MODE		0x09
#define REG_INTENSITY			0x0A
#define REG_SCAN_LIMIT		0x0B
#define REG_SHUTDOWN			0x0C
#define REG_DISPLAY_TEST	0x0F

#define CHAR_0				0
#define CHAR_1				BIT_0
#define CHAR_2				BIT_1
#define CHAR_3				(BIT_1 | BIT_0)
#define CHAR_4				BIT_2
#define CHAR_5				(BIT_2 | BIT_0)
#define CHAR_6				(BIT_2 | BIT_1)
#define CHAR_7				(BIT_2 | BIT_1 | BIT_0)
#define CHAR_8				BIT_3
#define CHAR_9				(BIT_3 | BIT_0)
#define CHAR_MINUS		(BIT_3 | BIT_1)
#define CHAR_E				(BIT_3 | BIT_1 | BIT_0)
#define CHAR_H				(BIT_3 | BIT_2)
#define CHAR_L				(BIT_3 | BIT_2 | BIT_0)
#define CHAR_P				(BIT_3 | BIT_2 | BIT_1)
#define CHAR_BLANK		(BIT_3 | BIT_2 | BIT_1 | BIT_0)
#define COLON_ON			1
#define COLON_OFF			0

#define SEGMENT_A		BIT_6
#define SEGMENT_B		BIT_5
#define SEGMENT_C		BIT_4
#define SEGMENT_D		BIT_3
#define SEGMENT_E		BIT_2
#define SEGMENT_F		BIT_1
#define SEGMENT_G		BIT_0
#define SEGMENT_DP	BIT_7

void display_task(void const *arg);
