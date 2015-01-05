#define BIT_0 1
#define BIT_1 (1<<1)
#define BIT_2 (1<<2)
#define BIT_3 (1<<3)
#define BIT_4 (1<<4)
#define BIT_5 (1<<5)
#define BIT_6 (1<<6)
#define BIT_7 (1<<7)
#define BIT_8 (1<<8)
#define BIT_9 (1<<9)
#define BIT_10 (1<<10)
#define BIT_11 (1<<11)
#define BIT_12 (1<<12)
#define BIT_13 (1<<13)
#define BIT_14 (1<<14)
#define BIT_15 (1<<15)

//net thread events
#define FLAG_UDP_PACKET_RECV	BIT_0
#define FLAG_DNS_RESOLVED			BIT_1
#define FLAG_NTP_REQ_START		BIT_2

//leds thread events
//#define FLAG_DISPLAY_UPDATE		BIT_0

//display thread events
#define FLAG_UPDATE_DISPLAY					BIT_0
#define FLAG_INVERSE_COLON					BIT_1
#define FLAG_SPI_TRANSFER_COMPLETE	BIT_2
