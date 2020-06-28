#include "runJsGame.h"
#include "MyTFT.h"
extern MyTFT_eSprite tft;
extern String fileName;
extern void startWifiDebug(bool isSelf);
extern void setFileName(String s);
extern bool isWifiDebug();
extern void reboot();
extern Tunes tunes;

int RunJsGame::loadSurface(File *fp, uint8_t* buf){
  uint8_t c;
  unsigned long offset;
  unsigned long width, height;
  unsigned long biSize;
  uint16_t bitCount;

  Serial.println("pre read");
  fp->read(&c, 1);
  Serial.println("read1");
  if(c != 'B'){
    printf("header error 0");
    Serial.print(c);
    Serial.println("unknown header");
    return -1;
  }
  fp->read(&c, 1);
  Serial.println("read2");
  if(c != 'M'){
    printf("header error 1");
    return -1;
  }
  Serial.println("pre seek");
  fp->seek(4 + 2 + 2, SeekCur); // size, resv1, resv2
  fp->read((uint8_t*)&offset, 4);

  fp->read((uint8_t*)&biSize, 4);
  fp->read((uint8_t*)&width, 4);
  fp->read((uint8_t*)&height, 4);
  fp->seek(2, SeekCur); // skip biPlanes
  fp->read((uint8_t*)&bitCount, 2);


  Serial.println("pre check");
  if(width != 128){
    printf("invalid width:%d\n", width);
    return -1;
  }
  if(height != 128){
    printf("invalid height:%d\n", height);
    return -1;
  }
  if(bitCount != 8){
    printf("invalid bitCount:%x\n", bitCount);
    return -1;
  }

  fp->seek(biSize - (4 + 4 + 4 + 2 + 2), SeekCur);
  uint8_t r, g, b;
  for(unsigned int i = 0; i < 256; i ++){
    fp->read(&b, 1);
    fp->read(&g, 1);
    fp->read(&r, 1);
    fp->seek(1, SeekCur);
    palette[i] = rgb24to16(r, g, b);
    Serial.print("palette");
    Serial.println(i);
    Serial.print(r);
    Serial.print(g);
    Serial.print(b);
  }

  Serial.println("pre seek");
  fp->seek(offset, SeekSet); // go to bmp data section

  for(unsigned int i = 0; i < width * height; i ++){
    uint8_t d;
    fp->read(&d, 1);
    *buf = d;
    buf ++;
  }
  return 0;
}
duk_ret_t RunJsGame::l_tone(duk_context* ctx){
  duk_push_global_object(ctx);          // push global
  duk_get_prop_string(ctx, -1, "obako");// push obako
  RunJsGame* self = (RunJsGame*)duk_get_pointer(ctx,-1);
  duk_pop_2(ctx); // obako, global

  int n = duk_get_int(ctx, 1);
  int f = duk_get_int(ctx, 2);

  portENTER_CRITICAL(&Tunes::timerMux);
  Tunes::d[n] = (uint16_t)(3.2768*f);
  portEXIT_CRITICAL(&Tunes::timerMux);
  return 0;
}

int RunJsGame::l_spr(duk_context* ctx){
  duk_push_global_object(ctx);          // push global
  duk_get_prop_string(ctx, -1, "obako");// push obako
  RunJsGame* self = (RunJsGame*)duk_get_pointer(ctx,-1);
  duk_pop_2(ctx); // obako, global

  int x = duk_get_int(ctx, 0);
  int y = duk_get_int(ctx, 1);
  int w = duk_get_int(ctx, 2);
  int h = duk_get_int(ctx, 3);
  int sx = duk_get_int(ctx, 4);
  int sy = duk_get_int(ctx, 5);

  int sw = w, sh = h;
  if(duk_get_top(ctx) == 8){ // todo: is this bug?
    sw = duk_get_int(ctx, 6);
    sh = (duk_get_int, 7);
  }
  uint8_t index;

  int xscale = w/sw;
  int yscale = h/sh;

  if(xscale == 1 && yscale == 1){
    for(uint8_t i = 0; i < sh; i ++){
      for(uint8_t j = 0; j < sw; j ++){
        index = self->surface[127 - (sy + i)][sx + j];
        if(index != 0){
          tft.drawPixel(x + j, y + i, self->palette[index]);
        }
      }
    }
  }else if(xscale > 1 && yscale > 1){
    for(uint8_t i = 0; i < sh; i ++){
      for(uint8_t j = 0; j < sw; j ++){
        index = self->surface[127 - (sy + i)][sx + j];
        if(index != 0){
          tft.fillRect(x + j*xscale, y + i*yscale, xscale, yscale, self->palette[index]);
        }
      }
    }
  }else{
    // not support small image
  }
  return 0;
}
/*
int RunJsGame::l_pget(duk_context* ctx){
  RunJsGame* self = (RunJsGame*)lua_touserdata(L, lua_upvalueindex(1));
  int x = lua_tointeger(L, 1);
  int y = lua_tointeger(L, 2);

  uint16_t c = tft.readPixel(x, y);

  uint8_t index = 0;
  for(unsigned int pi = 0; pi < 256; pi ++){
    if(self->palette[pi] == c){
      index = pi;
      break;
    }
  }
  uint8_t r = ((c >> 11) << 3); // 5bit
  uint8_t g = (((c >> 5) & 0b111111) << 2); // 6bit
  uint8_t b = ((c & 0b11111) << 3);       // 5bit

  lua_pushinteger(L, (lua_Integer)r);
  lua_pushinteger(L, (lua_Integer)g);
  lua_pushinteger(L, (lua_Integer)b);
  lua_pushinteger(L, (lua_Integer)index);
  return 4;
}
*/
duk_ret_t RunJsGame::l_color(duk_context* ctx){
  duk_push_global_object(ctx);          // push global
  duk_get_prop_string(ctx, -1, "obako");// push obako
  RunJsGame* self = (RunJsGame*)duk_get_pointer(ctx,-1);
  duk_pop_2(ctx); // obako, global

  int r,g,b;
  if(duk_get_top(ctx) == 3){ // from rgb
    r = duk_get_int(ctx, 0);
    g = duk_get_int(ctx, 1);
    b = duk_get_int(ctx, 2);

    self->col[0] = r;
    self->col[1] = g;
    self->col[2] = b;
  }else{ // from palette
    r = duk_get_int(ctx, 0);
    self->col[0] = ((self->palette[r] >> 11) << 3); // 5bit
    self->col[1] = (((self->palette[r] >> 5) & 0b111111) << 2); // 6bit
    self->col[2] = ((self->palette[r] & 0b11111) << 3);       // 5bit
  }

  return 0;
}
duk_ret_t RunJsGame::l_pset(duk_context* ctx){
  duk_push_global_object(ctx);          // push global
  duk_get_prop_string(ctx, -1, "obako");// push obako
  RunJsGame* self = (RunJsGame*)duk_get_pointer(ctx,-1);
  duk_pop_2(ctx); // obako, global

  int x = duk_get_int(ctx, 0);
  int y = duk_get_int(ctx, 1);

  tft.drawPixel(x, y, rgb24to16(self->col[0], self->col[1], self->col[2]));
  return 0;
}

duk_ret_t RunJsGame::l_text(duk_context* ctx){

  duk_push_global_object(ctx);
  duk_get_prop_string(ctx, -1, "obako");
  RunJsGame* self = (RunJsGame*)duk_get_pointer(ctx,-1);
  duk_pop_2(ctx); // pointer, obako, global

  duk_size_t len;
  const char* text = duk_get_lstring(ctx, 0, &len);
  int x = duk_get_int(ctx, 1);
  int y = duk_get_int(ctx, 2);

  tft.setCursor(x,y);
  tft.setTextColor(rgb24to16(self->col[0], self->col[1], self->col[2]));
  tft.print(text);
  return 0;
}
int RunJsGame::l_drawrect(duk_context* ctx){
  duk_push_global_object(ctx);
  duk_get_prop_string(ctx, -1, "obako");
  RunJsGame* self = (RunJsGame*)duk_get_pointer(ctx,-1);
  duk_pop_2(ctx); // pointer, obako, global

  int x = duk_get_int(ctx, 0);
  int y = duk_get_int(ctx, 1);
  int w = duk_get_int(ctx, 2);
  int h = duk_get_int(ctx, 3);

  tft.myDrawRect(x, y, w, h, rgb24to16(self->col[0], self->col[1], self->col[2]));
  return 0;
}
int RunJsGame::l_fillrect(duk_context* ctx){
  duk_push_global_object(ctx);
  duk_get_prop_string(ctx, -1, "obako");
  RunJsGame* self = (RunJsGame*)duk_get_pointer(ctx,-1);
  duk_pop_2(ctx); // pointer, obako, global

  int x = duk_get_int(ctx, 0);
  int y = duk_get_int(ctx, 1);
  int w = duk_get_int(ctx, 2);
  int h = duk_get_int(ctx, 3);

  tft.fillRect(x, y, w, h, rgb24to16(self->col[0], self->col[1], self->col[2]));
  return 0;
}
/*
int RunJsGame::l_fillcircle(duk_context* ctx){
  RunJsGame* self = (RunJsGame*)lua_touserdata(L, lua_upvalueindex(1));
  int x = lua_tointeger(L, 1);
  int y = lua_tointeger(L, 2);
  int r = lua_tointeger(L, 3);

  tft.fillCircle(x, y, r, rgb24to16(self->col[0], self->col[1], self->col[2]));
  return 0;
}
*/

duk_ret_t RunJsGame::l_btn(duk_context* ctx){
  duk_push_global_object(ctx);          // push global
  duk_get_prop_string(ctx, -1, "obako");// push obako
  RunJsGame* self = (RunJsGame*)duk_get_pointer(ctx,-1);
  duk_pop_2(ctx); // obako, global

  int n = duk_get_int(ctx, 0);

  duk_push_int(ctx, (lua_Integer)self->buttonState[n]);
  return 1;
}
/*
int RunJsGame::l_iswifidebug(duk_context* ctx){
  RunJsGame* self = (RunJsGame*)lua_touserdata(L, lua_upvalueindex(1));

  lua_pushboolean(L, isWifiDebug()?1:0);
  return 1;
}
int RunJsGame::l_getip(duk_context* ctx){
  RunJsGame* self = (RunJsGame*)lua_touserdata(L, lua_upvalueindex(1));
  char buf[16];
  IPAddress ip = WiFi.localIP();
  if(self->wifiDebugSelf){
    ip = WiFi.softAPIP();
  }
  sprintf(buf, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3] );

  lua_pushstring(L, buf);
  return 1;
}


int RunJsGame::l_wifiserve(duk_context* ctx){
  RunJsGame* self = (RunJsGame*)lua_touserdata(L, lua_upvalueindex(1));
  self->wifiDebugSelf = false;
  if(lua_gettop(L) == 1){
    const char* text = lua_tostring(L, 1);
    if(strncmp(text, "ap", 3) == 0){
      self->wifiDebugSelf = true;
    }
  }
  self->wifiDebugRequest = true;
  return 0;
}
int RunJsGame::l_run(duk_context* ctx){
  RunJsGame* self = (RunJsGame*)lua_touserdata(L, lua_upvalueindex(1));
  const char* file = lua_tostring(L, 1);

  setFileName(file);
  self->exitRequest = true;
  return 0;
}
int RunJsGame::l_list(duk_context* ctx){
  RunJsGame* self = (RunJsGame*)lua_touserdata(L, lua_upvalueindex(1));
  File f;

  lua_newtable(L);

  File root = SPIFFS.open("/");
  f = root.openNextFile();
  int i = 0;
  while(f){
    Serial.println(f.name());
    lua_pushstring(L, f.name());
    lua_rawseti(L, -2, i);
    f = root.openNextFile();
    i ++;
  }
  return 1;
}
int RunJsGame::l_require(duk_context* ctx){
  bool loadError = false;
  RunJsGame* self = (RunJsGame*)lua_touserdata(L, lua_upvalueindex(1));
  const char* fname = lua_tostring(L, 1);
  File fp = SPIFFS.open(fname, FILE_READ);

  struct LoadF lf;
  lf.f = fp;
  char cFileName[32];
  fileName.toCharArray(cFileName, 32);
  if(lua_load(L, getF, &lf, cFileName, NULL)){
    printf("error? %s\n", lua_tostring(L, -1));
    Serial.printf("error? %s\n", lua_tostring(L, -1));
    //runError = true;
    //errorString = lua_tostring(L, -1);
    loadError = true;
  }
  fp.close();

  if(loadError == false){
    if(lua_pcall(L, 0, 1, 0)){
      Serial.printf("init error? %s\n", lua_tostring(L, -1));
      //runError = true;
      //errorString = lua_tostring(L, -1);
    }
  }

  Serial.println("finish require");


  return 1;
}

int RunJsGame::l_httpsget(duk_context* ctx){
  RunJsGame* self = (RunJsGame*)lua_touserdata(L, lua_upvalueindex(1));
  const char* host = lua_tostring(L, 1);
  const char* path = lua_tostring(L, 2);
  WiFiClientSecure client;
  const int httpsPort = 443;
  Serial.println(host);
  Serial.println(path);
  if(!client.connect(host, httpsPort)){
    // connection failed
    Serial.println("connect failed");
  }
  client.print(String("GET ") + path + " HTTP/1.1\r\n" +
    "Host: " + host + "\r\n" +
    "User-Agent: o-bako\r\n" +
    "Connection: close\r\n\r\n"
  );
  String line;
  while(client.connected()){
    line = client.readStringUntil('\n');
    if(line == "\r"){
      // headers recieved
      Serial.println("headers recieved");
      break;
    }
  }
  line = client.readString();
  int lineLength = line.length();
  const char *lineChar = line.c_str();

  lua_pushstring(L, lineChar);
  Serial.println("done");
  return 1;
}

int RunJsGame::l_httpsgetfile(duk_context* ctx){
  RunJsGame* self = (RunJsGame*)lua_touserdata(L, lua_upvalueindex(1));
  const char* host = lua_tostring(L, 1);
  const char* path = lua_tostring(L, 2);
  const char* filePath = lua_tostring(L, 3);
  WiFiClientSecure client;
  const int httpsPort = 443;
  if(!client.connect(host, httpsPort)){
    // connection failed
    Serial.println("connect failed");
  }
  client.print(String("GET ") + path + " HTTP/1.1\r\n" +
    "Host: " + host + "\r\n" +
    "User-Agent: o-bako\r\n" +
    "Connection: close\r\n\r\n"
  );
  String line;
  int len = 0;
  while(client.connected()){
    line = client.readStringUntil('\n');
    if(line == "\r"){
      // headers recieved
      Serial.println("headers recieved");
      break;
    }
    if(line.startsWith("Content-Length: ")){
      len = line.substring(16).toInt();
    }

  }

  tunes.pause();
  File f = SPIFFS.open(filePath, FILE_WRITE);
  while(client.available() && len > 0){
    char c = client.read();
    f.write(c);
    len --;
  }
  f.close();
  tunes.resume();
  return 0;
}

int RunJsGame::l_savebmp(duk_context* ctx){
  RunJsGame* self = (RunJsGame*)lua_touserdata(L, lua_upvalueindex(1));
  const char* path = lua_tostring(L, 1);
  uint8_t buf[4];
  long* ltmp = (long*) buf;
  uint16_t* tmp2 = (uint16_t*)buf;
  uint8_t tmp1;

  tunes.pause();
  File f = SPIFFS.open(path, FILE_WRITE);
  f.write('B');
  f.write('M');
  *ltmp = 14 + 40 + 4 * 256;
  f.write(buf, 4); // file size
  *tmp2 = 0;
  f.write(buf, 2); // reserved1
  f.write(buf, 2); // reserved2
  *ltmp = 14 + 40 + 4 * 256;
  f.write(buf, 4); // header size

  // BITMAPCOREHEADER
  *ltmp = 40;
  f.write(buf, 4); // bc size
  *ltmp = 128;
  f.write(buf, 4); // width
  f.write(buf, 4); // height
  *tmp2 = 1;
  f.write(buf, 2); // planes
  *tmp2 = 8;
  f.write(buf, 2); // bitcount
  *ltmp = 0;
  f.write(buf,4); // compression
  *ltmp = 0;
  f.write(buf,4); // size image
  *ltmp = 0;
  f.write(buf,4); // horizon dot/m
  *ltmp = 0;
  f.write(buf,4); // vertical dot/m
  *ltmp = 0;
  f.write(buf,4); // cir used
  *ltmp = 0;
  f.write(buf,4); // cir important

  uint8_t r,g,b;
  for(unsigned int i = 0; i < 256; i ++){
    r = ((self->palette[i] >> 11) << 3);
    g = (((self->palette[i] >> 5) & 0b111111) << 2);
    b = ((self->palette[i] & 0b11111) << 3);
    f.write(b);
    f.write(g);
    f.write(r);
    f.write(0); // reserved
  }
  int x = 0,y = 127;
  for(unsigned int i = 0; i < 128*128; i ++){
    uint16_t d = tft.readPixel(x, y);
    uint8_t index = 0;
    for(unsigned int pi = 0; pi < 256; pi ++){
      if(self->palette[pi] == d){
        index = pi;
        break;
      }
    }
    f.write(index);
    x ++;
    if(x == 128){
      x = 0;
      y --;
    }
  }
  f.close();
  tunes.resume();
  return 0;
}

int RunJsGame::l_reboot(duk_context* ctx){
  RunJsGame* self = (RunJsGame*)lua_touserdata(L, lua_upvalueindex(1));

  reboot();
  return 0;
}
int RunJsGame::l_debug(duk_context* ctx){
  RunJsGame* self = (RunJsGame*)lua_touserdata(L, lua_upvalueindex(1));
  const char* text = lua_tostring(L, 1);

  Serial.println(text);
  return 0;
}
*/
String RunJsGame::getBitmapName(String s){
  int p = s.lastIndexOf("/");
  if(p == -1){
    p = 0;
  }
  return s.substring(0, p) + "/sprite.bmp";
}

void RunJsGame::init(){
  this->resume();
}
void RunJsGame::pause(){
  duk_destroy_heap(ctx);
}
void RunJsGame::resume(){
  char buf[MAX_CHAR];
  char str[100];

  Serial.println("resume JS");

  ctx = duk_create_heap_default();
  duk_push_global_object(ctx);

  duk_push_pointer(ctx, this);
  duk_put_prop_string(ctx, -2, "obako");

  duk_idx_t fidx = duk_push_c_function(ctx, RunJsGame::l_tone, 3);
  duk_put_prop_string(ctx, -2, "tone");

  /*
  lua_pushlightuserdata(L, this);
  lua_pushcclosure(L, l_tone, 1);
  lua_setglobal(L, "tone");

  lua_pushlightuserdata(L, this);
  lua_pushcclosure(L, l_pget, 1);
  lua_setglobal(L, "pget");
  */

  fidx = duk_push_c_function(ctx, l_spr, 6);
  duk_put_prop_string(ctx, -2, "spr");

  fidx = duk_push_c_function(ctx, l_pset, 2);
  duk_put_prop_string(ctx, -2, "pset");

  fidx = duk_push_c_function(ctx, l_color, 3);
  duk_put_prop_string(ctx, -2, "color");

  fidx = duk_push_c_function(ctx, l_text, 3);
  duk_put_prop_string(ctx, -2, "text");

  fidx = duk_push_c_function(ctx, l_drawrect, 4);
  duk_put_prop_string(ctx, -2, "drawrect");

  fidx = duk_push_c_function(ctx, l_fillrect, 4);
  duk_put_prop_string(ctx, -2, "fillrect");

  /*
  lua_pushlightuserdata(L, this);
  lua_pushcclosure(L, l_fillcircle, 1);
  lua_setglobal(L, "fillcircle");
  */

  fidx = duk_push_c_function(ctx, l_btn, 1);
  duk_put_prop_string(ctx, -2, "btn");

  /*
  lua_pushlightuserdata(L, this);
  lua_pushcclosure(L, l_getip, 1);
  lua_setglobal(L, "getip");

  lua_pushlightuserdata(L, this);
  lua_pushcclosure(L, l_iswifidebug, 1);
  lua_setglobal(L, "iswifidebug");

  lua_pushlightuserdata(L, this);
  lua_pushcclosure(L, l_wifiserve, 1);
  lua_setglobal(L, "wifiserve");

  lua_pushlightuserdata(L, this);
  lua_pushcclosure(L, l_run, 1);
  lua_setglobal(L, "run");

  lua_pushlightuserdata(L, this);
  lua_pushcclosure(L, l_list, 1);
  lua_setglobal(L, "list");

  lua_pushlightuserdata(L, this);
  lua_pushcclosure(L, l_reboot, 1);
  lua_setglobal(L, "reboot");

  lua_pushlightuserdata(L, this);
  lua_pushcclosure(L, l_debug, 1);
  lua_setglobal(L, "debug");

  lua_pushlightuserdata(L, this);
  lua_pushcclosure(L, l_require, 1);
  lua_setglobal(L, "require");

  lua_pushlightuserdata(L, this);
  lua_pushcclosure(L, l_httpsget, 1);
  lua_setglobal(L, "httpsget");

  lua_pushlightuserdata(L, this);
  lua_pushcclosure(L, l_httpsgetfile, 1);
  lua_setglobal(L, "httpsgetfile");

  lua_pushlightuserdata(L, this);
  lua_pushcclosure(L, l_savebmp, 1);
  lua_setglobal(L, "savebmp");
  */

  duk_pop(ctx); // pop global

  SPIFFS.begin(true);
  Serial.println("SPIFFS begin");

  if(SPIFFS.exists(getBitmapName(fileName))){
    File bmpFile = SPIFFS.open(getBitmapName(fileName) , FILE_READ);
    Serial.println("bitmap load begin");
    if(loadSurface(&bmpFile, (uint8_t*)surface) != 0){
      printf("bitmap load error");
      Serial.println("bitmap load error");
      runError = true;
      errorString = "bitmap load error fileName=" + getBitmapName(fileName);
    }
    Serial.println("bitmap load end");
    bmpFile.close();
  }
  Serial.println("loaded bitmap");

  File fp = SPIFFS.open(fileName, FILE_READ);
  Serial.println("open file");

  tft.fillScreen(TFT_BLACK);

  while(fp.available()){
    fp.readStringUntil('\n').toCharArray(buf, MAX_CHAR);
    Serial.println(buf);
    duk_push_string(ctx, buf);
  }
  duk_push_string(ctx, "\n");
  duk_insert(ctx, 0);
  Serial.println("pre join");
  duk_join(ctx, duk_get_top(ctx) - 1);
  Serial.println("join file");
  //Serial.println(duk_safe_to_string(ctx, -1));
  if(duk_peval(ctx) != 0){
    Serial.println("eval failed");
    Serial.println(duk_safe_to_string(ctx, -1));
  }
  Serial.println("eval success");
  duk_size_t len;
  const char* result = duk_get_lstring(ctx, 1, &len);
  Serial.println(result);

  duk_pop(ctx); // remove result

  Serial.println("finish");

  for(int i = 0; i < 7; i ++){
    buttonState[i] = 0;
  }

  tft.pushSprite(0, 0);
}
int RunJsGame::run(int remainTime){
  char str[100];
  char key;

  if(wifiDebugRequest){
    startWifiDebug(wifiDebugSelf);
    wifiMode = SHOW;
    wifiDebugRequest = false;
  }
  if(exitRequest){
    exitRequest = false;
    return 1; // exit
  }

  bool btn[7];

#ifdef M5STACK
  btn[0] = 1; // left
  btn[1] = !M5.BtnA.isPressed(); // right
  btn[2] = 1; // up
  btn[3] = !M5.BtnC.isPressed(); // down
  btn[4] = !M5.BtnB.isPressed(); // C
  btn[5] = 1; // A
  btn[6] = 1; // B
  /*
  btn[0] = digitalRead(1); // left
  btn[1] = digitalRead(17)&(!M5.BtnA.isPressed()); // right
  btn[2] = digitalRead(16); // up
  btn[3] = digitalRead(3)&(!M5.BtnC.isPressed()); // down
  btn[4] = !M5.BtnB.isPressed(); // C
  btn[5] = digitalRead(35); // A
  btn[6] = digitalRead(36); // B
  */

#else
  btn[0] = digitalRead(39);
  btn[1] = digitalRead(23);
  btn[2] = digitalRead(34);
  btn[3] = digitalRead(35);
  btn[4] = digitalRead(17);
  btn[5] = digitalRead(16);
  btn[6] = digitalRead(4);
#endif

  for(int i = 0; i < 7; i ++){
    if(btn[i]){
      buttonState[i] = 0;
    }else{
      buttonState[i] ++;
    }
  }

  if(wifiMode == NONE || wifiMode == RUN){
    if(runError){
      tft.setTextSize(1);
      tft.setTextColor(TFT_WHITE, TFT_BLUE);
      tft.setCursor(0, 0);
      tft.setTextWrap(true);
      tft.print(errorString);
      tft.setTextWrap(false);

      if(buttonState[5] == 10){ // reload
        return 1;
      }
      if(buttonState[6] == 10){ // reload
        setFileName("/init/main.lua"); // TODO: lua?
        return 1;
      }
      if(buttonState[4] == 10){ // reload
        wifiMode = SELECT;
      }
    }else{
      if(duk_peval_string(ctx, "loop()") != 0){
        Serial.print("loop() failed");
        Serial.println(duk_safe_to_string(ctx, -1));
      }
      //  Serial.printf("run error? %s\n", lua_tostring(L, -1));
      //  runError = true;
      //  errorString = lua_tostring(L, -1);
      duk_pop(ctx);

      if(buttonState[4] == 100){ // menu
        wifiMode = SELECT;
      }
    }
  }else if(wifiMode == SELECT){
    tft.fillRect(0, 0, 128, 64, rgb24to16(64,64,64));
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLUE);
    tft.setCursor(0, 0);
    tft.print("pause");
    tft.setCursor(0, 8);
    tft.print("  WiFi AP");
    tft.setCursor(0, 16);
    tft.print("  WiFi STA");
    tft.setCursor(0, 24);
    tft.print("  load /init/main.lua");
    tft.setCursor(0, (modeSelect + 1) * 8);
    tft.print(">");

    if(buttonState[2] == 1 && modeSelect > 0){
      modeSelect -= 1;
    }
    if(buttonState[5] == 1 || buttonState[3] == 1){
      modeSelect += 1;
      modeSelect = modeSelect%3;
    }
    if(buttonState[4] == 1){
      switch(modeSelect){
        case 0:
          wifiDebugRequest = true;
          wifiDebugSelf = true;
        break;
        case 1:
          wifiDebugRequest = true;
          wifiDebugSelf = false;
        break;
        case 2:
          setFileName("/init/main.lua"); // TODO: lua?
          return 1;
      }
    }
  }else if(wifiMode == SHOW){
    if(buttonState[4] == 10){ // reload
      wifiMode = RUN;
    }
  }

  // show FPS
  sprintf(str, "%02dFPS", 1000/remainTime); // FPS

  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, TFT_BLUE);
  tft.setCursor(90, 127 - 16);
  tft.print(str);

  sprintf(str, "%02dms", remainTime); // ms
  tft.setCursor(90, 127 - 8);
  tft.print(str);

  int wait = 1000/30 - remainTime;
  if(wait > 0){
    delay(wait);
  }
  return 0;
}

