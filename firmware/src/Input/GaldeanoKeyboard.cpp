#include <Arduino.h>
#include "Serial.h"
#include "GaldeanoKeyboard.h"
#include "../Emulator/keyboard_defs.h"

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
  int teclas[3]={0,0,0};
  int nTeclas=0;
  unsigned long thisUpdate=0,lastUpdate=0;
  int lastTeclas[3]={0,0,0};
  boolean EsIgual=false;
  boolean MandarTecla=true;
  SpecKeys keyValue = SPECKEY_NONE;
  for(int i=0;i<MAXFILAS;i++){
    pinMode(Filas[i], OUTPUT);
    digitalWrite(Filas[i], LOW); 
  }
  //init an reset all columnas
  for(int j=0;j<MAXFILAS;j++){
    pinMode(Columnas[j], INPUT_PULLDOWN);
  }
  Serial.println("Pines teclado configurados");
  
  while (true){
    key=SPECKEY_NONE;
    thisUpdate=millis();
    EsIgual=false;
    for(int i=0;i<MAXFILAS;i++){
      digitalWrite(Filas[i], HIGH);
      for(int j=0;j<MAXCOLUMNAS;j++){
        if (digitalRead(Columnas[j]) == HIGH ){
          //Serial.printf("Tecla pulsada F%d, C%d ... %d\n",i,j,m_teclas_v[i][j]);
          key=m_teclas_v[i][j];
          if(nTeclas<2)
            teclas[nTeclas++]=m_teclas_v[i][j];
          down=true;
        }
      }
      digitalWrite(Filas[i], LOW);
    }
    if(down){
        // han cambiado las teclas?
      for(int k=0;k<nTeclas;k++){
        //Serial.printf("Tecla pulsada old %d, new %d ... tecla %d\n",lastTeclas[k],teclas[k],k);
        if(teclas[k]!=lastTeclas[k])
          break;
        EsIgual=true;
        //Serial.printf("No han cambiado las teclas\n");
      }
    }

    //si no ha pasado un timeout no mandamos las teclas repetidas
    if(EsIgual==true){
      if( (thisUpdate-lastUpdate)<400){
        //Serial.printf("No han cambiado las teclas y aun no ha pasado el timeout\n");
        nTeclas=0;
      }  
    }
    if(nTeclas>0)
      lastUpdate=millis();
    //mandamos las teclas down
    for(int i=0;i<nTeclas;i++){
      keyValue = SpecKeys(teclas[i]);
      //Serial.printf("Tecla almacenada %d\n",teclas[i]);
      keyboard->m_keyEvent(keyValue, down);
      if(keyValue==SPECKEY_RESET){
        esp_restart();
      }
    }
    
    vTaskDelay(100);

    //mandamos las teclas released
    down=false;
    for(int i=0;i<nTeclas;i++){
      keyValue = SpecKeys(teclas[i]);
      keyboard->m_keyEvent(keyValue, down);
    }
    //limpiamos las variables
    nTeclas=0;
    for(int k=0;k<3;k++){
      lastTeclas[k]=teclas[k];
      teclas[k]=0;
    }
    
  }
}