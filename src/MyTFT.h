class MyTFT_eSPI : public TFT_eSPI {
  public:
    MyTFT_eSPI(int16_t _W = TFT_WIDTH, int16_t _H= TFT_HEIGHT): TFT_eSPI(_W, _H){
    };
    void drawObako(uint16_t* data){
      //pushImage(0,0,128,128,data);
  int32_t x = (320-256)/2;
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
    void myDrawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color){
      drawFastHLine(x, y, w, color);
      drawFastHLine(x, y + h - 1, w, color);
      // Avoid drawing corner pixels twice
      drawFastVLine(x, y+1, h-2, color);
      drawFastVLine(x + w - 1, y+1, h-2, color);
    }
    void drawObako(){
      _mytft->drawObako(_img);
    }
  private:
    MyTFT_eSPI* _mytft;
};


