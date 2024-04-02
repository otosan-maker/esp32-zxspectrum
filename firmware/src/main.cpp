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
#include <TFT_eSPI.h>
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
#include "Screens/PickerScreen.h"
#include "Screens/EmulatorScreen.h"
#include "Input/SerialKeyboard.h"
#include "Input/Nunchuck.h"

TFT_eSPI *tft = nullptr;
AudioOutput *audioOutput = nullptr;
PickerScreen<FileInfoPtr> *filePickerScreen = nullptr;
PickerScreen<FileLetterCountPtr> *alphabetPicker = nullptr;
EmulatorScreen *emulatorScreen = nullptr;
Screen *activeScreen = nullptr;
Files *files = nullptr;
SerialKeyboard *keyboard = nullptr;
Nunchuck *nunchuck = nullptr;

void setup(void)
{
  Serial.begin(115200);
  // Audio output
#ifdef SPK_MODE
  pinMode(SPK_MODE, OUTPUT);
  digitalWrite(SPK_MODE, HIGH);
#endif
#ifdef USE_DAC_AUDIO
  audioOutput = new DACOutput(I2S_NUM_0);
#endif
#ifdef BUZZER_GPIO_NUM
  audioOutput = new BuzzerOutput(BUZZER_GPIO_NUM);
#endif
#ifdef PDM_GPIO_NUM
  // i2s speaker pins
  i2s_pin_config_t i2s_speaker_pins = {
      .bck_io_num = I2S_PIN_NO_CHANGE,
      .ws_io_num = GPIO_NUM_0,
      .data_out_num = PDM_GPIO_NUM,
      .data_in_num = I2S_PIN_NO_CHANGE};
  audioOutput = new PDMOutput(I2S_NUM_0, i2s_speaker_pins);
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
  audioOutput = new I2SOutput(I2S_NUM_1, i2s_speaker_pins);
#endif
  audioOutput->start(15625);
  // Display
#ifdef TFT_POWER
  // turn on the TFT
  pinMode(TFT_POWER, OUTPUT);
  digitalWrite(TFT_POWER, LOW);
#endif
  tft = new TFT_eSPI();
  tft->begin();
#ifdef USE_DMA
  tft->initDMA(); // Initialise the DMA engine
#endif
  tft->setRotation(3);
  tft->fillScreen(TFT_BLACK);
  // Files
  files = new Files();
  // wire everythign up
  emulatorScreen = new EmulatorScreen(*tft, audioOutput);
  alphabetPicker = new PickerScreen<FileLetterCountPtr>(*tft, audioOutput, [&](FileLetterCountPtr entry, int index) {
    // a letter was picked - show the files for that letter
    Serial.printf("Picked letter: %s\n", entry->getLetter().c_str()), 
    filePickerScreen->setItems(files->getFileStartingWithPrefix("/", entry->getLetter().c_str(), ".sna"));
    activeScreen = filePickerScreen;
  }, [&]() {
    // nothing to do here - we're at the top level
  });
  filePickerScreen = new PickerScreen<FileInfoPtr>(*tft, audioOutput, [&](FileInfoPtr file, int index) {
    // a file was picked - load it into the emulator
    Serial.printf("Loading snapshot: %s\n", file->getPath().c_str());
    emulatorScreen->run(file->getPath());
    activeScreen = emulatorScreen;
  }, [&]() {
    // go back to the alphabet picker
    activeScreen = alphabetPicker;
    activeScreen->didAppear();
  });
  // feed in the alphabetically grouped files to the alphabet picker
  alphabetPicker->setItems(files->getFileLetters("/", ".sna"));
  // start off the keyboard and feed keys into the active scene
  keyboard = new SerialKeyboard([&](int key, bool down) {
    if (activeScreen)
    {
      activeScreen->updatekey(key, down);
    }
  });

  // start up the nunchuk controller and feed events into the active screen
  #ifdef NUNCHUK_CLOCK
  nunchuck = new Nunchuck([&](int key, bool down) {
    if (activeScreen)
    {
      activeScreen->updatekey(key, down);
    }
  }, NUNCHUK_CLOCK, NUNCHUK_DATA);
  #endif
  // start off on the file picker screen
  activeScreen = alphabetPicker;
}

unsigned long frame_millis;
void loop()
{
  auto ms = millis() - frame_millis;
  if (ms > 1000)
  {
    if (activeScreen == emulatorScreen)
    {
      float cycles = emulatorScreen->cycleCount / (ms * 1000.0);
      float fps = emulatorScreen->frameCount / (ms / 1000.0);
      Serial.printf("Executed at %.3FMHz cycles, frame rate=%.2f\n", cycles, fps);
      emulatorScreen->frameCount = 0;
      emulatorScreen->cycleCount = 0;
      frame_millis = millis();
    }
  }
}