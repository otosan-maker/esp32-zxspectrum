/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "m5_unit_joystick2.hpp"

void M5UnitJoystick2::write_bytes(uint8_t addr, uint8_t reg, uint8_t *buffer, uint8_t length)
{
    _wire->beginTransmission(addr);
    _wire->write(reg);
    for (int i = 0; i < length; i++) {
        _wire->write(*(buffer + i));
    }
    _wire->endTransmission();
}

void M5UnitJoystick2::read_bytes(uint8_t addr, uint8_t reg, uint8_t *buffer, uint8_t length)
{
    uint8_t index = 0;
    _wire->beginTransmission(addr);
    _wire->write(reg);
    _wire->endTransmission(false);
    _wire->requestFrom(addr, length);
    for (int i = 0; i < length; i++) {
        buffer[index++] = _wire->read();
    }
}

bool M5UnitJoystick2::begin(TwoWire *wire, uint8_t addr, uint8_t sda, uint8_t scl, uint32_t speed)
{
    _wire  = wire;
    _addr  = addr;
    _sda   = sda;
    _scl   = scl;
    _speed = speed;
    _wire->begin(_sda, _scl);
    _wire->setClock(speed);
    delay(10);
    _wire->beginTransmission(_addr);
    uint8_t error = _wire->endTransmission();
    if (error == 0) {
        return true;
    } else {
        return false;
    }
}

uint16_t M5UnitJoystick2::get_joy_adc_value_x(adc_mode_t adc_bits)
{
    uint8_t data[4];
    uint32_t value;

    if (adc_bits == ADC_16BIT_RESULT) {
        uint8_t reg = JOYSTICK2_ADC_VALUE_12BITS_REG;
        read_bytes(_addr, reg, data, 2);
        value = data[0] | (data[1] << 8);
    } else if (adc_bits == ADC_8BIT_RESULT) {
        uint8_t reg = JOYSTICK2_ADC_VALUE_8BITS_REG;
        read_bytes(_addr, reg, data, 1);
        value = data[0];
    }

    return value;
}

void M5UnitJoystick2::get_joy_adc_16bits_value_xy(uint16_t *adc_x, uint16_t *adc_y)
{
    uint8_t data[4];

    uint8_t reg = JOYSTICK2_ADC_VALUE_12BITS_REG;

    read_bytes(_addr, reg, data, 4);

    memcpy((uint8_t *)adc_x, &data[0], 2);
    memcpy((uint8_t *)adc_y, &data[2], 2);
}

void M5UnitJoystick2::get_joy_adc_8bits_value_xy(uint8_t *adc_x, uint8_t *adc_y)
{
    uint8_t data[4];

    uint8_t reg = JOYSTICK2_ADC_VALUE_8BITS_REG;

    read_bytes(_addr, reg, data, 2);

    *adc_x = data[0];
    *adc_y = data[1];
}

uint16_t M5UnitJoystick2::get_joy_adc_value_y(adc_mode_t adc_bits)
{
    uint8_t data[4];
    uint32_t value;

    if (adc_bits == ADC_16BIT_RESULT) {
        uint8_t reg = JOYSTICK2_ADC_VALUE_12BITS_REG + 2;
        read_bytes(_addr, reg, data, 2);
        value = data[0] | (data[1] << 8);
    } else if (adc_bits == ADC_8BIT_RESULT) {
        uint8_t reg = JOYSTICK2_ADC_VALUE_8BITS_REG + 1;
        read_bytes(_addr, reg, data, 1);
        value = data[0];
    }

    return value;
}

int16_t M5UnitJoystick2::get_joy_adc_12bits_offset_value_x(void)
{
    int16_t value;

    read_bytes(_addr, JOYSTICK2_OFFSET_ADC_VALUE_12BITS_REG, (uint8_t *)&value, 2);

    return value;
}

int16_t M5UnitJoystick2::get_joy_adc_12bits_offset_value_y(void)
{
    int16_t value;

    read_bytes(_addr, JOYSTICK2_OFFSET_ADC_VALUE_12BITS_REG + 2, (uint8_t *)&value, 2);

    return value;
}

int8_t M5UnitJoystick2::get_joy_adc_8bits_offset_value_x(void)
{
    int8_t value;

    read_bytes(_addr, JOYSTICK2_OFFSET_ADC_VALUE_8BITS_REG, (uint8_t *)&value, 1);

    return value;
}

int8_t M5UnitJoystick2::get_joy_adc_8bits_offset_value_y(void)
{
    int8_t value;

    read_bytes(_addr, JOYSTICK2_OFFSET_ADC_VALUE_8BITS_REG + 1, (uint8_t *)&value, 1);

    return value;
}

void M5UnitJoystick2::set_joy_adc_value_cal(uint16_t x_neg_min, uint16_t x_neg_max, uint16_t x_pos_min,
                                            uint16_t x_pos_max, uint16_t y_neg_min, uint16_t y_neg_max,
                                            uint16_t y_pos_min, uint16_t y_pos_max)
{
    uint8_t data[16];

    memcpy(&data[0], (uint8_t *)&x_neg_min, 2);
    memcpy(&data[2], (uint8_t *)&x_neg_max, 2);
    memcpy(&data[4], (uint8_t *)&x_pos_min, 2);
    memcpy(&data[6], (uint8_t *)&x_pos_max, 2);
    memcpy(&data[8], (uint8_t *)&y_neg_min, 2);
    memcpy(&data[10], (uint8_t *)&y_neg_max, 2);
    memcpy(&data[12], (uint8_t *)&y_pos_min, 2);
    memcpy(&data[14], (uint8_t *)&y_pos_max, 2);

    write_bytes(_addr, JOYSTICK2_ADC_VALUE_CAL_REG, (uint8_t *)&data[0], 16);
}

void M5UnitJoystick2::get_joy_adc_value_cal(uint16_t *x_neg_min, uint16_t *x_neg_max, uint16_t *x_pos_min,
                                            uint16_t *x_pos_max, uint16_t *y_neg_min, uint16_t *y_neg_max,
                                            uint16_t *y_pos_min, uint16_t *y_pos_max)
{
    uint8_t data[16];

    read_bytes(_addr, JOYSTICK2_ADC_VALUE_CAL_REG, data, 16);

    memcpy((uint8_t *)x_neg_min, &data[0], 2);
    memcpy((uint8_t *)x_neg_max, &data[2], 2);
    memcpy((uint8_t *)x_pos_min, &data[4], 2);
    memcpy((uint8_t *)x_pos_max, &data[6], 2);
    memcpy((uint8_t *)y_neg_min, &data[8], 2);
    memcpy((uint8_t *)y_neg_max, &data[10], 2);
    memcpy((uint8_t *)y_pos_min, &data[12], 2);
    memcpy((uint8_t *)y_pos_max, &data[14], 2);
}

uint8_t M5UnitJoystick2::get_button_value(void)
{
    uint8_t data[4];

    uint8_t reg = JOYSTICK2_BUTTON_REG;
    read_bytes(_addr, reg, data, 1);

    return data[0];
}

void M5UnitJoystick2::set_rgb_color(uint32_t color)
{
    write_bytes(_addr, JOYSTICK2_RGB_REG, (uint8_t *)&color, 4);
}

uint32_t M5UnitJoystick2::get_rgb_color(void)
{
    uint32_t rgb_read_buff = 0;

    read_bytes(_addr, JOYSTICK2_RGB_REG, (uint8_t *)&rgb_read_buff, 4);

    return rgb_read_buff;
}

uint8_t M5UnitJoystick2::get_bootloader_version(void)
{
    _wire->beginTransmission(_addr);
    _wire->write(JOYSTICK2_BOOTLOADER_VERSION_REG);
    _wire->endTransmission(false);

    uint8_t reg_value;

    _wire->requestFrom((int)_addr, (int)1);
    reg_value = Wire.read();
    return reg_value;
}

uint8_t M5UnitJoystick2::get_i2c_address(void)
{
    _wire->beginTransmission(_addr);
    _wire->write(JOYSTICK2_I2C_ADDRESS_REG);
    _wire->endTransmission(false);

    uint8_t reg_value;

    _wire->requestFrom((int)_addr, (int)1);
    reg_value = Wire.read();
    return reg_value;
}

uint8_t M5UnitJoystick2::set_i2c_address(uint8_t addr)
{
    _wire->beginTransmission(_addr);
    _wire->write(JOYSTICK2_I2C_ADDRESS_REG);
    _wire->write(addr);
    _addr = addr;
    if (_wire->endTransmission() == 0)
        return true;
    else
        return false;
}

uint8_t M5UnitJoystick2::get_firmware_version(void)
{
    _wire->beginTransmission(_addr);
    _wire->write(JOYSTICK2_FIRMWARE_VERSION_REG);
    _wire->endTransmission(false);

    uint8_t reg_value;

    _wire->requestFrom((int)_addr,(int) 1);
    reg_value = Wire.read();
    return reg_value;
}


M5UnitJoystick2::M5UnitJoystick2(KeyEventType keyEvent, PressKeyEventType pressKeyEvent) : m_keyEvent(keyEvent), m_pressKeyEvent(pressKeyEvent)
{
    if (begin(&Wire, JOYSTICK2_ADDR, 8, 9)){
        Serial.println("Encontramos un joystick M5V2");
        set_rgb_color(0x00ff00);
        xTaskCreatePinnedToCore(JoystickTask, "joystickTask", 4096, this, 1, NULL, 0);
    } else{
        Serial.println("No hay un joystick M5V2");
    }
}

void M5UnitJoystick2::JoystickTask(void *pParam)
{
    M5UnitJoystick2 *device = (M5UnitJoystick2 *)pParam;
    uint16_t adc_x, adc_y;
    uint8_t   boton;

    std::unordered_map<SpecKeys, int> keyStates = {
      {JOYK_UP, 0},
      {JOYK_DOWN, 0},
      {JOYK_LEFT, 0},
      {JOYK_RIGHT, 0},
      {JOYK_FIRE, 0}};

  while (true)
  {
    vTaskDelay(10 / portTICK_PERIOD_MS);
    device->get_joy_adc_16bits_value_xy(&adc_x, &adc_y);
    boton=device->get_button_value();
    
    std::vector<SpecKeys> detectedKeys;
       
    if (adc_x > 45000){ 
        // going left
          detectedKeys.push_back(JOYK_LEFT);
        }
    else if (adc_x < 15000){ 
        // going  right
        detectedKeys.push_back(JOYK_RIGHT);
        }
    if (adc_y > 45000){ 
        // going up
          detectedKeys.push_back(JOYK_UP);
        }
    else if (adc_y < 15000){ 
        // going down
          detectedKeys.push_back(JOYK_DOWN);
        }
    if (boton==0)
        {
          detectedKeys.push_back(JOYK_FIRE);
        }
    // unpress any keys that are not detected
    for (auto &keyState : keyStates)
        {
          if (std::find(detectedKeys.begin(), detectedKeys.end(), keyState.first) == detectedKeys.end()){
            if (keyState.second != 0)
            {
              device->m_keyEvent(keyState.first, false);
              keyState.second = 0;
            }
          }
        }
        // for each detected key - check if it's a new press and fire off any events
    int now = millis();
    for (SpecKeys key : detectedKeys){
          // send the first event if the key has not been pressed yet
          if (keyStates[key] == 0){
            device->m_keyEvent(key, true);
            device->m_pressKeyEvent(key);
            keyStates[key] = now;
          }
          else if (now - keyStates[key] > 500){
            // we need to repeat the key
            device->m_pressKeyEvent(key);
            // we'll repeat after 100 ms from now on
            keyStates[key] = now - 400;
          }
        }
  }
}