#pragma once
#include <Arduino.h>
#include "StateMachine.hpp"
#include "Data.hpp"
#include "Actuators.hpp"
#include "Sensors.hpp"
#include "MQTT-WIFI.hpp"

class Board
{

  static inline uint32_t junction_time_ms{0};
  static inline bool timeout_ultrasonidos{false};
  /*State Machine declaration*/
  static inline constexpr auto connecting_state = make_state(General_states::Connecting,
        Transition<General_states>{General_states::Operational, []() { return Comms::is_connected(); }}
    );

  static inline constexpr auto operational_state = make_state(General_states::Operational,
        Transition<General_states>{General_states::Fault, []() { return !Comms::is_connected(); }}
    );

  static inline constexpr auto fault_state = make_state(General_states::Fault);

  static inline constexpr auto forward_state = make_state(Operational_states::Forward,
        Transition<Operational_states>{Operational_states::Junction_stop, []() { 
          if(timeout_ultrasonidos){
            return Sensors::distancia_ultra < 6.0; 
          }else
          {
            return false;
          } }}
    );

  static inline constexpr auto junction_stop_state = make_state(Operational_states::Junction_stop,
        Transition<Operational_states>{Operational_states::Junction_forward, []() { return Comms::get_auth_flag(); }}
    );

  static inline constexpr auto junction_forward_state = make_state(Operational_states::Junction_forward,
        Transition<Operational_states>{Operational_states::Forward, []() { 
          if(timeout_ultrasonidos){
            return Sensors::distancia_ultra < 6.0; 
          }else
          {
            false;
          }
        }}
    );

  // static inline constexpr auto junction_forward_state = make_state(Operational_states::Junction_forward,
  //     Transition<Operational_states>{Operational_states::Forward, []() { return Comms::get_end_flag(); }}
  // );

  static inline constinit auto Nested_state_machine = [forward_state,junction_stop_state,junction_forward_state]()consteval{
    auto sm = make_state_machine(Operational_states::Forward, forward_state,junction_stop_state,junction_forward_state);
    using namespace std::chrono_literals;

    /*--------Forward----------*/

    sm.add_enter_action([](){
      Actuators::set_led_green(true);
      Actuators::blink_led_no_color(true);
      Comms::set_end_flag(false);
      Serial.println("forward");
    },forward_state);

    sm.add_enter_action([](){
      timeout_ultrasonidos = false;
      junction_time_ms= Scheduler::get_global_time();
      Scheduler::set_timeout(1500,[](){
        timeout_ultrasonidos = true;
      });
    },forward_state);

    sm.add_cyclic_action([](){
      Sensors::read_ultrasonido();
    },50ms,forward_state);

    sm.add_cyclic_action([](){
        Actuators::control_loop();
      }, 5ms, forward_state);

    /*--------Junction stop----------*/

    sm.add_enter_action([](){
      Actuators::stop();
      Comms::send_auth_request();
      Serial.println("Stop");
    },junction_stop_state);

    sm.add_cyclic_action([](){
      static bool toggle = true;
      Actuators::set_led_blue(toggle);
      Actuators::blink_led_no_color(toggle);
      toggle =!toggle;
    },500ms,junction_stop_state);

    sm.add_cyclic_action([](){
      Comms::send_auth_request();
    },250ms,junction_stop_state);
    
    /*--------Junction forward----------*/

    sm.add_enter_action([](){
      Actuators::set_led_blue(true);
      Comms::set_auth_flag(false);
      timeout_ultrasonidos = false;
      Serial.println("forward");
      junction_time_ms= Scheduler::get_global_time();
      Scheduler::set_timeout(900,[](){
        timeout_ultrasonidos = true;
      });
    },junction_forward_state);

    sm.add_cyclic_action([](){
        Actuators::control_loop();
    }, 5ms, junction_forward_state);

    sm.add_cyclic_action([](){
      Sensors::read_ultrasonido();
    },50ms,junction_forward_state);

    sm.add_cyclic_action([](){
      static bool toggle = true;
      Actuators::blink_led_no_color(toggle);
      toggle =!toggle;
    }, 200ms, junction_forward_state);

    sm.add_exit_action([](){
      uint32_t aux_time = Scheduler::get_global_time() - junction_time_ms;
      Comms::send_time(aux_time);
      timeout_ultrasonidos = false;
    },junction_forward_state);


    return sm;
  }();

  static inline constinit auto State_machine = [connecting_state,operational_state,fault_state]()consteval{
    auto nested_sm = StateMachineHelper::add_nesting(operational_state,Nested_state_machine);
    auto sm = make_state_machine(General_states::Connecting, 
      StateMachineHelper::add_nested_machines(nested_sm),
    connecting_state,operational_state,fault_state);
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
      Serial.print("Va a fault");
      Actuators::stop();
    },fault_state);

    return sm;
  }();


  public: 
  static void start()
  {
    Sensors::init();   
    Actuators::init();
    Scheduler::register_task(1,[](){
      Sensors::read_infrarojo();
    });

    State_machine.start();

    Scheduler::register_task(10,[](){
      State_machine.check_transitions();
      // if(Nested_state_machine.get_current_state()== Operational_states::Junction_forward){
      //     Nested_state_machine.force_change_state(forward_state);
      // }
    });

  }

};