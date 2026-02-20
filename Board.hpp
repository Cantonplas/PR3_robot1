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
        Transition<General_states>{General_states::Operational, []() { return /*Comms::is_connected();*/ true; }}
    );

  static inline constexpr auto operational_state = make_state(General_states::Operational,
        Transition<General_states>{General_states::Fault, []() { return /*!Comms::is_connected();*/false; }}
    );

  static inline constexpr auto fault_state = make_state(General_states::Fault);

  static inline constexpr auto forward_state = make_state(Operational_states::Forward,
        Transition<Operational_states>{Operational_states::Junction_stop, []() { return Sensors::distancia_ultra < 5.0; }}
    );

  // static inline constexpr auto junction_stop_state = make_state(Operational_states::Junction_stop,
  //       Transition<Operational_states>{Operational_states::Junction_forward, []() { return Comms::get_aut_flag(); }}
  //   );

  static inline constexpr auto junction_stop_state = make_state(Operational_states::Junction_stop,
        Transition<Operational_states>{Operational_states::Junction_forward, []() { return Sensors::distancia_ultra > 6.0; }}
    );

  static inline constexpr auto junction_forward_state = make_state(Operational_states::Junction_forward/*, Por determinar
      Transition<Operational_states>{Operational_states::Junction_forward, []() { return Comms::auth_flag; }}*/
  );

  static inline constinit auto Nested_state_machine = [forward_state,junction_stop_state,junction_forward_state]()consteval{
    auto sm = make_state_machine(Operational_states::Forward, forward_state,junction_stop_state,junction_forward_state);
    using namespace std::chrono_literals;

    /*--------Forward----------*/

    sm.add_enter_action([](){
      // Actuators::move(Actuators::Direction::Forward,Actuator_data::MAX_SPEED,Actuator_data::MAX_SPEED); Esto en operational
      Actuators::set_led_green(true);
    },forward_state);

    sm.add_cyclic_action([](){
        Actuators::control_loop();
      }, 5ms, forward_state);

    /*--------Junction stop----------*/

    sm.add_enter_action([](){
      Actuators::stop();
      Comms::send_auth_request();
    },junction_stop_state);

    sm.add_cyclic_action([](){
      static bool toggle = true;
      Actuators::set_led_blue(toggle);
      toggle =!toggle;
    },500ms,junction_stop_state);

    sm.add_cyclic_action([](){
      Comms::send_auth_request();
    },250ms,junction_stop_state);
    
    sm.add_exit_action([](){
      Actuators::move(Actuators::Direction::Forward,Actuator_data::MAX_SPEED,Actuator_data::MAX_SPEED);
    },junction_stop_state);

    /*--------Junction forward----------*/

    sm.add_enter_action([](){
      Actuators::set_led_blue(true);
    },junction_forward_state);

    sm.add_cyclic_action([](){
        Actuators::control_loop();
    }, 5ms, junction_forward_state);

    return sm;
  }();

  static inline constinit auto State_machine = [connecting_state,operational_state,fault_state]()consteval{
    auto sm = make_state_machine(General_states::Connecting, connecting_state,operational_state,fault_state);
    using namespace std::chrono_literals;

    sm.add_enter_action([](){
      Actuators::move(Actuators::Direction::Forward,Actuator_data::MAX_SPEED,Actuator_data::MAX_SPEED);
    },operational_state);

    sm.add_cyclic_action([](){
      static bool toggle = true;
      Actuators::set_led_green(toggle);
      toggle = !toggle;
    },500ms,connecting_state);

    sm.add_enter_action([](){
      Actuators::set_led_red(true);
      Actuators::stop();
    },fault_state);

    sm.add_state_machine(Nested_state_machine,operational_state);

    return sm;
  }();

  public: 
  static void start()
  {
    Scheduler::register_task(1,[](){
      Sensors::read_infrarojo();
    });
    Scheduler::register_task(50,[](){
      Sensors::read_ultrasonido();
    });

    State_machine.start();

    Scheduler::register_task(10,[](){
      State_machine.check_transitions();
    });

  }

};