/*=====================================================================
 * Open Vega+ Emulator. Handheld ESP32 based hardware with
 * ZX Spectrum emulation
 *
 * (C) 2019 Alvaro Alea Fernandez
 *
 * This program is free software; redistributable under the terms of
 * the GNU General Public License Version 2
 *
 * Based on the Aspectrum emulator Copyright (c) 2000 Santiago Romero Iglesias
 * Use Adafruit's IL9341 and GFX libraries.
 * Compile as ESP32 Wrover Module
 *======================================================================
 */
#include <esp_err.h>
#include <map>
#include <vector>
#include <sstream>
#include "SPI.h"
#include "AudioOutput/I2SOutput.h"
#include "AudioOutput/PDMOutput.h"
#include "AudioOutput/DACOutput.h"
#include "AudioOutput/BuzzerOutput.h"
#include "Emulator/snaps.h"
#include "Emulator/spectrum.h"
#include "Files/Files.h"
#include "Screens/NavigationStack.h"
#include "Screens/MainMenuScreen.h"
#include "Input/SerialKeyboard.h"
#include "Input/Nunchuck.h"
#include "TFT/TFTDisplay.h"
#include "TFT/TFTeSPIWrapper.h"
#include "TFT/ST7789.h"
#ifdef TOUCH_KEYBOARD
#include "Input/TouchKeyboard.h"
#endif
#ifdef TOUCH_KEYBOARD_V2
#include "Input/TouchKeyboardV2.h"
#endif

const char *MOUNT_POINT = "/fs";

void setup(void)
{
  Serial.begin(115200);
  // for(int i = 0; i<5; i++)
  // {
  //   Serial.print(".");
  //   delay(1000);
  // }
  Serial.println("Starting up");
  // navigation stack
  NavigationStack *navigationStack = new NavigationStack();
  // Audio output
#ifdef SPK_MODE
  pinMode(SPK_MODE, OUTPUT);
  digitalWrite(SPK_MODE, HIGH);
#endif
#ifdef USE_DAC_AUDIO
  AudioOutput *audioOutput = new DACOutput(I2S_NUM_0);
#endif
#ifdef BUZZER_GPIO_NUM
  AudioOutput *audioOutput = new BuzzerOutput(BUZZER_GPIO_NUM);
#endif
#ifdef PDM_GPIO_NUM
  // i2s speaker pins
  i2s_pin_config_t i2s_speaker_pins = {
      .bck_io_num = I2S_PIN_NO_CHANGE,
      .ws_io_num = GPIO_NUM_0,
      .data_out_num = BUZZER_GPIO_NUM,
      .data_in_num = I2S_PIN_NO_CHANGE};
  AudioOutput *audioOutput = new PDMOutput(I2S_NUM_0, i2s_speaker_pins);
#endif
#ifdef I2S_SPEAKER_SERIAL_CLOCK
#ifdef SPK_MODE
  pinMode(SPK_MODE, OUTPUT);
  digitalWrite(SPK_MODE, HIGH);
#endif
  // i2s speaker pins
  i2s_pin_config_t i2s_speaker_pins = {
      .bck_io_num = I2S_SPEAKER_SERIAL_CLOCK,
      .ws_io_num = I2S_SPEAKER_LEFT_RIGHT_CLOCK,
      .data_out_num = I2S_SPEAKER_SERIAL_DATA,
      .data_in_num = I2S_PIN_NO_CHANGE};
  AudioOutput *audioOutput = new I2SOutput(I2S_NUM_1, i2s_speaker_pins);
#endif
#ifdef TOUCH_KEYBOARD
  TouchKeyboard *touchKeyboard = new TouchKeyboard(
      [&](SpecKeys key, bool down)
      {
        {
          navigationStack->updatekey(key, down);
        }
      });
  touchKeyboard->calibrate();
  touchKeyboard->start();
#endif
#ifdef TOUCH_KEYBOARD_V2
  TouchKeyboardV2 *touchKeyboard = new TouchKeyboardV2(
      [&](SpecKeys key, bool down) 
      { navigationStack->updatekey(key, down); },
      [&](SpecKeys key)
      { navigationStack->pressKey(key); });
  touchKeyboard->start();
#endif
  audioOutput->start(15625);
  TFTDisplay *tft = new ST7789(TFT_MOSI, TFT_SCLK, TFT_CS, TFT_DC, TFT_RST, TFT_BL, 320, 240);
  // tft = new TFTeSPIWrapper();
  // Files
#ifdef USE_SDCARD
#ifdef SD_CARD_PWR
  if (SD_CARD_PWR != GPIO_NUM_NC)
  {
    pinMode(SD_CARD_PWR, OUTPUT);
    digitalWrite(SD_CARD_PWR, SD_CARD_PWR_ON);
  }
#endif
#ifdef USE_SDIO
  SDCard *fileSystem = new SDCard(MOUNT_POINT, SD_CARD_CLK, SD_CARD_CMD, SD_CARD_D0, SD_CARD_D1, SD_CARD_D2, SD_CARD_D3);
  Files<SDCard> *files = new Files<SDCard>(fileSystem);
#else
  SDCard *fileSystem = new SDCard(MOUNT_POINT, SD_CARD_MISO, SD_CARD_MOSI, SD_CARD_CLK, SD_CARD_CS);
  Files<SDCard> *files = new Files<SDCard>(fileSystem);
#endif
#else
  Flash *fileSystem = new Flash(MOUNT_POINT);
  Files<Flash> *files = new Files<Flash>(fileSystem);
#endif
  // create the directory structure
  files->createDirectory("/snapshots");
  MainMenuScreen menuPicker(*tft, audioOutput, files);
  navigationStack->push(&menuPicker);
  // start off the keyboard and feed keys into the active scene
  SerialKeyboard *keyboard = new SerialKeyboard([&](SpecKeys key, bool down)
                                                { navigationStack->updatekey(key, down); });

// start up the nunchuk controller and feed events into the active screen
#ifdef NUNCHUK_CLOCK
  Nunchuck *nunchuck = new Nunchuck([&](SpecKeys key, bool down)
                                    { navigationStack->updatekey(key, down); },
                                    [&](SpecKeys key)
                                    { navigationStack->pressKey(key); },
                                    NUNCHUK_CLOCK, NUNCHUK_DATA);
#endif
  Serial.println("Running on core: " + String(xPortGetCoreID()));
  // just keep running
  while (true)
  {
    vTaskDelay(10000);
  }
}

unsigned long frame_millis;
void loop()
{
}
