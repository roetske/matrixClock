bool getNTPtime_auto() 
{   bool ok = false;  
    tm timeinfox;
    Serial.println("check ntp");
    if (WiFi.status() == WL_CONNECTED)
    { Serial.println(WiFi.localIP());
      //NTP short function but works
      configTzTime(TZ_INFO,NTP_SERVER,NTP_SERVER2,NTP_SERVER3);//esp32 only esp8266 withoutTz
      if (getLocalTime(&timeinfox))
        {  rtc.setTimeStruct(timeinfox);
           Serial.println("update NTP clock succeeded");
           Serial.println(rtc.getDateTime());
          ok = true;
        }
     else
      {Serial.println("error getlocaltime");}
      
    }  
    return ok;
}

bool getNTPtime() 
{   bool ok = false;  
    tm timeinfox;
    Serial.println("check ntp");
    if (check_Wifi())
    { //NTP short fucntion but works
      configTzTime(TZ_INFO,NTP_SERVER,NTP_SERVER2,NTP_SERVER3);//esp32 only esp8266 withoutTz
      if (getLocalTime(&timeinfox))
        {  rtc.setTimeStruct(timeinfox);
           Serial.println("update NTP clock succeeded");
           Serial.println(rtc.getDateTime());
          ok = true;
        }
     else
      {Serial.println("error getlocaltime");}
      
    }  
    stop_Wifi();
    
    return ok;
}

int ymdToWeekNumber (int y, int m, int d) {
  // reject out-of-range input
  if ((y < 1583)||(y > 25000)) return 0;
  if ((m < 1)||(m > 12)) return 0;
  if ((d < 1)||(d > 31)) return 0;
  // compute correction for year
  //     If Jan. 1 falls on: Mo Tu We Th Fr Sa Su
  // then the correction is:  0 +1 +2 +3 -3 -2 -1
  int corr = ((y + 2 + (y-1)/4 - (y-1)/100 + (y-1)/400) % 7) - 3;
  // compute day of the year (in range 1-366)
  int doy = d;
  if (m > 1) doy += 31;
  if (m > 2) doy += (((y%100) ? (y%4) : (y%400)) ? 28 : 29);
  if (m > 3) doy += 31;
  if (m > 4) doy += 30;
  if (m > 5) doy += 31;
  if (m > 6) doy += 30;
  if (m > 7) doy += 31;
  if (m > 8) doy += 31;
  if (m > 9) doy += 30;
  if (m > 10) doy += 31;
  if (m > 11) doy += 30;
  // compute corrected day number
  int cdn = corr + doy;
  // compute number of weeks, rounding up to nearest whole week
  // this gives us our week number
  int wknum = ((cdn + 6) / 7);
  // check for boundary conditions:
  // if our calculation would give us "week 0",
  // then it is really the final week of the previous year
  // and we need to find out whether that is week 52 or week 53
  if (wknum < 1) {
    // first, compute correction for the previous year
    corr = ((y + 1 + (y-2)/4 - (y-2)/100 + (y-2)/400) % 7) - 3;
    // then, compute day of year with respect to that same previous year
    doy = d + ((((y-1)%100) ? ((y-1)%4) : ((y-1)%400)) ? 365 : 366);
    // finally, re-compute the corrected day number and the week number
    cdn = corr + doy;
    wknum = ((cdn + 6) / 7);
    return wknum;
  }
  // if our calculation would give us "week 53",
  // we need to find out whether week 53 really exists,
  // or whether it is week 1 of the following year
  if (wknum > 52) {
    // check for year beginning on Thurs.
    if (corr==3) return 53;
    // check for leap year beginning on Wed.
    if (((y%4)==0) && (corr==2)) return 53;
    // otherwise, there is no week 53
    return 1;
  }
  return wknum;
}
void getweeknmbr()
{ int year = rtc.getYear();
  int month = rtc.getMonth() + 1;// verdomme is van 0 tot 11
  int day = rtc.getDay();
  int week = ymdToWeekNumber(year,month,day);
  //mon 1 sunday 0
  int dayofweek = rtc.getDayofWeek();
  if (dayofweek == 0)
   {dayofweek = 7;}
  year = year -2000;
  sprintf(szMesg, "%dW%dD%d",year, week,dayofweek);
  
}

void f_heartbeat()
{
  if (heartbeat > heartbeatinterval)
  {
    digitalWrite(blinkled,!digitalRead(blinkled));
    heartbeat = 0;
  }  
}