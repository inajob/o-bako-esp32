#include <FS.h>
#include <SPI.h>

#ifdef M5STACK
#include "M5Stack.h"

class MyTFT_eSPI : public TFT_eSPI {
  public:
    MyTFT_eSPI(int16_t _W = TFT_WIDTH, int16_t _H= TFT_HEIGHT): TFT_eSPI(_W, _H){
    };
    void drawObako(uint16_t* data){
      //pushImage(0,0,128,128,data);
  int32_t x = 0;
  int32_t y = 0;
  int32_t w = 128; // o-bako resolution
  int32_t h = 128;

  int32_t dx = 0;
  int32_t dy = 0;
  int32_t dw = w*2;
  int32_t dh = 16 + (128-16)*2;

  if (x < 0) { dw += x; dx = -x; x = 0; }
  if (y < 0) { dh += y; dy = -y; y = 0; }

  if ((x + w) > _width ) dw = _width  - x;
  if ((y + h) > _height) dh = _height - y;

  if (dw < 1 || dh < 1) return;

  //spi_begin(); // copy from TFT_eSPI
#if defined (SPI_HAS_TRANSACTION) && defined (SUPPORT_TRANSACTIONS) && !defined(ESP32_PARALLEL)
  if (locked) {locked = false; spi.beginTransaction(SPISettings(SPI_FREQUENCY, MSBFIRST, TFT_SPI_MODE)); CS_L;}
#else
  CS_L;
#endif

  inTransaction = true;

  setWindow(x, y, x + dw - 1, y + dh - 1);

  data += dx + dy * w;
  uint32_t count = 0;
  uint16_t lineBuffer[dw];
  while (dh--)
  {
    if(count < 16 || count%2 ==0){
      for(uint16_t i = 0; i < dw; i ++){
        lineBuffer[i] = data[i/2];
      }
      data += w;
    }
    count ++;
    pushColors(lineBuffer, dw, _swapBytes);
  }

  inTransaction = false;
  //spi_end(); // copy from TFT_eSPI
#if defined (SPI_HAS_TRANSACTION) && defined (SUPPORT_TRANSACTIONS) && !defined(ESP32_PARALLEL)
  if(!inTransaction) {if (!locked) {locked = true; CS_H; spi.endTransaction();}}
#else
  if(!inTransaction) CS_H;
#endif


    }
};
class MyTFT_eSprite : public TFT_eSprite {
  public:
    MyTFT_eSprite(MyTFT_eSPI* tft): TFT_eSprite(tft){
      _mytft = tft;
    }
    void drawObako(){
      _mytft->drawObako(_img);
    }
  private:
    MyTFT_eSPI* _mytft;
};


MyTFT_eSPI screen = MyTFT_eSPI();
MyTFT_eSprite tft = MyTFT_eSprite(&screen);
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
  M5.begin(false); // LCD disabled
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
  screen.init();
  screen.setRotation(1);
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

