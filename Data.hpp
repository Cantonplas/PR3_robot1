#pragma once

#define ID_COCHE 1

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
  static constexpr uint8_t PIN_LED = RGB_BUILTIN;

};

