#pragma once
#include "driver/gpio.h"
#include "../Emulator/keyboard_defs.h"


#define F1 GPIO_NUM_2
#define F2 GPIO_NUM_4
#define F3 GPIO_NUM_6
#define F4 GPIO_NUM_7
#define F5 GPIO_NUM_3
#define F6 GPIO_NUM_21
#define F7 GPIO_NUM_47

#define C1 GPIO_NUM_5
#define C2 GPIO_NUM_38
#define C3 GPIO_NUM_39
#define C4 GPIO_NUM_40
#define C5 GPIO_NUM_41
#define C6 GPIO_NUM_18
#define C7 GPIO_NUM_42

#define MAXFILAS 7
#define MAXCOLUMNAS 7

static gpio_num_t Filas[MAXFILAS] = { F1, F2, F3, F4, F5 , F6 ,F7 };
static gpio_num_t Columnas[MAXCOLUMNAS] = { C1, C2, C3, C4 ,C5 ,C6 ,C7 };

class GaldeanoKeyboard {
  private:
    using KeyEventType = std::function<void(SpecKeys keyCode, bool isPressed)>;
    KeyEventType m_keyEvent;
  public:
  GaldeanoKeyboard(KeyEventType keyEvent);
  static void keyboardTask(void *pParam);
};