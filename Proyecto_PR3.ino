#pragma once
#include "Board.hpp"

void setup() {
  Serial.begin(115200);
  Board::start();
  Scheduler::start();


  
}

void loop() {
  Scheduler::update();
  //hay que hacer en otro hilo mqtt loop o algo así se llama, Comms::update();
}


void ErrorHandler(String s)
{
  Serial.print(s);
  while(1){
      Serial.print("Error handler called...");
      delay(0.5);
  };
}