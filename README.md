# 🕰️ ESP8266 Wi‑Fi Cuckoo Clock (with TM1637 Display)

![My SmartPlant setup](https://i.kek.sh/oIme08u8tA7.jpg)

This is a Wi‑Fi‑based “cuckoo” clock 🐦 running on an ESP8266 that:
- 🌐 Synchronizes the time from NTP.
- 🖥️ Shows the time on a TM1637 4‑digit LED display.
- 🔊 Plays a soft “cuckoo” sound on a relay or buzzer (PIN1) at each hour.
- 🌤️/🌙 Handles Hungarian daylight saving time transitions (CET/CEST).

---

## 💡 Features

- ⏱️ Time retrieved from NTP (`hu.pool.ntp.org`) with automatic time zone handling (`CET-1CEST,M3.5.0/2,M10.5.0/3`).
- 🕛 12‑hour format display (1–12) on the TM1637.
- 🔯 The colon („:”) blinks every second.
- 🚀 On the first boot, the clock “catches up” the time difference from a base hour (e.g. starting from 6).
- 🐤 One cuckoo at each full hour (except for the initial catch‑up right after boot).
- ☀️ **Summer time (DST start)**: on the last Sunday of March at 03:00 an extra cuckoo signals the forward jump.
- 🌧️ **Winter time (DST end)**: on the last Sunday of October at 02:00 one cuckoo is skipped, adjusting to the time zone change.

---

## 🔧 Hardware

- 🧠 ESP8266 (e.g. NodeMCU v2).
- 🖥️ TM1637 4‑digit LED display:
  - CLK → D1  
  - DIO → D2
- 🔔 Cuckoo output:
  - PIN1 → D5 (for a relay or buzzer).

---

## 📦 Software dependencies

- `ESP8266WiFi`
- `WiFiUdp`
- `TM1637Display` 🧩
- `time.h`
- `ArduinoOTA` 📡

Install the TM1637 library in the Arduino IDE Library Manager:
- Search for `TM1637` (usually the `TM1637Display` variant).

---

## ⚙️ Settings

Modify these in the code:

```cpp
const char* ssid = "WIFI_SSID";
const char* password = "WIFI_CREDENTIAL";
```

OTA / network:
- 🌐 OTA hostname: `cuckoo`
- 🔐 OTA password: `password`

---

## 📡 Over‑The‑Air (OTA) update

The firmware can be updated remotely:
- Arduino IDE → Tools → Ports → `cuckoo` (OTA device).
- 🗝️ Use the OTA password you set.

---

## 🚀 How to run

1. 🔗 Assemble the hardware (ESP8266 + TM1637 + relay/buzzer).
2. 📤 Upload the sketch from the Arduino IDE.
3. 💬 Fill in your own Wi‑Fi credentials (`ssid`, `password`).
4. ⚡ Power on the ESP8266; it connects to Wi‑Fi and syncs time from NTP.
5. 🕛 The time appears in 12‑hour format, and the cuckoo sounds every hour.

---

## 🛠️ Development / modifications

- 🎵 Change the sound pattern or duration in `sendCuckoo()`.
- 🕐 Adjust `baseHour` in `handleCuckoo()` if you want a different starting hour.
- 📅 The DST logic (last Sunday of March/October) already calculates the correct day automatically using `temp.tm_mday` and `mktime()`.
