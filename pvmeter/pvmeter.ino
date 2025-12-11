//
// Name:    pvmeter.ino
// Version: v1.4.8
//
//

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSansBold18pt7b.h>
#include <Fonts/FreeSansBold24pt7b.h>
#include <LittleFS.h>
#include <NetworkClientSecure.h>
#include <WiFi.h>
#include <ESPping.h>
//#include <MQTT.h>
#include <WiFiUdp.h>
#include <time.h>

#include "config.h"
#include "pins.h"
#include "serialnr.h"
#include "datetime.h"
#include "flash.h"
#include "display.h"
#include "net.h"
#include "utils.h"

// global variables
status_t status;  // status
time_t now, then, sens, ntpt;
int mode = 0;
tm ntptm;
unsigned long boot_millis;
//SPIClass *vspi = NULL;
//SPIClass *hspi = NULL;

NetworkClientSecure netclient;
//Adafruit_ST7789 display = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
Adafruit_ST7789 display = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST);

WiFiUDP udp;

// setup
void setup() {
  //char buffer[32] = {0};
  tm n;
  float sensor;

  // set up serial line
  Serial.begin(115200);
  // init I2C
  Wire.begin();
  delay(500);
  // get time from DS3231 RTC
  if (rtc_get_epoch(&now) == 0) {
    status.rtc = 0;
  } else {
    status.rtc = -1;
  }
  // check for broken RTC ...
  boot_millis = millis();

  Serial.println(F("-----------------------------------"));
  Serial.print(F("Firmware: "));
  Serial.println(FIRMWARE);
  Serial.print(F("Version:  "));
  Serial.println(VERSION);
  Serial.print(F("SerialNr: "));
  Serial.println(SERIALNUMBER);
  Serial.print(F("Date:     "));
  rtc_get_date(&n);
  print_date(n);
  Serial.println(F("-----------------------------------"));

  //vspi = new SPIClass(VSPI);
  //hspi = new SPIClass(HSPI);
  //vspi->begin();

  // set pinmodes
  pinMode(statusled, OUTPUT);
  pinMode(transformer, INPUT);
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  // mount filesystem / flash
  if (!LittleFS.begin(true)) {
    Serial.println(F("ERROR: Flash mount failed"));
    status.fs = -1;
  }

  // init display
  init_display(display);

  // test sensor
  if (read_sensor(&sensor, SENSOR_SAMPLE) == 0) {
    status.sensor = 0;
  } else {
    status.sensor = -1;
  }

  // initial display time, sensor and status
  if (rtc_get_date(&n) == 0) {
    status.rtc = 0;
  } else {
    status.rtc = -1;
  }
  display_date(display, n);
  display_status(display, status);
  display_sensor(display, 0);

  // test wifi
  if (wifi_connect() == 0) {
    status.wifi = 0;
  } else {
    status.wifi = -1;
  }
  // test umts
  if (umts_test() == 0) {
    status.umts = 0;
  } else {
    status.umts = -1;
  }
  // test cloud
  if (cloud_test(netclient) == 0) {
    status.cloud = 0;
  } else {
    status.cloud = -1;
  }

  // blink led once on startup
  digitalWrite(statusled, HIGH);
  delay(100);
  digitalWrite(statusled, LOW);

}


void loop() {
  //int bytesrec=0;
  int msglen = 63;
  char usbmsg[msglen] = { 0 };
  float sensor = 0;
  tm n;

  // null buffers
  memset(usbmsg, 0, msglen);

  // get time from RTC
  if (rtc_get_epoch(&now) == 0) {
    status.rtc = 0;
    //Serial.print("RTC epoch: "); Serial.println(now);
  } else {
    status.rtc = -1;
  };
  // TODO: check for dead rtc ... too many things depend on it

  // reconnect wifi
  if (status.wifi == -1) {
    if (wifi_connect() == 0) {
      status.wifi = 0;
      display_status(display, status);
    } else {
      status.wifi = -1;
    }
  }
  // re-test umts
  if (status.umts == -1) {
    if (umts_test() == 0) {
      status.umts = 0;
      display_status(display, status);
    } else {
      status.umts = -1;
    }
  }
  // re-test cloud
  if (status.cloud == -1) {
    if (cloud_test(netclient) == 0) {
      status.cloud = 0;
      display_status(display, status);
    } else {
      status.cloud = -1;
    }
  }

  // read sensor
  if (now >= sens + SENSOR_PERIOD) {
    sens = now;
    // sample for 100ms ~ 5x sinus wave
    if (read_sensor(&sensor, SENSOR_SAMPLE) == 0) {
      status.sensor = 0;
    } else {
      status.sensor = -1;
    }
    // display
    display_sensor(display, sensor);
    // log to fs
    if (log_sensor(LittleFS, sensor) == 0) {
      status.fs = 0;
    } else {
      status.fs = -1;
    }
    // log to cloud
    if (cloud_sensor(netclient, sensor) == 0) {
      status.cloud = 0;
    } else {
      status.cloud = -1;
    }
  }

  // update time and status on display, once per minute
  if (now >= then + UPDATE_PERIOD) {
    then = now;
    if (rtc_get_date(&n) == 0) {
      status.rtc = 0;
    } else {
      status.rtc = -1;
    }
    display_date(display, n);
    // status
    display_status(display, status);
  }

  // get ntp time and update rtc
  if(now >= ntpt + NTP_PERIOD) {
    ntpt = now;
    if(status.wifi==0) {
      Serial.print("getting NTP time... ");
      udp.begin(NTP_LOCAL_PORT);
      send_ntp_packet(NTP_SERVER);
      delay(1000);
      if(receive_ntp_packet(&ntptm)==0) { status.ntp = 0; Serial.println("ok."); }
      else { status.ntp = -1; Serial.println("failed."); }
      udp.stop();
      //showTime();
    }
  }

  // serial (USB) from PC/user
  if (Serial.available()) {
    //bytesrec = Serial.readBytes(usbmsg,msglen);
    Serial.readBytes(usbmsg, msglen);
    handle_usb(usbmsg);
  }
  // being polite :)
  delay(10);
}
// end of main loop




// handle serial from pc
void handle_usb(char *msg) {
  char *ptr;
  tm n;
  time_t ut;

  // help
  if ((msg[0] == 'h') || (msg[0] == '?')) {
    print_help();
    return;
  }
  // status
  if (strncmp(msg, "status", 6) == 0) {
    print_status(status);
    return;
  }
  // date
  if (strncmp(msg, "date", 4) == 0) {
    rtc_get_date(&n);
    print_date(n);
    return;
  }
  // unixtime
  if (strncmp(msg, "unixtime", 8) == 0) {
    rtc_get_epoch(&ut);
    Serial.println(ut);
    return;
  }
  // format
  if (strncmp(msg, "format", 6) == 0) {
    LittleFS.format();
    return;
  }
  // mkdir
  if (strncmp(msg, "mkdir", 5) == 0) {
    // strip off \n, \r and spaces
    ptr = strchr(msg, 0x0a);
    if (ptr != NULL) { *ptr = 0; }
    ptr = strchr(msg, 0x0d);
    if (ptr != NULL) { *ptr = 0; }
    ptr = strchr(&msg[6], 0x20);
    if (ptr != NULL) { *ptr = 0; }
    if (!LittleFS.mkdir(&msg[6])) { Serial.print(F("ERROR: ")); }
    return;
  }
  // rmdir
  if (strncmp(msg, "rmdir", 5) == 0) {
    // strip off \n and \r and spaces
    ptr = strchr(msg, 0x0a);
    if (ptr != NULL) { *ptr = 0; }
    ptr = strchr(msg, 0x0d);
    if (ptr != NULL) { *ptr = 0; }
    ptr = strchr(&msg[6], 0x20);
    if (ptr != NULL) { *ptr = 0; }
    if (LittleFS.rmdir(&msg[6])) { Serial.println(F("removed")); }
    return;
  }
  if (strncmp(msg, "ls", 2) == 0) {  // ls
    // strip off \n and \r
    ptr = strchr(msg, 0x0a);
    if (ptr != NULL) { *ptr = 0; }
    ptr = strchr(msg, 0x0d);
    if (ptr != NULL) { *ptr = 0; }
    if (strlen(&msg[3]) == 0) { msg[3] = 0x2f; }
    //Serial.print("->");Serial.print(&msg[3]);Serial.println("<-");
    list_dir(LittleFS, &msg[3], 3);
    return;
  }
  // cat
  if (strncmp(msg, "cat", 3) == 0) {
    // strip off \n and \r and spaces
    ptr = strchr(msg, 0x0a);
    if (ptr != NULL) { *ptr = 0; }
    ptr = strchr(msg, 0x0d);
    if (ptr != NULL) { *ptr = 0; }
    ptr = strchr(&msg[4], 0x20);
    if (ptr != NULL) { *ptr = 0; }
    read_file(LittleFS, &msg[4]);
    return;
  }
  // rm
  if (strncmp(msg, "rm", 2) == 0) {
    // strip off \n and \r and spaces
    ptr = strchr(msg, 0x0a);
    if (ptr != NULL) { *ptr = 0; }
    ptr = strchr(msg, 0x0d);
    if (ptr != NULL) { *ptr = 0; }
    ptr = strchr(&msg[3], 0x20);
    if (ptr != NULL) { *ptr = 0; }
    delete_file(LittleFS, &msg[3]);
    return;
  }
  // print data from today ( YYYYMMDD,unixtime,sensor )
  if (strncmp(msg, "data", 4) == 0) {
    char datafile[32] = { 0 };
    tm now;
    rtc_get_date(&now);
    sprintf(datafile, "/data/%04d%02d%02d.log", now.tm_year, now.tm_mon, now.tm_mday);
    read_file(LittleFS, datafile);
    return;
  }
  // wifi scan
  if (strncmp(msg, "wifiscan", 8) == 0) {
    wifi_scan();
    return;
  }
  // wifi connect
  if (strncmp(msg, "wificonn", 8) == 0) {
    if (wifi_connect() == 0) {
      status.wifi = 0;
    } else {
      status.wifi = -1;
    };
    return;
  }
  // wifi status
  if (strncmp(msg, "wifistatus", 10) == 0) {
    wifi_status();
    return;
  }
  // cloud test
  if (strncmp(msg, "cloudtest", 9) == 0) {
    if (cloud_test(netclient) == 0) {
      status.cloud = 0;
    } else {
      status.cloud = -1;
    };
    return;
  }

  // if we reached here ... it's an unsupported/unknow command ;)
  Serial.println(F("unknown command, try \"help\""));
}


void print_help() {
  Serial.println();
  Serial.print(FIRMWARE);
  Serial.print(F(" v"));
  Serial.println(VERSION);
  Serial.println();
  Serial.println(F("available commands:"));
  Serial.println();
  Serial.println(F("h (or ?)                 : this help"));
  Serial.println(F("status                   : system status"));
  Serial.println(F("date                     : current date / time"));
  //Serial.println(F("setdate <date>           : set rtc date/time, e.g. setdate {14,55,0,2025,1,2}  for 20250102 14:55:00"));
  Serial.println(F("unixtime                 : unixtime (seconds since 1970/01/01)"));
  Serial.println(F("ls <dir>                 : list directory, e.g. ls /data"));
  Serial.println(F("mkdir <dir>              : create directory, e.g. mkdir /tmp"));
  Serial.println(F("rmdir <dir>              : remove directory, e.g. rmdir /tmp"));
  Serial.println(F("cat <file>               : show content of file, e.g. cat /data/20250131.log"));
  Serial.println(F("rm <file>                : remove file"));
  Serial.println(F("wifiscan                 : wifi scan"));
  Serial.println(F("wificonn                 : wifi connect"));
  Serial.println(F("wifistatus               : wifi status"));
  Serial.println(F("cloudtest                : cloud test"));
  Serial.println(F("data                     : print all data from today"));
  Serial.println();
}
