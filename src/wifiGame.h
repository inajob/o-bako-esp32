#include <Arduino.h>
#include <WiFi.h>
#include <FS.h>

#ifdef M5STACK
#include "M5Stack.h"
#else
#include <TFT_eSPI.h>
#endif

#include "SPIFFS.h"
#include "baseGame.h"
#include "Tunes.h"


#ifndef WIFI_GAME_H
#define WIFI_GAME_H

#define MAX_CHAR 256

class WifiGame
{
  public:
    enum HTTPPhase{
      INIT,
      METHOD,
      HEADER,
      BODY
    };
    enum HTTPEncodePhase{
      RAW,
      PERCENT,
      PERCENT1
    };


    WiFiServer server;
    WifiGame():server(80){
    }
    int buttonState[7];
    void init(bool);
    int initSTA();
    String randomString(String prefix, int n);
    void initAP();
    int run(int remainTime);
    void pause();
    void resume();
    void assignSetting(String* key, String* value, String* ssid, String* password);
    int c2hex(char c);
    void getHandler(WiFiClient *c, String path);
    void postHandler(WiFiClient *c, String path, String body);
};

#endif
