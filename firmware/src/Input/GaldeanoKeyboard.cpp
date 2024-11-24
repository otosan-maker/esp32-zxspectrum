#include <Arduino.h>
#include "Serial.h"
#include "GaldeanoKeyboard.h"

GaldeanoKeyboard::GaldeanoKeyboard(KeyEventType keyEvent) : m_keyEvent(keyEvent)
{
  xTaskCreatePinnedToCore(keyboardTask, "keyboardTask", 4096, this, 1, NULL, 0);
}

void GaldeanoKeyboard::keyboardTask(void *pParam)
{
  GaldeanoKeyboard *keyboard = (GaldeanoKeyboard *)pParam;
  //init an reset all filas
  int32_t key,old_key;
  bool down = false;
  
  for(int i=0;i<MAXFILAS;i++){
    pinMode(Filas[i], OUTPUT);
    digitalWrite(Filas[i], LOW); 
  }
  //init an reset all columnas
  for(int j=0;j<MAXFILAS;j++){
    pinMode(Columnas[j], INPUT_PULLDOWN);
  }
  Serial.println("Pines teclado configurados");
  old_key=SPECKEY_NONE;
  while (true){
    key=SPECKEY_NONE;
    for(int i=0;i<MAXFILAS;i++){
      digitalWrite(Filas[i], HIGH);
      for(int j=0;j<MAXCOLUMNAS;j++){
        if (digitalRead(Columnas[j]) == HIGH ){
          Serial.printf("Tecla pulsada F%d, C%d\n",i,j);
          key=m_teclas_v[i][j];
          down=true;
        }
      }
      digitalWrite(Filas[i], LOW);
    }
    
    
    SpecKeys keyValue = SpecKeys(key);
    keyboard->m_keyEvent(keyValue, down);

    vTaskDelay(100);
    down=false;
    keyboard->m_keyEvent(keyValue, down);
  }
}