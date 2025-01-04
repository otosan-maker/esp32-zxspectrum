#include "Arduino.h"
#include "uploadGameScreen.h"
#include "fonts/GillSans_30_vlw.h"
#include "fonts/GillSans_15_vlw.h"
#include "images/rainbow_image.h"
#include "esp_ota_ops.h"


uploadGameScreen::uploadGameScreen(TFTDisplay &tft,AudioOutput *audioOutput) : Screen(tft, audioOutput){
  
  }

void uploadGameScreen::pressKey(SpecKeys key) {
  //m_navigationStack->pop();
}

void uploadGameScreen::setupStorage(){
  const esp_partition_t *data_partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_0, NULL);
  //const esp_partition_t *data_partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_1, "galdeano");
  if (data_partition==NULL)
    Serial.printf("data partition not found\n");
  esp_err_t err = esp_ota_set_boot_partition(data_partition);
  
  Serial.printf("salida de set boot partiCion %d",err);
  Serial.println("");
  vTaskDelay(pdMS_TO_TICKS(2000));
  esp_restart();

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