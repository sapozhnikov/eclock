//#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include "cmsis_os.h"
#include "rl_net.h"
#include "rl_net_lib.h"
#include "net_task.h"
#include "leds_control.h"
#include "rtc_control.h"
#include "bits.h"

extern time_t ntpEpochTime;

typedef enum
{
	errOK = 0,
	errDnsResolve,
	errDnsTOut,
	errNtpCBackTOut,
	errNtpNotReady
} netTaskErr;

static void sntpCBack (uint32_t time); //callback function
static void dnsCBack (dnsClientEvent event, const uint8_t *ip_addr); //callback function
netTaskErr getEpochTime (void);
void updateRtcTime(void);

uint8_t ntpHostIP[4];
//volatile static uint32_t epochTime; //UNIX time from internet
//volatile static uint8_t hours, mins;

extern LOCALM localm[];
osThreadId netTaskId; // ID of Net Thread

const char ntpHostName[] = "pool.ntp.org";

#define TASK_LED ledBlue
#define UDP_TIMEOUT_MS 5000
#define DNS_TIMEOUT_MS 5000

#define DHCP_WAIT_DELAY_MS 5000
#define DHCP_WAIT_TRIES 7
#define DNS_WAIT_DELAY_MS 10000
#define DNS_WAIT_TRIES 10
#define SNTP_WAIT_DELAY_MS 10000
#define SNTP_WAIT_TRIES 10

void net_task (void const *arg)
{
	uint8_t count;
	bool ntpSuccess;
	
	netTaskErr resultGetTime;
	
	netTaskId = osThreadGetId();
	
	while (1)
	{
		osSignalWait(FLAG_NTP_REQ_START, osWaitForever);
	
		ntpSuccess = false;
		ledSetState(TASK_LED, LedOn);
		for (count = 0; count < DHCP_WAIT_TRIES; count++)
		{
			if ((localm[NETIF_ETH].IpAddr[0] != 0) ||
				(localm[NETIF_ETH].IpAddr[1] != 0) ||
				(localm[NETIF_ETH].IpAddr[2] != 0) ||
				(localm[NETIF_ETH].IpAddr[3] != 0))
			{
				resultGetTime = getEpochTime();
				if (resultGetTime == errOK)
				{
					updateRtcTime();
					ntpSuccess = true;
					break;
				}
			}
			else
				osDelay(DHCP_WAIT_DELAY_MS);
		}
		if (ntpSuccess == true)
			ledSetState(TASK_LED, LedOff);
		else
			ledSetState(TASK_LED, LedBlink);
	}
}

netTaskErr getEpochTime(void)
{
	uint8_t count;
	netTaskErr result = errOK;
	netStatus sntpStatus,
		dnsStatus;
	osEvent statusNtpWait,
		statusDnsWait;
	
	// getting IP of NTP server
	for (count = 0; count < DNS_WAIT_TRIES; count++)
	{
		osSignalClear(netTaskId, FLAG_DNS_RESOLVED);
		dnsStatus = get_host_by_name (ntpHostName, dnsCBack);
		if (dnsStatus == netOK)
		{
			statusDnsWait = osSignalWait(FLAG_DNS_RESOLVED, DNS_TIMEOUT_MS);
			if ((statusDnsWait.status == osEventSignal) &&
				((ntpHostIP[0] != 0) ||
				(ntpHostIP[1] != 0) ||
				(ntpHostIP[2] != 0) ||
				(ntpHostIP[3] != 0))
			)
			{
				result = errOK;
				break;
			}
			else
			{
				osSignalClear(netTaskId, FLAG_DNS_RESOLVED);
				result = errDnsTOut;
			}
		}
		else
		{
			result = errDnsResolve;
		}
		osDelay(DNS_WAIT_DELAY_MS);
	}
	
	if (result != errOK)
		return result;
	
	// requesting Unix time from NTP server
	for (count = 0; count < SNTP_WAIT_TRIES; count++)
	{
		osSignalClear(netTaskId, FLAG_UDP_PACKET_RECV);
		sntpStatus = sntp_get_time (&ntpHostIP[0], sntpCBack);
		if (sntpStatus == netOK)
		{
			statusNtpWait = osSignalWait(FLAG_UDP_PACKET_RECV, UDP_TIMEOUT_MS);
			if ((statusNtpWait.status == osEventSignal) && (ntpEpochTime != 0))
			{
				result = errOK;
				break;
			}
			else
			{
				osSignalClear(netTaskId, FLAG_UDP_PACKET_RECV);
				result = errNtpCBackTOut;
			}
		}
		else
		{
			result = errNtpNotReady; // SNTP not ready or bad parameters.
		}
		osDelay(SNTP_WAIT_DELAY_MS);
	}
	
	return result;
}

static void sntpCBack (uint32_t time)
{
	ntpEpochTime = time;
	osSignalSet (netTaskId, FLAG_UDP_PACKET_RECV);
	
//  if (time == 0) {
//  }
//  else {
//		epochTime = time;
//		epochTime %= 86400;
//		hours = epochTime / 3600;
//		epochTime %= 3600;
//		mins = epochTime / 60;
//  }
}

// This function is called by the DNS client when dns event occurs.
static void dnsCBack (dnsClientEvent event, const uint8_t *ip_addr)
{
  switch (event)
	{
    case dnsClientSuccess:
      // Host Address successfully resolved.
			ntpHostIP[0] = ip_addr[0];
			ntpHostIP[1] = ip_addr[1];
			ntpHostIP[2] = ip_addr[2];
			ntpHostIP[3] = ip_addr[3];
      break;
		/*
    case dnsClientNotResolved:
      // Host Name does not exist in DNS record database.
      break;
    case dnsClientTimeout:
      // All DNS Resolver retries used up and timeouts expired.
      break;
    case dnsClientError:
      // DNS Protocol Error, invalid or corrupted reply received.
      break;
		*/
		default:
			ntpHostIP[0] = 0;
			ntpHostIP[1] = 0;
			ntpHostIP[2] = 0;
			ntpHostIP[3] = 0;
			break;
  }
	osSignalSet (netTaskId, FLAG_DNS_RESOLVED);
}
