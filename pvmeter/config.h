#pragma once
//
// config.h
//

#define VERSION "1.4.8"
#define FIRMWARE "PVMeter"
#define TZ "UTC"

#define USE_HSPI_PORT

// wifi / network
#define WIFI_SSID "mySSID"
#define WIFI_PASS "myPASS"
//#define WIFI_TIMEOUT 5

// umts wifi stick
//#define WIFI_SSID "mySSID"
//#define WIFI_PASS "myPASS"
#define WIFI_TIMEOUT 5

// ping test (gsm/umts status)
//#define PING_ADDRESS "8, 8, 8, 8"
#define PING_ADDRESS "vodafone.com"

// cloud server
#define CLOUD_SERVER "solarbildung.faun.org"
//#define CLOUD_SERVER "cloud.solarbildung.org"
#define CLOUD_PORT 443
#define CLOUD_GET "/status.txt"
#define CLOUD_POST "/pvmeter"

// update time and status every 10 seconds
#define UPDATE_PERIOD 10

// sensor
//#define SENSOR_PERIOD 60 // seconds between sensor measure
#define SENSOR_PERIOD 5 // seconds between sensor measure
//#define SENSOR_THRETHOLD 35 // cut noise below 35 watt
#define SENSOR_THRETHOLD 5 // cut noise below 5 watt
#define SENSOR_SAMPLE 100 // sample for 100x1ms
//#define SENSOR_FILE "/data/data.log" // fallback in case we do not have RTC

// update ntp time every 5 minutes
//#define NTP_PERIOD 300
#define NTP_PERIOD 30
#define NTP_SERVER "de.pool.ntp.org"
#define MAX_TIME_DIFF 60  // max time diff between RTC and NTP

// I2C
#define I2C_RTC_ADDR     0x68
//#define I2C_DISPLAY_ADDR 0x3C
//#define I2C_EPROM_ADDR     0x22

// mode
#define MODE_AP            1  // access point
#define MODE_WIFI          2  // wifi connect

// status
#define OK          0
#define FAILED      -1

// status
typedef struct {
  int all;
  int rtc = -1;
  int sensor = -1;
  int fs = -1;
  int tft = -1;
  int wifi = -1;
  int umts = -1;
  int cloud = -1;
  int ntp = -1;
} status_t;
