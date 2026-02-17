#pragma once
#include <Arduino.h>
#include "Data.hpp"
#include "Sensors.hpp"

class Actuators
{
  enum class Direction: uint8_t
  {
    Forward =0,
    Backward =1
  };


  static void init()
  {
     ledcAttach(Pinout::MOTOR1F, Actuator_data::PWM_FREQ, Actuator_data::PWM_RES);
     ledcAttach(Pinout::MOTOR1B, Actuator_data::PWM_FREQ, Actuator_data::PWM_RES);
     ledcAttach(Pinout::MOTOR2F, Actuator_data::PWM_FREQ, Actuator_data::PWM_RES);
     ledcAttach(Pinout::MOTOR2B, Actuator_data::PWM_FREQ, Actuator_data::PWM_RES);
     neopixelWrite(Pinout::PIN_LED, 0, 0, 0);
  }

  static void move(const Direction direction ,const uint32_t& velIzq, const uint32_t& velDer)
  {
    if(direction == Direction::Forward)
    {
      ledcWrite(Pinout::MOTOR1F, velIzq);
      ledcWrite(Pinout::MOTOR1B, 0);
      ledcWrite(Pinout::MOTOR2F, velDer);
      ledcWrite(Pinout::MOTOR2B, 0);
      return;
    }else
    {
      ledcWrite(Pinout::MOTOR1F, 0);
      ledcWrite(Pinout::MOTOR1B, velIzq);
      ledcWrite(Pinout::MOTOR2F, 0);
      ledcWrite(Pinout::MOTOR2B, velDer);
    }
  }

  static void stop()
  {
      ledcWrite(Pinout::MOTOR1F, 0);
      ledcWrite(Pinout::MOTOR1B, 0);
      ledcWrite(Pinout::MOTOR2F, 0);
      ledcWrite(Pinout::MOTOR2B, 0);
  }

  static void control_loop()
  {
    if(Sensors::infrarojo_izq==1 && Sensors::infrarojo_der==1)
      {
        Actuators::move(Actuators::Direction::Forward,Actuator_data::NORMAL_SPEED,Actuator_data::NORMAL_SPEED);
        return;
      }
      if(Sensors::infrarojo_izq==0 && Sensors::infrarojo_der==1)
      {
        Actuators::move(Actuators::Direction::Forward,Actuator_data::NORMAL_SPEED,0);
        return;
      }
      if(Sensors::infrarojo_izq==1 && Sensors::infrarojo_der==0)
      {
        Actuators::move(Actuators::Direction::Forward,0,Actuator_data::NORMAL_SPEED);
        return;
      }
      Actuators::move(Actuators::Direction::Forward,0,0);
  }

  static void set_led_red(bool state)
  {
    if(state == false)
    {
      neopixelWrite(Pinout::PIN_LED, 0, 0, 0);
    }
    else
    {
      neopixelWrite(Pinout::PIN_LED, 255, 0, 0);
    }
  }
  static void set_led_green(bool state)
  {
    if(state == false)
    {
      neopixelWrite(Pinout::PIN_LED, 0, 0, 0);
    }
    else
    {
      neopixelWrite(Pinout::PIN_LED, 0, 255, 0);
    }
  }
  static void set_led_blue(bool state)
  {
    if(state == false)
    {
      neopixelWrite(Pinout::PIN_LED, 0, 0, 0);
    }
    else
    {
      neopixelWrite(Pinout::PIN_LED, 0, 0, 255);
    }
  }
};