//
// net.ino
//

#include "net.h"
#include "config.h"


int wifi_connect(void) {
  int counter = 0;
  Serial.print("connecting to wifi ...");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    if(counter >7) { Serial.println(" timeout."); WiFi.disconnect(); return(-1); }
    counter++;
    delay(500);
    Serial.print(".");
  }
  Serial.println(" ok.");
  return(0);
}


int wifi_scan(void) {
 // Set WiFi to station mode and disconnect from an AP if it was previously connected.
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.print("starting wifi scan ...");

  // WiFi.scanNetworks will return the number of networks found.
  int n = WiFi.scanNetworks();
  Serial.println(F(" done."));
  if (n == 0) {
    Serial.println(F("no networks found"));
  } else {
    Serial.print(n);
    Serial.println(F(" networks found"));
    Serial.println(F("Nr | SSID                             | RSSI | CH | Encryption"));
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.printf("%2d", i + 1);
      Serial.print(F(" | "));
      Serial.printf("%-32.32s", WiFi.SSID(i).c_str());
      Serial.print(F(" | "));
      Serial.printf("%4ld", WiFi.RSSI(i));
      Serial.print(F(" | "));
      Serial.printf("%2ld", WiFi.channel(i));
      Serial.print(F(" | "));
      switch (WiFi.encryptionType(i)) {
        case WIFI_AUTH_OPEN:            Serial.print("open"); break;
        case WIFI_AUTH_WEP:             Serial.print("WEP"); break;
        case WIFI_AUTH_WPA_PSK:         Serial.print("WPA"); break;
        case WIFI_AUTH_WPA2_PSK:        Serial.print("WPA2"); break;
        case WIFI_AUTH_WPA_WPA2_PSK:    Serial.print("WPA+WPA2"); break;
        case WIFI_AUTH_WPA2_ENTERPRISE: Serial.print("WPA2-EAP"); break;
        case WIFI_AUTH_WPA3_PSK:        Serial.print("WPA3"); break;
        case WIFI_AUTH_WPA2_WPA3_PSK:   Serial.print("WPA2+WPA3"); break;
        case WIFI_AUTH_WAPI_PSK:        Serial.print("WAPI"); break;
        default:                        Serial.print("unknown");
      }
      Serial.println();
      delay(10);
    }
  }
  Serial.println("");

  // Delete the scan result to free memory for code below.
  WiFi.scanDelete();

  // Wait a bit before scanning again.
  //delay(5000);
  return(0);
}

int wifi_status(void) {
  Serial.print("wifi status: ");
  switch (WiFi.status()) {
    case WL_NO_SSID_AVAIL: 
      Serial.println("SSID not found"); break;
    case WL_CONNECT_FAILED:
      Serial.print("connection failed"); break;
    case WL_CONNECTION_LOST: Serial.println("connection lost"); break;
    case WL_SCAN_COMPLETED:  Serial.println("scan completed"); break;
    case WL_DISCONNECTED:    Serial.println("disconnected"); break;
    case WL_CONNECTED:
      Serial.println("connected");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      break;
    default:
      Serial.println(WiFi.status());
      break;
  }
  return(0);
}

int umts_test(void) {
  //IPAddress ip(PING_ADDRESS);
  bool ret = Ping.ping(PING_ADDRESS);
  if (ret) {
    Serial.println("Ping successful");
    return(0);
  } 
  else {
    Serial.println("Ping failed");
    return(-1);
  }
}

// test tcp connection
int cloud_connect(NetworkClientSecure netclient) {
  netclient.setInsecure();  //skip verification
  Serial.print("connecting to "); Serial.print(CLOUD_SERVER);
  if (!netclient.connect(CLOUD_SERVER, CLOUD_PORT)) { Serial.println(" failed"); return(-1); } 
  else { Serial.println(" ok"); }
  netclient.stop();
  return(0);
}

// test https connect
int cloud_test(NetworkClientSecure netclient) {
  char http[128] = {0};

  netclient.setInsecure();  // skip verification
  Serial.print("connecting to "); Serial.print(CLOUD_SERVER);
  if (!netclient.connect(CLOUD_SERVER, CLOUD_PORT)) { Serial.println(" failed"); return(-1); } else { Serial.println(" ok"); }

  // create http request
  sprintf(http,"GET %s HTTP/1.0\r\nHost: %s\r\nConnection: close\r\n\r\n",CLOUD_GET,CLOUD_SERVER);
  // make the https request
  netclient.print(http);

  while (netclient.connected()) {
    String line = netclient.readStringUntil('\n');
    if (line == "\r") { break; } // headers received
  }
  while (netclient.available()) {
    char c = netclient.read();
    Serial.write(c);
  }
  netclient.stop();
  return(0);
}


int cloud_sensor(NetworkClientSecure netclient, float sensor) {
  tm now;
  time_t unixtime;
  //int size = 0;
  char http[128] = {0};
  char post[128] = {0};

  netclient.setInsecure();  // skip verification
  Serial.print("connecting to "); Serial.print(CLOUD_SERVER); Serial.print("... "); 
  if (!netclient.connect(CLOUD_SERVER, CLOUD_PORT)) { Serial.println("failed."); return(-1); }
  else { Serial.println("ok."); }

  rtc_get_date(&now);
  rtc_get_epoch(&unixtime);
  // create post request
  sprintf(post,"day=%d%02d%02d&unixtime=%lld&sensor=%.3f&key=%s",now.tm_year,now.tm_mon,now.tm_mday,unixtime,sensor,SERIALNUMBER);
  // create http request
  sprintf(http,"POST %s HTTP/1.0\r\nHost: %s\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: %d\r\n\r\n",CLOUD_POST,CLOUD_SERVER,strlen(post));
  // make the https request
  netclient.print(http);
  netclient.println(post);

  while (netclient.connected()) {
    String line = netclient.readStringUntil('\n');
    if (line == "\r") { break; } // headers received
  }
  // read body
  while (netclient.available()) {
    char c = netclient.read();
    //Serial.write(c);
  }
  // disconnect
  netclient.stop();
  return(0);
}
