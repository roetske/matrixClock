/*time and date on matrix
board esp32 dev
     1 levelconverter
     2 matrixled 4 mod
info  
thanks and credits to all who share their code on github
//time posix 
https://remotemonitoringsystems.ca/time-zone-abbreviations.php
https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv for Timezone codes for your region
CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00
https://www.youtube.com/watch?v=iOPBL9POnRc

remarks
oef werkt met datum en week nu nog opkuisen
tijd correct bijhouden of zetten esp32time 
zomer en wintertijd update internet om de 40 min
bij power verlies of reset moet je internet hebben om tijd te herladen
autoconnect versie wel shit stop start eruit wifi

*/
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

#include <ESP32Time.h>
#include <WiFi.h>
#include <WebServer.h>
#include <AutoConnect.h>
#include <elapsedMillis.h>

#include <Wire.h>


//blinkled
#define blinkled 2 //for heartbeat
ESP32Time rtc;
//autoconnect
AutoConnect       portal;
AutoConnectConfig config;

//spi le matrix
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW//PAROLA_HW
#define MAX_DEVICES	8
#define CLK_PIN   18  // or SCK
#define DATA_PIN  23  // or MOSI
#define CS_PIN    5  // or SS
// Hardware SPI connection
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
#define SPEED_TIME  100//75
#define PAUSE_TIME  0
#define MAX_MESG  20

// Global variables
//matrix
char szTime[9];    // mm:ss\0
char szMesg[MAX_MESG+1] = "";
bool flasher = false;  // seconds passing flasher
uint8_t  display = 0;  // current display mode
//internet

const char* ssid =   "your ssid";
const char* password =   "your password";

const char* NTP_SERVER =  "2.be.pool.ntp.org";         //set your timeserver
const char* NTP_SERVER2 = "0.europe.pool.ntp.org";
const char* NTP_SERVER3 = "1.europe.pool.ntp.org";

const char* TZ_INFO    = "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00"; //set your timezone

tm timeinfo;
int heartbeatinterval = 1000;
elapsedMillis heartbeat;
int clockinterval = 1000;
elapsedMillis matrixclock;
int refreshtextinterval = 20000;
elapsedMillis refreshtext;
int updateinterval = 40; //update to ntp in min interval
int lastupdate = 0;
//***********************************

//setup
void setup()
{
  Serial.begin(115200);
  //mcp23017
  //set time to trigger immediate update
  //rtc.setTime(0,10, 0, 1, 1, 1970);
  pinMode(blinkled, OUTPUT);
  digitalWrite(blinkled,HIGH);
  //autoconnect
  setupautoconnect();
  //klok
  getNTPtime_auto() ;
  timeinfo = rtc.getTimeStruct();
  //cheat check update works
  //rtc.setTime(0,10, 0, 1, 1, 1970);
  lastupdate = rtc.getMinute();
  
  //matrix
  P.begin(2);
  P.setZone(0, 0, MAX_DEVICES-5);//message 0 tot 3
  P.setZone(1, MAX_DEVICES-4, MAX_DEVICES-1);//clock 4 tot 7
  P.setInvert(false);
  P.displayZoneText(1, szTime, PA_CENTER, SPEED_TIME, PAUSE_TIME, PA_PRINT, PA_NO_EFFECT);//clock center : shifts according to clock string
  P.displayZoneText(0, szMesg, PA_CENTER, SPEED_TIME, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);//message center
  //clear all displays
  P.displayAnimate();
  P.displayReset(0);
  P.displayReset(1);
  delay(500);
}
//mainloop
void loop()
{
  portal.handleClient();
  P.displayAnimate();
  matrixdisplaytext();
  showmatrixclock();
  checkNTPupdate();   
  f_heartbeat();
   
}

////////////
//subs
////////////

void setupautoconnect() {
  config.apid = "ap_clock";//name accesspoint
  config.psk  = "";//password empty nothing
  config.apip = IPAddress(192,168,10,101); // Sets SoftAP IP address
  config.gateway = IPAddress(192,168,10,1); // Sets WLAN router IP address
  config.netmask = IPAddress(255,255,255,0); // Sets WLAN scope
  config.dns1 = IPAddress(192,168,10,1); // Sets primary DNS address
  config.autoReconnect = true; // Enable auto-reconnect
  config.autoSave = AC_SAVECREDENTIAL_AUTO;
  //AC_SAVECREDENTIAL_NEVER; // No save credential

  //Config.staip = IPAddress(192,168,10,10); // Sets static IP
  //Config.staGateway = IPAddress(192,168,10,1); // Sets WiFi router address
  //Config.staNetmask = IPAddress(255,255,255,0); // Sets WLAN scope
 
  //config.portalTimeout = 60000;  // It will time out in 60 seconds
  config.retainPortal = true;
  config.autoRise = true;  // Suppresses the launch of the captive portal from AutoConnect::begin.
  portal.config(config);    // Don't forget it.
  portal.begin();
}



void getDate(char *psz)
// Code for reading clock date
{
 String date =rtc.getDate();
 int sizestr = date.length()+1;
 char charBuf[sizestr];
 date.toCharArray(charBuf,sizestr);
 sprintf(szMesg,charBuf);
}

void getTime(char *psz, bool f = true)
// Code for reading clock time
{ 
  sprintf(psz, "%02d%c%02d", rtc.getHour(true), (f ? ':' : ' '), rtc.getMinute());

}
void matrixdisplaytext()
{
  if (refreshtext > refreshtextinterval)
  { 
  if (P.getZoneStatus(0))
   {
    switch (display)
    {
      case 0: //weeknumber
        P.setTextEffect(0, PA_SCROLL_LEFT, PA_SCROLL_UP_LEFT);
        getweeknmbr();
        break;

      case 1: //datum gewoon
        P.setTextEffect(0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
        getDate(szMesg);
        break;
      default:  
        display = 0;
        break;
    
    }
    display++;
    if (display >= 2)
      { display = 0;}
    
    P.displayReset(0);
    refreshtext =0;
  }
 }
}
void showmatrixclock()
{ if (matrixclock > clockinterval)
  { 
  // Finally, adjust the time string if we have to
    getTime(szTime, flasher);
    flasher = !flasher;
    P.displayReset(1);
    matrixclock = 0;
  }
}
void checkNTPupdate()
{
  if ((lastupdate + updateinterval) < rtc.getMinute())
   {lastupdate = rtc.getMinute();
    getNTPtime_auto();
   }
  if (rtc.getMinute() == 0)
    {lastupdate = 0;}    
}
