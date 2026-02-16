#pragma once
#include <Arduino.h>
#include "Data.hpp"

class Sensors
{   
    inline static bool infrarojo_izq{false};
    inline static bool infrarojo_der{false};
    inline static float distancia_ultra{0.0f}
    public:
    static void init()
    {
        pinMode(Pinout::INFRA_IZQ, INPUT);
        pinMode(Pinout::INFRA_DER, INPUT);
        pinMode(Pinout::TRIG_PIN,OUTPUT);
        pinMode(Pinout::ECHO_PIN,INPUT);
    }

    static void read_infrarojo()
    {
        infrarojo_izq = digitalRead(INFRA_IZQ);
        infrarojo_der = digitalRead(INFRA_DER);
    }

    static void read_ultrasonido()
    {
        digitalWrite(TRIG_PIN, LOW);
        delayMicroseconds(2);
        digitalWrite(TRIG_PIN, HIGH);
        delayMicroseconds(10);
        digitalWrite(TRIG_PIN, LOW);
        long duration = pulseIn(ECHO_PIN, HIGH, 1000);
        if (duration != 0)
        {
            float distanciaCm = duration * 0.034 / 2.0;
        }
    }


},