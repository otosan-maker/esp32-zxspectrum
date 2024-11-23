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
  for(int i=0;i<MAXFILAS;i++){
    pinMode(Filas[i], OUTPUT);
    digitalWrite(Filas[i], LOW); 
    Serial.printf("fila output %d",Filas[i]);
  }
  //init an reset all columnas
  for(int j=0;j<MAXFILAS;j++){
    pinMode(Columnas[j], INPUT_PULLDOWN);
  }
  Serial.println("Pines teclado configurados");
  while (true)
  {
    //Serial.println("Teclado task");
    for(int i=0;i<MAXFILAS;i++){
      digitalWrite(Filas[i], HIGH);
      for(int j=0;j<MAXCOLUMNAS;j++){
        if (digitalRead(Columnas[j]) == HIGH ){
          Serial.printf("Tecla pulsada F%d, C%d\n",i,j);
        }
      }
      digitalWrite(Filas[i], LOW);
    }
    vTaskDelay(100);
  }
}