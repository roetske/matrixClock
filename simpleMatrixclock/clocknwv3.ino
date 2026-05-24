/*
esp32 met antenne
levelconverter
4 blocks voor tijd
hoe wifi goed zette
Stap 1: Verbinden met de ESP32
Open de Wi-Fi instellingen op je telefoon.

Zoek in de lijst naar het netwerk met de naam ESP32-Clock-Setup (dit is de naam die je in de code hebt meegegeven aan WiFiManager).

Tik erop om ermee te verbinden. Let op: je hebt geen wachtwoord nodig.

Als je telefoon klaagt dat dit netwerk "Geen internetverbinding" heeft, klopt dat! Kies er expliciet voor om toch verbonden te blijven.

Stap 2: Het configuratiescherm openen
Bij de meeste telefoons opent er nu automatisch een pop-upscherm (een zogenaamde 'inlogpagina' of captive portal).

Mocht er niets automatisch openen:

Open de internetbrowser (zoals Chrome of Safari) op je telefoon.

Typ in de adresbalk exact het volgende IP-adres in en druk op enter: 192.168.4.1
*/
#include <WiFi.h>
#include <WiFiManager.h> 
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <time.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW  
#define MAX_DEVICES 4                      

#define CS_PIN    5   
#define DATA_PIN  23  
#define CLK_PIN   18  

MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

const char* TZ_INFO = "CET-1CEST,M3.5.0,M10.5.0/3"; 
const char* NTP_SERVER = "pool.ntp.org";

String version = "version3"; // Aangepast naar String met hoofdletter S
char timeBuffer[20];
unsigned long lastTimeUpdate = 0;

// Helper function to handle scrolling text smoothly during blocking operations
void scrollMessage(const char* msg) {
  P.displayText(msg, PA_CENTER, 60, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
  P.displayReset(); // Zorg voor een schone start van de animatie
  
  // Run a quick loop to let the text scroll completely across the screen once
  unsigned long start = millis();
  while (!P.displayAnimate()) {
    // Break out if it takes too long (failsafe verhoogd naar 20000ms zodat de hele string uitloopt)
    if (millis() - start > 20000) break; 
    delay(15); // Iets meer ademruimte voor achtergrondtaken zoals Wi-Fi
  }
}

void setup() {
  Serial.begin(115200);

  P.begin();
  P.setIntensity(2); 
  P.displayClear();
  
  // Scroll our opening instruction!
  scrollMessage("CONNECT TO: ESP32-CLOCK-SETUP TO SET WIFI");

  WiFiManager wm;
  wm.setConnectTimeout(30); 
  
  // This blocks execution, but the display will hold the last frame of the scroll cleanly
  if(!wm.autoConnect("ESP32-Clock-Setup")) {
      Serial.println("Failed to connect or timed out. Restarting...");
      scrollMessage("CONNECTION FAILED! RESTARTING...");
      delay(1000);
      ESP.restart();
  }

  Serial.println("Connected to Wi-Fi successfully!");
  scrollMessage("WIFI OK! SYNCHING TIME...");

  configTzTime(TZ_INFO, NTP_SERVER);
  
  struct tm timeinfo;
  int retryCounter = 0;
  while (!getLocalTime(&timeinfo) && retryCounter < 10) {
    Serial.println("Waiting for NTP time sync...");
    delay(1000);
    retryCounter++;
  }
  
  P.displayClear();
}

void loop() {
  // Update time buffer every 500 milliseconds (cruciaal om het knipperen strak te houden)
  if (millis() - lastTimeUpdate >= 500) {
    lastTimeUpdate = millis();
    
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
      
      // Bepaal of de dubbele punt aan of uit moet (wisselt elke seconde)
      // We gebruiken de seconden-teller van de interne klok (timeinfo.tm_sec)
      if (timeinfo.tm_sec % 2 == 0) {
        // Rechte 24h weergave met dubbele punt (bijv. 14:35)
        strftime(timeBuffer, sizeof(timeBuffer), "%H:%M", &timeinfo);
      } else {
        // 24h weergave met een spatie in plaats van de dubbele punt (bijv. 14 35)
        strftime(timeBuffer, sizeof(timeBuffer), "%H %M", &timeinfo);
      }
      
      // Display the ticking clock centered (this fits completely on 4 blocks)
      P.setTextAlignment(PA_CENTER);
      P.print(timeBuffer);
    } else {
      P.print("NO TIME");
    }
  }
}