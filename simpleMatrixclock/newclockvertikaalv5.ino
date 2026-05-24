/*
esp32 dev mod
matrix 4 blokjes max72xx
matrix builder voor cijfers en letters
https://xantorohara.github.io/led-matrix-editor/
custom cijfers en dot + WiFiManager integratie
wifimanager 192.168.4.1
*/
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFiManager.h> // Installeer deze via de Library Manager
#include "time.h"

String version ="version vertikaal v5.0 (WiFiManager)";

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW 
#define MAX_DEVICES 4 

#define DATA_PIN  23  
#define CLK_PIN   18  
#define CS_PIN    5   

MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;      
const int   daylightOffset_sec = 3600; 

// Gewone cijfers (Volgorde: 0 t/m 9)
const uint64_t PROGMEM cijfersHex[] = {
  0x003c666e7666663c, // 0
  0x001818381818187e, // 1
  0x003c66060c30607e, // 2
  0x003c66061c06663c, // 3
  0x000c1c2c4c7e0c0c, // 4
  0x007e607c0606663c, // 5
  0x003c66607c66663c, // 6
  0x007e660c0c181818, // 7
  0x003c66663c66663c, // 8
  0x003c66663e06663c  // 9
};

// Gecorrigeerde cijfers MET DOT (Volgorde exact gelijk: 0 t/m 9)
const uint64_t PROGMEM cijfersMetDotHex[] = {
  0x813c666e7666663c, // 0 met dot
  0x811818381818187e, // 1 met dot
  0x813c66060c30607e, // 2 met dot
  0x813c66061c06663c, // 3 met dot
  0x810c1c2c4c7e0c0c, // 4 met dot
  0x817e607c0606663c, // 5 met dot
  0x813c66607c66663c, // 6 met dot
  0x817e660c0c181818, // 7 met dot
  0x813c66663c66663c, // 8 met dot
  0x813c66663e06663c  // 9 met dot
};

void toonCijferOpBlok(uint8_t deviceId, uint8_t getal, bool metDot) {
  uint64_t image;
  if (metDot) {
    memcpy_P(&image, &cijfersMetDotHex[getal], sizeof(uint64_t));
  } else {
    memcpy_P(&image, &cijfersHex[getal], sizeof(uint64_t));
  }
  
  for (uint8_t col = 0; col < 8; col++) {
    uint8_t kolomData = (image >> (col * 8)) & 0xFF;
    mx.setColumn(deviceId, col, kolomData);
  }
}

// Hulpfunctie om alle blokjes even te vullen (handig tijdens portal-opstart)
void toonStatusLeds(uint8_t b3, uint8_t b2, uint8_t b1, uint8_t b0) {
  toonCijferOpBlok(3, b3, false);
  toonCijferOpBlok(2, b2, false);
  toonCijferOpBlok(1, b1, false);
  toonCijferOpBlok(0, b0, false);
  mx.update();
}

void setup() {
  Serial.begin(115200);
  mx.begin();
  mx.control(MD_MAX72XX::INTENSITY, 2); 
  mx.clear();

  // Visuele indicatie: Toon "0000" om aan te geven dat hij start
  toonStatusLeds(0, 0, 0, 0);

  // WiFiManager initialisatie
  WiFiManager wifiManager;

  // Indien hij niet kan verbinden, start hij een AP genaamd "LantaarnKlok_AP"
  // Zonder wachtwoord, zodat je er direct makkelijk in kan.
  // De code blijft hier "hangen" totdat je succesvol de Wi-Fi hebt ingesteld.
  if (!wifiManager.autoConnect("LantaarnKlok_AP")) {
    Serial.println("Verbinding mislukt en timeout bereikt...");
    delay(10000);
    ESP.restart(); // Herstart als het misgaat
  }

  Serial.println("Succesvol verbonden met Wi-Fi!");
  
  // Tijd ophalen en instellen
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void loop() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    // Toon even viertekenige streepjes (allemaal 1-en) als NTP faalt
    toonStatusLeds(1, 1, 1, 1);
    delay(1000);
    return;
  }

  uint8_t uren   = timeinfo.tm_hour;
  uint8_t minuten = timeinfo.tm_min;
  uint8_t seconden = timeinfo.tm_sec;

  uint8_t urenTiental   = uren / 10;
  uint8_t urenEenheid   = uren % 10;
  uint8_t minutenTiental = minuten / 10;
  uint8_t minutenEenheid = minuten % 10;

  // Ritmisch knipperen op de seconden
  bool toonDot = (seconden % 2 == 0);

  // Onze perfecte blokverdeling:
  toonCijferOpBlok(3, urenTiental, false);     // Top
  toonCijferOpBlok(2, urenEenheid, false);     // Laag 2
  toonCijferOpBlok(1, minutenTiental, toonDot); // Laag 1 + Knipperende dot
  toonCijferOpBlok(0, minutenEenheid, false);   // Bodem

  mx.update(); 
  delay(200);  
}