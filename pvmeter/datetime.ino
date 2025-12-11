//
// datetime.ino
//

#include <Arduino.h>
#include "datetime.h"
#include "config.h"
#include "utils.h"


// get time/date from RTC via I2C
int rtc_get_date (tm *now) {
  Wire.beginTransmission(I2C_RTC_ADDR);
  Wire.write(0x00); // set DS3232 register pointer to 0x00 (seconds)
  Wire.endTransmission();
  Wire.requestFrom(I2C_RTC_ADDR, 7);
  now->tm_sec  = hex2dec(Wire.read() & 0x7f);
  now->tm_min  = hex2dec(Wire.read());
  now->tm_hour = hex2dec(Wire.read() & 0x3f);
  Wire.read(); // dayOfWeek
  now->tm_mday = hex2dec(Wire.read());
  now->tm_mon  = hex2dec(Wire.read());
  now->tm_year = 2000 + hex2dec(Wire.read());
  if((now->tm_mon < 0)||(now->tm_mon > 12)) { return(-1); }
  return(0);
}

int rtc_get_epoch(time_t *now) {
	//uint8_t century_bit;
	//uint16_t century;
	//time_t epoch;
	struct tm epoch_tm;

  Wire.beginTransmission(I2C_RTC_ADDR);
  Wire.write(0x00); // set DS3232 register pointer to 0x00 (seconds)
  Wire.endTransmission();
  Wire.requestFrom(I2C_RTC_ADDR, 7); 
	epoch_tm.tm_sec = hex2dec(Wire.read() & 0x7f);
	epoch_tm.tm_min = hex2dec(Wire.read());
	epoch_tm.tm_hour = hex2dec(Wire.read() & 0x3f);
	epoch_tm.tm_wday = hex2dec(Wire.read());
	epoch_tm.tm_mday = hex2dec(Wire.read())+1;
	epoch_tm.tm_mon = hex2dec(Wire.read())-1;
	epoch_tm.tm_year = 70 + hex2dec(Wire.read());

	*now = mktime(&epoch_tm);
	*now += SECONDS_FROM_1970_TO_2000;
  if((*now < 1000000000)||(*now > 3000000000)) { return(-1); }
  else { return(0); }
}

// set RTC date/time (needed only once in battery lifetime)
void rtc_set_date(tm now) {
  Wire.beginTransmission(I2C_RTC_ADDR);
  Wire.write(0x00); // set DS3232 register pointer to 0x00 (seconds)
  Wire.write(dec2hex(now.tm_sec));
  Wire.write(dec2hex(now.tm_min));
  Wire.write(dec2hex(now.tm_hour));  // set 24 hour format (bit 6 == 0)
  Wire.write(4); // day of week 1=sun ... 7=sat
  Wire.write(dec2hex(now.tm_mday));
  Wire.write(dec2hex(now.tm_mon+1));
  //Wire.write(dec2hex(now.tm_year-2000));
  Wire.write(dec2hex(now.tm_year+60));
  Wire.endTransmission();
}

// send an NTP request to the time server at the given address
void send_ntp_packet(const char * address) {
  byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); // NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}

//int receive_ntp_packet(time_t * ntpt) {
int receive_ntp_packet(tm * ntptm) {
  byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

  if (udp.parsePacket()) { // we received a packet, read the data from it
    udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

    // the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, extract the two words:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    time_t epoch = ( highWord << 16 | lowWord ) - SECONDS_FROM_1900_TO_1970;
    //Serial.print("NTP epoch: "); Serial.println(epoch);
    //Serial.print("RTC epoch: "); Serial.println(now);

    // convert to tm*
    ntptm = gmtime(&epoch);
    /*
    Serial.print(ntptm->tm_year+1900);
    Serial.print(" ");
    Serial.print(ntptm->tm_mon+1);
    Serial.print(" ");
    Serial.print(ntptm->tm_mday);
    Serial.print(" ");
    Serial.print(ntptm->tm_hour);
    Serial.print(":");
    Serial.print(ntptm->tm_min);
    Serial.print(":");
    Serial.print(ntptm->tm_sec);
    Serial.println();
    */

    // set RTC from NTP
    if((epoch-now > MAX_TIME_DIFF)||(now-epoch > MAX_TIME_DIFF)) {
      rtc_set_date(*ntptm);
      now = epoch; // in case we jumped back in time :)
    }
    return(0);
  }
  return(-1); 
}
