#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <iostream>
#include "freertos/timers.h"
#include <array>
#include <concepts>
#include <cstddef>
#include <type_traits>
#include <utility>
#include <unordered_map>
#include <algorithm>
#include <chrono> 


using ms = std::chrono::milliseconds;

template <typename T, size_t Capacity>
using FixedVector = StaticVector<T, Capacity>;

template<class StateEnum>
concept IsEnum = std::is_enum_v<StateEnum>;

template<class T>
concept ValidTime = std::same_as<T, std::chrono::microseconds>;

using Callback = void (*)();
using Guard = bool (*)();

static constexpr size_t NUMBER_OF_ACTIONS = 20;


class TimedAction 
{
public:
  Callback action = nullptr;
  uint32_t period = 0;
  uint8_t id = 255;
  bool is_on = false;

  TimedAction() = default;
  constexpr bool operator==(const TimedAction&) const = default;
};

template <IsEnum StateEnum>
struct Transition 
{
    StateEnum target;
    Guard predicate;
    constexpr bool operator==(const Transition&) const = default;
};

template <class StateEnum, typename... T>
concept are_transitions = (std::same_as<T, Transition<StateEnum>> && ...);

template <IsEnum StateEnum, size_t NTransitions,size_t Number_of_state_orders=0>
class State 
{ 
private:
  FixedVector<TimedAction,NUMBER_OF_ACTIONS> cyclic_actions = {};
  FixedVector<Callback,NUMBER_OF_ACTIONS> on_enter_actions = {};
  FixedVector<Callback,NUMBER_OF_ACTIONS> on_exit_actions = {};
  StateEnum state = {};
  FixedVector<Transition<StateEnum>, NTransitions> transitions = {};

  static constexpr size_t transition_count = NTransitions;

  template <typename... T>
        requires are_transitions<StateEnum, T...>
    consteval State(StateEnum state, T... transitions)
        : state(state) 
        {
            (this->transitions.push_back(transitions), ...);
        }

  consteval State() = default;

  template <size_t N, size_t O>
  consteval State(const State<StateEnum, N, O>& other){
      state = other.get_state();
      for(const auto& t : other.get_transitions()) transitions.push_back(t);
      for(const auto& a : other.get_cyclic_actions()) cyclic_actions.push_back(a);
      for(const auto& a : other.get_enter_actions()) on_enter_actions.push_back(a);
      for(const auto& a : other.get_exit_actions()) on_exit_actions.push_back(a);
  }
  constexpr bool operator==(const State&) const = default;
    
  

  constexpr const StateEnum& get_state() const { return state; };
  constexpr const auto& get_transitions() const { return transitions; };
  constexpr const auto& get_cyclic_actions() const {return cyclic_actions;};
  constexpr const auto& get_enter_actions() const {return on_enter_actions;};
  constexpr const auto& get_exit_actions() const {return on_exit_actions;};

  consteval void add_enter_action(Callback action)
  {
    on_enter_actions.push_back(action);
  }

  consteval void add_exit_action(Callback action)
  {
    on_exit_actions.push_back(action);
  }

  void enter()
  {
    for (const auto& action : on_enter_actions) 
      {
          if(action) action();
      }
      register_all_timed_actions();
  }

  void exit()
  {
    unregister_all_timed_actions();
    for (const auto& action : on_exit_actions) 
      {
        if(action) action();
      }
  }

  void unregister_all_timed_actions()
  {
    for(size_t i = 0; i < cyclic_actions.size(); i++)
    {
      TimedAction& timed_action = cyclic_actions[i];
      if(timed_action.action == nullptr){continue;}
      if(!timed_action.is_on){ continue;}

      Scheduler::unregister_task(timed_action.id);
      timed_action.is_on = false;
    }
  }

  void register_all_timed_actions()
  {
    for(size_t i = 0; i < cyclic_actions.size(); i++)
    {
      TimedAction& timed_action = cyclic_actions[i];

      if(timed_action.action == nullptr)
      { 
        continue;
      }

      timed_action.id = Scheduler::register_task(timed_action.period, timed_action.action);

      if(timed_action.id == 0)
      {
        for(size_t j = 0; j < i; ++j)
        {
          TimedAction& prev_action = cyclic_actions[j];
          if(prev_action.action == nullptr){ continue; }
          if(!prev_action.is_on){ continue; }
          Scheduler::unregister_task(prev_action.id);
          prev_action.is_on = false;
        }
        ErrorHandler("Failed to register timed action");
        return;
      }
      timed_action.is_on = true;
    }
  }

  void remove_cyclic_action(TimedAction *timed_action)
  {
    if(timed_action->is_on){ unregister_timed_action(timed_action);}

    for(size_t i = 0; i < cyclic_actions.size(); i++)
    {
      TimedAction& slot = cyclic_actions[i];
      if(&slot == timed_action)
      {
          slot = TimedAction{}; 
          return;
      }
    }
  }

  void unregister_timed_action(TimedAction* timed_action)
  {
    Scheduler::unregister_task(timed_action->id);
    timed_action->is_on = false;
  }

  template <ValidTime TimeUnit>
  consteval TimedAction * 
  add_cyclic_action(Callback action,
                                  TimeUnit period){
    TimedAction timed_action = {};

    if constexpr (std::is_same_v<TimeUnit, std::chrono::milliseconds>){
        timed_action.period = period.count();
    }
    else 
    {
        ErrorHandler("Invalid Time Unit");
    }
    
    timed_action.action = action;
    cyclic_actions.push_back(timed_action);
    return &cyclic_actions[cyclic_actions.size() - 1];
  }


};