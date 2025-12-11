#pragma once
//
// flash.h
//

#include <LittleFS.h>

extern int log_sensor(fs::FS &fs, float sensor);
extern void list_dir(fs::FS &fs, const char *dirname, uint8_t levels);
extern void create_dir(fs::FS &fs, const char *path);
extern void remove_dir(fs::FS &fs, const char *path);
extern void read_file(fs::FS &fs, const char *path);
extern void write_file(fs::FS &fs, const char *path, const char *message);
extern void append_file(fs::FS &fs, const char *path, const char *message);
extern void delete_file(fs::FS &fs, const char *path);

