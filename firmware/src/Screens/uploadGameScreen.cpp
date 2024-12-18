#include "Arduino.h"
#include "uploadGameScreen.h"
#include "fonts/GillSans_30_vlw.h"
#include "fonts/GillSans_15_vlw.h"
#include "images/rainbow_image.h"
#include "tusb_msc_storage.h"


uploadGameScreen::uploadGameScreen(TFTDisplay &tft,AudioOutput *audioOutput) : Screen(tft, audioOutput){
  
  }

void uploadGameScreen::pressKey(SpecKeys key) {
  //m_navigationStack->pop();
}

void uploadGameScreen::setupStorage(){
  //usb_msc_init();
  //msc_init();
  //SPIFFS.end();
  setupMSC();
}



void uploadGameScreen::didAppear() {
  m_tft.startWrite();
  m_tft.fillScreen(TFT_BLACK);
  m_tft.loadFont(GillSans_15_vlw);
  m_tft.setTextColor(TFT_WHITE, TFT_BLACK);
  m_tft.drawString("Load games - end press any key", 0, 0);
  m_tft.drawFastHLine(0, 15, m_tft.width() - 1, TFT_WHITE);
  m_tft.loadFont(GillSans_30_vlw);
  m_tft.drawString("LOAD GAMES", 30, 100);
  m_tft.setWindow(m_tft.width() - rainbowImageWidth, m_tft.height() - rainbowImageHeight, m_tft.width() - 1, m_tft.height() - 1);
  m_tft.pushPixels((uint16_t *) rainbowImageData, rainbowImageWidth * rainbowImageHeight);
  m_tft.endWrite();

  setupStorage();
}

void uploadGameScreen::willDisappear() {

}