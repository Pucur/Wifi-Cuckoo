#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <TM1637Display.h>
#include <time.h>
#include <ArduinoOTA.h>

// ------------------------
// WIFI
// ------------------------
const char* ssid = "WIFI_SSID";
const char* password = "WIFI_CREDENTIAL";

bool summerCuckooDone = false;
bool winterCuckooDone = false;
int lastHourChecked = -1;

// ------------------------
// PINOUT
// ------------------------
#define CLK D1
#define DIO D2
#define PIN1 D5   // cuckoo

TM1637Display display(CLK, DIO);

// ------------------------
// NTP
// ------------------------
WiFiUDP udp;
const char* ntpServer = "hu.pool.ntp.org";
const int localNtpPort = 123;

// ------------------------
// TIME
// ------------------------
time_t now;
tm timeinfo;

bool showColon = true;
unsigned long lastUpdate = 0;

bool initialHandled = false;
int lastCuckooHour = -1;
int lastCuckooMinute = -1;
bool skipNextCuckoo = false;


// ------------------------
// CUCKOO
// ------------------------
void sendCuckoo(int count) {
  for (int i = 0; i < count; i++) {
    digitalWrite(PIN1, HIGH);
    delay(50);
    digitalWrite(PIN1, LOW);
    delay(1000);
  }
}

void handleCuckoo(struct tm *t) {
  int hr = t->tm_hour;
  int min = t->tm_min;

  if (min != lastCuckooMinute) {

    if (!initialHandled) {
      int baseHour = 6;
      int toSend = (hr >= baseHour) ? hr - baseHour : (24 - baseHour + hr);
      for (int i = 0; i < toSend; i++) sendCuckoo(1);
      initialHandled = true;
    }
else if (hr != lastCuckooHour) {
    if (skipNextCuckoo) {
        skipNextCuckoo = false;
    } else {
        sendCuckoo(1);
    }
}

    lastCuckooHour = hr;
    lastCuckooMinute = min;
  }
}

// ------------------------
// WIFI CONNECT
// ------------------------
void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

// ------------------------
// NTP TIME SYNC
// ------------------------
void syncTime() {
  configTzTime("CET-1CEST,M3.5.0/2,M10.5.0/3", ntpServer);
  delay(1000);
  time(&now);
}

// ------------------------
// OTA SETUP
// ------------------------
void setupOTA() {
  ArduinoOTA.setHostname("cuckoo");
  ArduinoOTA.setPassword("password");

  ArduinoOTA.begin();
}

void handleDSTCuckoo(struct tm *t) {
  int hr = t->tm_hour;
  int month = t->tm_mon + 1;
  int day = t->tm_mday;

  if (hr == lastHourChecked) return;
  lastHourChecked = hr;

  // ===== SUMMER TIME FIX =====
  if (month == 3 && !summerCuckooDone) {
    tm temp = *t;
    temp.tm_mday = 31;
    mktime(&temp);
    int lastSunday = 31 - temp.tm_wday;

    if (day == lastSunday && hr == 3) {
      sendCuckoo(1);
      summerCuckooDone = true;
    }
  }

  // ===== WINTER TIME FIX =====
  if (month == 10 && !winterCuckooDone) {
    tm temp = *t;
    temp.tm_mday = 31;
    mktime(&temp);
    int lastSunday = 31 - temp.tm_wday;

    if (day == lastSunday && hr == 2) {
      skipNextCuckoo = true;
      winterCuckooDone = true;
    }
  }
}

// ------------------------
// SETUP
// ------------------------
void setup() {
  pinMode(PIN1, OUTPUT);
  digitalWrite(PIN1, LOW);

  display.setBrightness(7);

  connectWiFi();
  udp.begin(localNtpPort);
  syncTime();
  initialHandled = false;

  setupOTA();
}

// ------------------------
// LOOP
// ------------------------
void loop() {

  ArduinoOTA.handle();

  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  unsigned long nowMs = millis();
  if (nowMs - lastUpdate >= 1000) {
    lastUpdate = nowMs;

    time(&now);
    localtime_r(&now, &timeinfo);

static int lastDay = -1;
if (timeinfo.tm_mday != lastDay) {
  lastDay = timeinfo.tm_mday;

  summerCuckooDone = false;
  winterCuckooDone = false;
  lastHourChecked = -1;

}
    
    handleDSTCuckoo(&timeinfo);
    handleCuckoo(&timeinfo);

    int hr = timeinfo.tm_hour;
    int min = timeinfo.tm_min;

    int hr12 = hr % 12;
    if (hr12 == 0) hr12 = 12;

    display.showNumberDecEx(
      hr12 * 100 + min,
      showColon ? 0b11100000 : 0,
      true
    );

    showColon = !showColon;
  }
}

