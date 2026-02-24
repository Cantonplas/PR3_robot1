#pragma once
#include "Board.hpp"

void setup() {
  Serial.begin(115200);
  Board::start();
  Scheduler::start();
  Comms::init();
  xTaskCreate(
    Comms::update,         
   "mqtt_update",       
    8192,             
    NULL,           
    1,                
    NULL              
  );
  
}

void loop() {
  Scheduler::update();
  delay(1);
}


void ErrorHandler(String s)
{
  Serial.print(s);
  while(1){
      Serial.print("Error handler called...");
      delay(500);
  };
}