#pragma once
#include "Board.hpp"


void parpadearLed() {
    // Lee el estado actual y lo invierte
    digitalWrite(PIN_LED, !digitalRead(PIN_LED));
}




void setup() {
  Serial.begin(115200);
  Board::start();
  Scheduler::start();


  
}

void loop() {
  Scheduler::update();
  // int izq = digitalRead(INFRA_IZQ);
  // int der = digitalRead(INFRA_DER);
  // if(izq==1 && der==1)
  // {
  //   moverMotores(VELOCIDAD,VELOCIDAD);
  //   return;
  // }
  // if(izq==0 && der==1)
  // {
  //   moverMotores(VELOCIDAD,0);
  //   return;
  // }
  // if(izq==1 && der==0)
  // {
  //   moverMotores(0,VELOCIDAD);
  //   return;
  // }

  // moverMotores(0,0);
  

}


void ErrorHandler(String s)
{
  Serial.print(s);
  while(1){
      Serial.print("Error handler called...");
      delay(0.5);
  };
}