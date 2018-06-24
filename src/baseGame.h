#ifndef BASE_GAME_H
#define BASE_GAME_H

class BaseGame
{
  public:
  virtual void init(){};
  virtual int run(int remainTime){return 0;};
  virtual void pause(){};
  virtual void resume(){};
};

#endif
