//
// flash.ino
//

#include <Arduino.h>
#include "flash.h"
#include "datetime.h"


int log_sensor(fs::FS &fs, float sensor){
  File file;
  char buffer[40] = {0};
  char datafile[32] = {0};
  time_t unixtime;
  tm n;

  rtc_get_date(&n);
  sprintf(datafile,"/data/%04d%02d%02d.log",n.tm_year,n.tm_mon,n.tm_mday);
  // /data/YYYYMMDD.log
  file = fs.open(datafile, FILE_APPEND);
  if (!file) {
    //file = LittleFS.open(DATA_FILE, FILE_WRITE);
    file = fs.open(datafile, FILE_WRITE);
    if (!file) { Serial.print(F("ERROR: failed to open log file for writing: ")); Serial.println(datafile); return(-1); }
  }
  rtc_get_epoch(&unixtime);
  //sprintf (buffer,"%d,%.3f\r\n",unixtime,sensor); // CSV format
  //sprintf (buffer,"%s,%d%02d%02d,%lld,%.3f\r\n",SERIALNUMBER,n.tm_year,n.tm_mon,n.tm_mday,unixtime,sensor); // CSV format
  sprintf (buffer,"%d%02d%02d,%lld,%.3f\r\n",n.tm_year,n.tm_mon,n.tm_mday,unixtime,sensor); // CSV format
  if (!file.print(buffer)) { Serial.println(F("ERROR: failed writing to log file ")); Serial.println(datafile); return(-1); }
  file.close();
  return(0);
}

void list_dir(fs::FS &fs, const char *dirname, uint8_t levels) {
  //Serial.printf("listing directory: %s/\r\n", dirname);
  Serial.printf("listing directory: %s\r\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println(F("- failed to open directory"));
    return;
  }
  if (!root.isDirectory()) {
    Serial.println(F(" - not a directory"));
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print(F("  DIR : "));
      Serial.println(file.name());
      if (levels) {
        list_dir(fs, file.path(), levels - 1);
      }
    } else {
      Serial.print(F("  FILE: "));
      Serial.print(file.name());
      Serial.print(F("\tSIZE: "));
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

void create_dir(fs::FS &fs, const char *path) {
  Serial.printf("mkdir %s ... ", path);
  if (fs.mkdir(path)) {
    Serial.println(F("ok"));
  } else {
    Serial.println(F("failed"));
  }
}

void remove_dir(fs::FS &fs, const char *path) {
  Serial.printf("removing Dir: %s\n", path);
  if (fs.rmdir(path)) {
    Serial.println(F("dir removed"));
  } else {
    Serial.println(F("rmdir failed"));
  }
}

void read_file(fs::FS &fs, const char *path) {
  Serial.printf("reading file %s ... ", path);

  File file = fs.open(path);
  if (!file || file.isDirectory()) {
    Serial.println(F("- failed to open file for reading"));
    return;
  }

  Serial.println("");
  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
}

void write_file(fs::FS &fs, const char *path, const char *message) {
  Serial.printf("writing file: %s\r\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println(F("- failed to open file for writing"));
    return;
  }
  if (file.print(message)) {
    Serial.println(F("- file written"));
  } else {
    Serial.println(F("- write failed"));
  }
  file.close();
}

void append_file(fs::FS &fs, const char *path, const char *message) {
  Serial.printf("appending to file: %s\r\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println(F("- failed to open file for appending"));
    return;
  }
  if (file.print(message)) {
    Serial.println(F("- message appended"));
  } else {
    Serial.println(F("- append failed"));
  }
  file.close();
}

void delete_file(fs::FS &fs, const char *path) {
  Serial.printf("deleting file: %s ... ", path);
  if (fs.remove(path)) {
    Serial.println(F("ok."));
  } else {
    Serial.println(F("failed."));
  }
}
