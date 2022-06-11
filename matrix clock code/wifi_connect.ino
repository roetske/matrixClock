bool init_Wifi()
{ WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  int counter = 0;
  bool ok = false;

  while ((WiFi.status() != WL_CONNECTED) && (counter < 100))
  { delay(200);
    Serial.println(counter);
    ++counter;
  }
  if (WiFi.status() == WL_CONNECTED)
    {ok = true;
    Serial.println("--Wifi connected :)--");
    Serial.println(WiFi.localIP());
    }
  else
   {Serial.println("--No Wifi :( --");}
  return ok;
}
bool check_Wifi()
{int counter = 0;
 bool ok = false;
 if (WiFi.status() != WL_CONNECTED) 
  { stop_Wifi();//to be sure first off before restart
    //restart
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println("Reconnecting to WiFi...");
    while ((WiFi.status() != WL_CONNECTED) && (counter <100)) 
      { Serial.println(counter);
        delay(200);
        ++counter;
      }
  }
  if (WiFi.status() == WL_CONNECTED) 
     {ok = true;
     Serial.println("--Wifi connected :)--");
     Serial.println(WiFi.localIP());
     }
  else
   {Serial.println("--No Wifi :( --");}
  return ok;
}
void stop_Wifi()
{
  WiFi.disconnect(true);
  delay(10);
  WiFi.mode(WIFI_OFF);
  delay(10);
}
