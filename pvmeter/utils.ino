//
// utils.ino
//

#include "utils.h"
#include "config.h"
#include "pins.h"

// convert hex to decimal ... and vice versa
byte __attribute__ ((noinline)) dec2hex(byte val) {
  return ((val/10*16) + (val%10));
}
byte __attribute__ ((noinline)) hex2dec(byte val) {
  return ( (val/16*10) + (val%16) );
}

// read sensor value MAX method
int read_sensor_max(float *sensor, int mcount) {
  unsigned int s = 0; // sensor
  unsigned int s_max = 0; // sensor max
  uint32_t start_time = millis();

  while((millis()-start_time) < mcount) {
    s = analogRead(transformer);
    if(s > s_max) { s_max = s; }
  }
  *sensor = s_max * 230 / 220 / 10;
  // threthold
  if(*sensor < SENSOR_THRETHOLD) { *sensor = 0; }
  if(s_max == 0) { return(-1); }
  else { return(0); }
}

// read sensor value AVERAGE
int read_sensor(float *sensor, int mcount) {
  unsigned int s = 0; // sensor
  unsigned int s_max = 0; // sensor max
  //unsigned int s_sum = 0; // sensor sum
  uint32_t s_sum = 0; // sensor sum
  uint32_t start_time = millis();
  //while((millis()-start_time) < mcount) {
  for (int i=0; i< mcount; i++) {
    s = analogRead(transformer);
    if(s > s_max) { s_max = s; }
    s_sum += s;
    delay(1);
  }

  Serial.print("MAX: ");
  float m = s_max;
  m = m * 230 / 220 / 10;
  Serial.printf("%.2f", m);
  Serial.print(" SUM: ");
  float sum = s_sum;
  // TODO: fine tune scaling
  sum = sum / 10 / mcount;
  Serial.printf("%.2f", sum);
  Serial.print(" DUR: ");
  Serial.print(millis() - start_time);
  Serial.println();
  *sensor = sum;
  // threthold
  if(*sensor < SENSOR_THRETHOLD) { *sensor = 0; }

  if(s_sum == 0) { return(-1); }
  else { return(0); }
}

int print_date (tm now) {
  char buffer[64] = {0};
  sprintf (buffer,"%04d/%02d/%02d %02d:%02d:%02d %s",now.tm_year,now.tm_mon,now.tm_mday,now.tm_hour,now.tm_min,now.tm_sec,TZ);
  Serial.println(buffer);
  return(0);
}

int print_data () {
  char buffer[64] = {0};
  Serial.println(buffer);
  return(0);
}

int print_status(status_t status) {
  Serial.println("system status:");
  Serial.print(" rtc:    "); if(status.rtc==0) { Serial.println("ok"); } else { Serial.println("-"); }
  Serial.print(" sensor: "); if(status.sensor==0) { Serial.println("ok"); } else { Serial.println("-"); }
  Serial.print(" fs:     "); if(status.fs==0) { Serial.println("ok"); } else { Serial.println("-"); }
  Serial.print(" tft:    "); if(status.tft==0) { Serial.println("ok"); } else { Serial.println("-"); }
  Serial.print(" wifi:   "); if(status.wifi==0) { Serial.println("ok"); } else { Serial.println("-"); }
  Serial.print(" umts:   "); if(status.umts==0) { Serial.println("ok"); } else { Serial.println("-"); }
  Serial.print(" cloud:  "); if(status.cloud==0) { Serial.println("ok"); } else { Serial.println("-"); }
  Serial.print(" ntp:    "); if(status.ntp==0) { Serial.println("ok"); } else { Serial.println("-"); }
  return(0);
}

