#pragma once
#include "Screen.h"


class uploadGameScreen : public Screen
{
private:

void setupStorage();

public:
  uploadGameScreen(TFTDisplay &tft,AudioOutput *audioOutput) ;
  
  void pressKey(SpecKeys key) ;
  void didAppear() ;
  void willDisappear();
};