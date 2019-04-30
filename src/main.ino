#include <FS.h>
#include <SPI.h>

#ifdef M5STACK
#include "M5Stack.h"
#include "M5StackUpdater.h"
#include "MyTFT.h"

MyTFT_eSPI screen = MyTFT_eSPI();
MyTFT_eSprite tft = MyTFT_eSprite(&screen);
#else
#include <TFT_eSPI.h>
#include "MyTFT.h"
//MyTFT_eSPI screen = MyTFT_eSPI();
//MyTFT_eSprite tft = MyTFT_eSprite(&screen);
TFT_eSPI screen = TFT_eSPI();
TFT_eSprite tft = TFT_eSprite(&screen);

#endif

#include "Tunes.h"
#include "runLuaGame.h"
#include "wifiGame.h"
BaseGame* game;
String fileName = "/init/main.lua";
WifiGame* wifiGame = NULL;
Tunes tunes;

void startWifiDebug(bool isSelf){
  tunes.pause();
  wifiGame = new WifiGame();
  wifiGame->init(isSelf);
  tunes.resume();
}

void setup(){
  Serial.begin(115200);

#ifdef M5STACK

 pinMode(BUTTON_A_PIN, INPUT_PULLUP);
 if(digitalRead(BUTTON_A_PIN) == 0) {
    M5.begin();
    Wire.begin();
    Serial.println("Will Load menu binary");
    updateFromFS(SD);
    ESP.restart();
  }

 /*
 // my shield
 pinMode(3, INPUT_PULLUP);
 pinMode(1, INPUT_PULLUP);
 pinMode(16, INPUT_PULLUP);
 pinMode(17, INPUT_PULLUP);
 pinMode(35, INPUT_PULLUP);
 pinMode(36, INPUT_PULLUP);
 */

 M5.begin(false,false,false); // LCD disabled
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
  screen.init();
  screen.setRotation(1);
  screen.fillScreen(TFT_BLACK);
#else
  screen.init();
  screen.setRotation(0);
#endif

  tft.createSprite(128, 128);
  tft.setTextWrap(false);

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

  game = new RunLuaGame();
  game->init();

  tunes.init();
}

uint32_t preTime;

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
      game = new RunLuaGame();
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
    game = new RunLuaGame();
    game->init();
    tunes.resume();
  }
  tft.setCursor(0,120);
  tft.setTextColor(0xffff);
  tft.print(ESP.getFreeHeap());

#ifdef M5STACK
  tft.drawObako();
  M5.update();
#else
  tft.pushSprite(0, 0);
#endif
}

