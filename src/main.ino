#include <FS.h>
#include <SPI.h>

#ifdef M5STACK
#include "M5Stack.h"
TFT_eSprite tft = TFT_eSprite(&M5.Lcd);
#else
#include <TFT_eSPI.h>
TFT_eSPI screen = TFT_eSPI();
TFT_eSprite tft = TFT_eSprite(&screen);
#endif

#include "Tunes.h"
#include "runGame.h"
#include "wifiGame.h"
BaseGame* game;
String fileName = "/init/main.lua";
WifiGame* wifiGame = NULL;
Tunes tunes;

void setup(){
  Serial.begin(115200);

#ifdef M5STACK
  M5.begin();
#else
  screen.init();
  screen.setRotation(0);
#endif

  tft.createSprite(128, 128);

  tft.fillScreen(TFT_BLACK);

  tft.setCursor(3, 6);
  tft.setTextColor(TFT_WHITE, TFT_BLUE);
  tft.pushSprite(0, 0);


#ifdef M5STACK
#else
  pinMode(39, INPUT_PULLUP);
  pinMode(23, INPUT_PULLUP);
  pinMode(34, INPUT_PULLUP);
  pinMode(35, INPUT_PULLUP);

  pinMode(17, INPUT_PULLUP);
  pinMode(16, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
#endif

  game = new RunGame();
  game->init();

  tunes.init();
}

uint32_t preTime;

void startWifiDebug(bool isSelf){
  tunes.pause();
  wifiGame = new WifiGame();
  wifiGame->init(isSelf);
  tunes.resume();
}
void setFileName(String s){
  fileName = s;
}
bool isWifiDebug(){
  return wifiGame != NULL;
}
void reboot(){
  wifiGame->pause();
  ESP.restart();
}

void loop(){
  uint32_t now = millis();
  uint32_t remainTime= (now - preTime);
  preTime = now;

  if(wifiGame){ // debug mode
    int r = wifiGame->run(remainTime);
    if(r != 0){
      tunes.pause();
      game->pause();
      free(game);
      game = new RunGame();
      game->init();
      tunes.resume();
    }
  }
  tunes.run();

  int mode = game->run(remainTime);

  if(mode != 0){
    tunes.pause();
    game->pause();
    free(game);
    game = new RunGame();
    game->init();
    tunes.resume();
  }

#ifdef M5STACK
  M5.Lcd.drawRect((320 - 128)/2 - 1,(240 - 128)/2 - 1,128 + 2,128 + 2, WHITE);
  tft.pushSprite((320 - 128)/2,(240 - 128)/2);
  M5.update();
#else
  tft.pushSprite(0, 0);
#endif
}

