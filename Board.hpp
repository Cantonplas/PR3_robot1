#pragma once
#include <Arduino.h>
#include "StateMachine.hpp"
#include "Data.hpp"
#include "Actuators.hpp"
#include "Sensors.hpp"
#include "MQTT-WIFI.hpp"

class Board
{
  /*State Machine declaration*/
  static inline constexpr auto connecting_state = make_state(General_states::Connecting,
        Transition<General_states>{General_states::Operational, []() { return Comms::is_connected(); }}
    );

  static inline constexpr auto operational_state = make_state(General_states::Connecting,
        Transition<General_states>{General_states::Fault, []() { return !Comms::is_connected(); }}
    );

  static inline constexpr auto fault_state = make_state(General_states::Fault);

  static inline constexpr auto forward_state = make_state(Operational_states::Forward,
        Transition<Operational_states>{Operational_states::Junction_stop, []() { return Sensors::distancia_ultra < 5.0; }}
    );

  static inline constexpr auto junction_stop_state = make_state(Operational_states::Junction_stop,
        Transition<Operational_states>{Operational_states::Junction_forward, []() { return Comms::auth_flag; }}
    );

  static inline constexpr auto junction_forward_state = make_state(Operational_states::Junction_forward/*, Por determinar
      Transition<Operational_states>{Operational_states::Junction_forward, []() { return Comms::auth_flag; }}*/
  );

  static constinit auto nested_state_machine = [forward_state,junction_stop_state,junction_forward_state]()consteval{
    auto sm = make_state_machine(States::STATE1, state,state2);
    sm.add_enter_action([](){
      transicion1 = false;
      Serial.println("Entro al estado 1");
    },state);

    sm.add_enter_action([](){
      transicion2 = false;
      Serial.println("Entro al estado 2");
    },state2);

    using namespace std::chrono_literals;
    
     sm.add_cyclic_action([](){
        parpadearLed();
        Serial.println("parpadeando led cada 100ms en estado 1");
      }, 100ms, state);

      sm.add_cyclic_action([](){
        parpadearLed();
        Serial.println("parpadeando led cada 500ms en estado 2");
      }, 500ms, state2);

      sm.add_exit_action([](){
            Serial.println("Saliendo de estado 2");
        }, state2);

        sm.add_exit_action([](){
            Serial.println("Saliendo de estado 1");
        }, state);

    return sm;

}();



};