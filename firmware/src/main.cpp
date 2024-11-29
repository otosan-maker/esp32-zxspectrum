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
#include "Serial.h"
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
#include "TFT/ST7789.h"
#include "TFT/ILI9341.h"
#include "Input/GaldeanoKeyboard.h"
#include "Wire.h"
#include "Input/m5_unit_joystick2.hpp"


const char *MOUNT_POINT = "/fs";

void setup(void)
{
  Serial.begin(115200);
  Serial.println("Arrancamos");
    // Files

  Flash *fileSystem = new Flash(MOUNT_POINT);
  Files<Flash> *files = new Files<Flash>(fileSystem);

  // print out avialable ram
  Serial.printf("Free heap: %d\n", ESP.getFreeHeap());
  Serial.printf("Free PSRAM: %d\n", ESP.getFreePsram());
  
  //vTaskDelay(pdMS_TO_TICKS(10000));

  Serial.println("Starting up");
 
  // Audio output
#ifdef USE_DAC_AUDIO
  AudioOutput *audioOutput = new DACOutput(I2S_NUM_0);
#endif
#ifdef BUZZER_GPIO_NUM
  AudioOutput *audioOutput = new BuzzerOutput(BUZZER_GPIO_NUM);
#endif


  if (audioOutput) {
    audioOutput->start(15625);
  }

  
  TFTDisplay *tft = new ILI9341(TFT_MOSI, TFT_SCLK, TFT_CS, TFT_DC, TFT_RST, TFT_BL, TFT_WIDTH, TFT_HEIGHT);
  
  // create the directory structure
  if (fileSystem->isMounted()){
    Serial.println("Montado el sistema SPFFI");
    if (files->isAvailable()){
      Serial.println("y el file system está disponible");
    }
  }
  files->createDirectory("/snapshots");
  // navigation stack
  NavigationStack *navigationStack = new NavigationStack();
  MainMenuScreen menuPicker(*tft, audioOutput, files);
  navigationStack->push(&menuPicker);
  //menuPicker.run48K();
  // start off the keyboard and feed keys into the active scene
  GaldeanoKeyboard *keyboard = new GaldeanoKeyboard([&](SpecKeys key, bool down)
                                                { navigationStack->updatekey(key, down); if (down) { navigationStack->pressKey(key); } });

// start up the nunchuk controller and feed events into the active screen
#ifdef NUNCHUK_CLOCK
  Nunchuck *nunchuck = new Nunchuck([&](SpecKeys key, bool down)
                                    { navigationStack->updatekey(key, down); },
                                    [&](SpecKeys key)
                                    { navigationStack->pressKey(key); },
                                    NUNCHUK_CLOCK, NUNCHUK_DATA);
#endif

M5UnitJoystick2 *mJoyStick2 = new M5UnitJoystick2([&](SpecKeys key, bool down)
                                    { navigationStack->updatekey(key, down); },
                                    [&](SpecKeys key)
                                    { navigationStack->pressKey(key); });

  Serial.println("Running on core: " + String(xPortGetCoreID()));

  while(true){
    vTaskDelay(10000);
  }
}

unsigned long frame_millis;
void loop()
{
    
}
