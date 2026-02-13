#pragma once
#include <Arduino.h>
#include "DataStructs.hpp"
#include "StateMachine.hpp"
// Motor izquierdo: IN1 (GPIO 4), IN2 (GPIO 5)
#define IN1 4
#define IN2 5
// Motor derecho: IN3 (GPIO 6), IN4 (GPIO 7)
#define IN3 6
#define IN4 7

#define INFRA_IZQ 15
#define INFRA_DER 16
#define TRIG_PIN 17
#define ECHO_PIN 18


/* ---------- PWM ---------- */
const int PWM_FREQ = 5000; // 5 kHz
const int PWM_RES = 8; // 0–255
const int VELOCIDAD = 180; // (≈70 % de potencia)


void setup() {
  Serial.begin(115200);
  
 ledcAttach(IN1, PWM_FREQ, PWM_RES);
 ledcAttach(IN2, PWM_FREQ, PWM_RES);
 ledcAttach(IN3, PWM_FREQ, PWM_RES);
 ledcAttach(IN4, PWM_FREQ, PWM_RES);

 pinMode(INFRA_IZQ, INPUT);
 pinMode(INFRA_DER, INPUT);
}

void loop() {
  int izq = digitalRead(INFRA_IZQ);
  int der = digitalRead(INFRA_DER);
  if(izq==1 && der==1)
  {
    moverMotores(VELOCIDAD,VELOCIDAD);
    return;
  }
  if(izq==0 && der==1)
  {
    moverMotores(VELOCIDAD,0);
    return;
  }
  if(izq==1 && der==0)
  {
    moverMotores(0,VELOCIDAD);
    return;
  }

  moverMotores(0,0);
  

}

void moverMotores(int velIzq, int velDer) {
 // El avance se logra aplicando PWM en IN1 e IN3.
 // IN2 e IN4 se mantienen a 0.
 // Motor izquierdo
 ledcWrite(IN1, velIzq);
 ledcWrite(IN2, 0);
 // Motor derecho
 ledcWrite(IN3, velDer);
 ledcWrite(IN4, 0);
}

void ErrorHandler(String s)
{
  Serial.print(s);
  while(1){

  };
}