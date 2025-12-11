//
// display.ino
//

#include <Arduino.h>
#include "display.h"

//void init_display(Adafruit_SH1107 &display) {
void init_display(Adafruit_ST7789 &display) {
  display.init(TFT_HEIGHT, TFT_WIDTH); // TFT
  //display.setRotation(1); // prod
  display.setRotation(3); // testing
  display.fillScreen(BLACK);
  // frame
  display.drawRoundRect(0, 0, display.width(), display.height(), 9, ORANGE);
  // wifi logo
  //display.drawBitmap(5, 4, logo_wifi, LOGO_WIFI_WIDTH, LOGO_WIFI_HEIGHT, 1);
}

void display_date(Adafruit_ST7789 &display, tm now) {
  char buffer[32] = {0};

  display.fillRect(18, 200, 300, 35, BLACK);
  display.setCursor(20,220);
  //display.setFont(&FreeMonoBold12pt7b);
  display.setFont(&FreeMonoBold12pt7b);
  display.setTextColor(WHITE);
  sprintf (buffer,"%02d.%02d.%04d %02d:%02d %s",now.tm_mday,now.tm_mon,now.tm_year,now.tm_hour,now.tm_min,TZ);
  //sprintf (buffer,"%02d:%02d",now.tm_hour,now.tm_min);
  display.print(buffer);
}


void display_sensor(Adafruit_ST7789 &display, float sensor) {
  char buffer[32] = {0};

  display.fillRoundRect(40, 78, 240, 80, 9, ORANGE);
  display.drawRoundRect(40, 78, 240, 80, 9, RED);
  display.setFont(&FreeMonoBold24pt7b);
  display.setTextColor(WHITE);
  display.setCursor(95,132);

  // testing, pls remove
  //sensor = random(900);
  if(sensor<100) {  display.setCursor(95+30,132); }
  if(sensor<10)  {  display.setCursor(95+60,132); }
  sprintf (buffer,"%.0f W",sensor);
  display.println(buffer);
}

void display_status(Adafruit_ST7789 &display, status_t status) {
  //char buffer[32] = {0};
  int color, fcolor, rcolor;
  // rtc sensor fs tft wifi umts cloud

  display.setFont(&FreeSans12pt7b);
  //display.setFont(&FreeSansBold12pt7b);

  if(status.rtc==0) { color=GREEN; fcolor=BLACK; rcolor=ORANGE; } else { color=RED; fcolor=WHITE; rcolor=GREEN; }
  display.fillRoundRect(8, 10, 35, 28, 5, color);
  display.drawRoundRect(8, 10, 35, 28, 5, rcolor);
  display.setTextColor(fcolor);
  display.setCursor(12,30);
  display.println(F("rtc"));

  if(status.sensor==0) { color=GREEN; fcolor=BLACK; rcolor=ORANGE; } else { color=RED; fcolor=WHITE; rcolor=GREEN; }
  display.fillRoundRect(48, 10, 58, 28, 5, color);
  display.drawRoundRect(48, 10, 58, 28, 5, rcolor);
  display.setTextColor(fcolor);
  display.setCursor(52,30);
  display.println(F("sens"));

  if(status.fs==0) { color=GREEN; fcolor=BLACK; rcolor=ORANGE; } else { color=RED; fcolor=WHITE; rcolor=GREEN; }
  display.fillRoundRect(111, 10, 26, 28, 5, color);
  display.drawRoundRect(111, 10, 26, 28, 5, rcolor);
  display.setTextColor(fcolor);
  display.setCursor(115,30);
  display.println(F("fs"));

  if(status.wifi==0) { color=GREEN; fcolor=BLACK; rcolor=ORANGE; } else { color=RED; fcolor=WHITE; rcolor=GREEN; }
  display.fillRoundRect(142, 10, 43, 28, 5, color);
  display.drawRoundRect(142, 10, 43, 28, 5, rcolor);
  display.setTextColor(fcolor);
  display.setCursor(146,30);
  display.println(F("wifi"));

  if(status.umts==0) { color=GREEN; fcolor=BLACK; rcolor=ORANGE; } else { color=RED; fcolor=WHITE; rcolor=GREEN; }
  display.fillRoundRect(190, 10, 52, 28, 5, color);
  display.drawRoundRect(190, 10, 52, 28, 5, rcolor);
  display.setTextColor(fcolor);
  display.setCursor(194,30);
  display.println(F("gsm"));

  if(status.cloud==0) { color=GREEN; fcolor=BLACK; rcolor=ORANGE; } else { color=RED; fcolor=WHITE; rcolor=GREEN; }
  display.fillRoundRect(247, 10, 66, 28, 5, color);
  display.drawRoundRect(247, 10, 66, 28, 5, rcolor);
  display.setTextColor(fcolor);
  display.setCursor(251,30);
  display.println(F("cloud"));

}


/*
FreeMono12pt7b.h		FreeSansBoldOblique12pt7b.h
FreeMono18pt7b.h		FreeSansBoldOblique18pt7b.h
FreeMono24pt7b.h		FreeSansBoldOblique24pt7b.h
FreeMono9pt7b.h			FreeSansBoldOblique9pt7b.h
FreeMonoBold12pt7b.h		FreeSansOblique12pt7b.h
FreeMonoBold18pt7b.h		FreeSansOblique18pt7b.h
FreeMonoBold24pt7b.h		FreeSansOblique24pt7b.h
FreeMonoBold9pt7b.h		FreeSansOblique9pt7b.h
FreeMonoBoldOblique12pt7b.h	FreeSerif12pt7b.h
FreeMonoBoldOblique18pt7b.h	FreeSerif18pt7b.h
FreeMonoBoldOblique24pt7b.h	FreeSerif24pt7b.h
FreeMonoBoldOblique9pt7b.h	FreeSerif9pt7b.h
FreeMonoOblique12pt7b.h		FreeSerifBold12pt7b.h
FreeMonoOblique18pt7b.h		FreeSerifBold18pt7b.h
FreeMonoOblique24pt7b.h		FreeSerifBold24pt7b.h
FreeMonoOblique9pt7b.h		FreeSerifBold9pt7b.h
FreeSans12pt7b.h		FreeSerifBoldItalic12pt7b.h
FreeSans18pt7b.h		FreeSerifBoldItalic18pt7b.h
FreeSans24pt7b.h		FreeSerifBoldItalic24pt7b.h
FreeSans9pt7b.h			FreeSerifBoldItalic9pt7b.h
FreeSansBold12pt7b.h		FreeSerifItalic12pt7b.h
FreeSansBold18pt7b.h		FreeSerifItalic18pt7b.h
FreeSansBold24pt7b.h		FreeSerifItalic24pt7b.h
FreeSansBold9pt7b.h		FreeSerifItalic9pt7b.h
*/

// tft.setFont(&FreeMonoBoldOblique12pt7b);
/*
// In global declarations:
GFXcanvas1 canvas(128, 32); // 128x32 pixel canvas
// In code later:
canvas.println("I like cake");
tft.drawBitmap(x, y, canvas.getBuffer(), 128, 32, foreground, background); // Copy to screen
*/
