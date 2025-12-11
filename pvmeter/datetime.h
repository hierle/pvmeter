#pragma once
//
// datetime.h
//

// unixtime offset
#define SECONDS_FROM_1970_TO_2000 946684800
#define SECONDS_FROM_1900_TO_1970 2208988800
//const unsigned long SECONDS_FROM_1900_TO_1970 946684800UL;
//const unsigned long SECONDS_FROM_1900_TO_1970 = 2208988800UL;

#define NTP_LOCAL_PORT 8888   // local port to listen for UDP packets
#define NTP_PACKET_SIZE 48 // NTP time stamp is in the first 48 bytes of the message

extern int rtc_get_date(tm *now);
extern int rtc_get_epoch(time_t *unixtime);
extern long time_diff(tm now, tm then);
extern int rtc_set_time(tm now);
extern void send_ntp_packet(const char *address);
extern int receive_ntp_packet(tm * ntptm);

