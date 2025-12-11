#pragma once
//
// utils.h
//

// convert hex to decimal ... and vice versa
extern byte __attribute__ ((noinline)) dec2hex(byte val);
extern byte __attribute__ ((noinline)) hex2dec(byte val);
//extern float read_sensor(int mcount, status_t *status);
extern int read_sensor(float *sensor, int mcount);
extern int print_date(tm now);
//extern int print_unixtime();
extern int print_data ();
extern int print_status(status_t status);
//extern void showTime();
