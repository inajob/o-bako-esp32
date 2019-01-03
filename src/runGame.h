#include <Arduino.h>

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <FS.h>
#ifdef M5STACK
#include "M5Stack.h"
#else
#include <TFT_eSPI.h>
#endif


#include "SPIFFS.h"
#include "baseGame.h"
#include "Tunes.h"


extern "C"{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#ifndef RUN_GAME_H
#define RUN_GAME_H

#define MAX_CHAR 256

struct LoadF{
  File f;
  char buf[MAX_CHAR];
};

inline uint16_t rgb24to16(uint8_t r, uint8_t g, uint8_t b) {
  uint16_t tmp = ((r>>3) << 11) | ((g>>2) << 5) | (b>>3);
  return tmp; //(tmp >> 8) | (tmp << 8);
}

class RunGame: public BaseGame
{
  public:
    enum WifiPhase{
      NONE,
      SELECT,
      SHOW,
      RUN
    };


    lua_State* L;
    byte col[3] = {0,0,0};
    int buttonState[7];
    uint8_t surface[128][128]; //16.3kb
    uint16_t palette[256];
    bool wifiDebugRequest = false;
    bool wifiDebugSelf = false;
    WifiPhase wifiMode = NONE;
    int modeSelect = 0;
    bool exitRequest = false;
    bool runError = false;
    String errorString;

    int loadSurface(File* fp, uint8_t* buf);
    static int l_tone(lua_State* L);
    static int l_spr(lua_State* L);
    static int l_pset(lua_State* L);
    static int l_pget(lua_State* L);
    static int l_color(lua_State* L);
    static int l_text(lua_State* L);
    static int l_drawrect(lua_State* L);
    static int l_fillrect(lua_State* L);
    static int l_fillcircle(lua_State* L);
    static int l_btn(lua_State* L);
    static int l_getip(lua_State* L);
    static int l_iswifidebug(lua_State* L);
    static int l_wifiserve(lua_State* L);
    static int l_run(lua_State* L);
    static int l_list(lua_State* L);
    static int l_require(lua_State* L);
    static int l_httpsget(lua_State* L);
    static int l_httpsgetfile(lua_State* L);
    static int l_savebmp(lua_State* L);
    static int l_reboot(lua_State* L);
    String getBitmapName(String s);
    void init();
    int run(int remainTime);
    void pause();
    void resume();
};

#endif
