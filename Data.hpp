#pragma once

#define ID_COCHE 0

enum class General_states: uint8_t
{
  Connecting =0,
  Operational =1,
  Fault=2
};

enum class Operational_states: uint8_t
{
  Forward = 0,
  Junction_stop=1,
  Junction_forward=2
};

namespace Pinout
{
  //Infrarojos:
  static constexpr uint8_t INFRA_IZQ =15;
  static constexpr uint8_t INFRA_DER =16;
  //Ultrasonidos:
  static constexpr uint8_t TRIG_PIN =17;
  static constexpr uint8_t ECHO_PIN =18;

  // Motor izquierdo: IN1 (GPIO 4), IN2 (GPIO 5)
  static constexpr uint8_t MOTOR1F =4;
  static constexpr uint8_t MOTOR1B =5;
  // Motor derecho: IN3 (GPIO 6), IN4 (GPIO 7)
  static constexpr uint8_t MOTOR2F =6;
  static constexpr uint8_t MOTOR2B =7;

  static constexpr uint8_t PIN_LED = RGB_BUILTIN;

};

namespace Actuator_data
{
  static constexpr uint32_t PWM_FREQ = 10000; // 5 kHz
  static constexpr uint8_t PWM_RES = 8; // 0–255
  // static constexpr uint8_t NORMAL_SPEED = 180; // (≈70 % de potencia)
  static constexpr uint8_t NORMAL_SPEED = 255; // (≈70 % de potencia)
  static constexpr uint8_t MAX_SPEED = 255; // (≈70 % de potencia)
};

