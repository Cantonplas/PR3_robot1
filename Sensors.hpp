#pragma once
#include <Arduino.h>
#include "Data.hpp"

class Sensors
{   
    public:
    inline static bool infrarojo_izq{false};
    inline static bool infrarojo_der{false};
    inline static float distancia_ultra{0.0f};

    inline static volatile unsigned long inicioPulso = 0;
    inline static volatile unsigned long duracionPulso = 0;

    private:
    static void ARDUINO_ISR_ATTR ecoInterrupcion() {
    if (digitalRead(Pinout::ECHO_PIN) == HIGH) {
        inicioPulso = micros();
    } 
    else {
        duracionPulso = micros() - inicioPulso;
        distancia_ultra = duracionPulso * 0.034 / 2.0;
    }
    }

    public:
    static void init()
    {
        pinMode(Pinout::INFRA_IZQ, INPUT);
        pinMode(Pinout::INFRA_DER, INPUT);
        pinMode(Pinout::TRIG_PIN,OUTPUT);
        pinMode(Pinout::ECHO_PIN,INPUT);
        attachInterrupt(digitalPinToInterrupt(Pinout::ECHO_PIN), ecoInterrupcion, CHANGE);
    }

    static void read_infrarojo()
    {
        infrarojo_izq = digitalRead(Pinout::INFRA_IZQ);
        infrarojo_der = digitalRead(Pinout::INFRA_DER);
    }

    static void read_ultrasonido()
    {
        digitalWrite(Pinout::TRIG_PIN, LOW);
        delayMicroseconds(2);
        digitalWrite(Pinout::TRIG_PIN, HIGH);
        delayMicroseconds(10);
        digitalWrite(Pinout::TRIG_PIN, LOW);
    }

};