#pragma once
//
// net.h
//

extern int wifi_connect(void);
extern int wifi_scan(void);
extern int wifi_status(void);
extern int umts_test(void);
extern int cloud_connect(NetworkClientSecure netclient);
extern int cloud_test(NetworkClientSecure netclient);
extern int cloud_sensor(NetworkClientSecure netclient, float sensor);
