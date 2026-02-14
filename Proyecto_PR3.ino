#pragma once
#include <Arduino.h>
#include "StateMachine.hpp"
#include "Data.hpp"
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

const int PIN_LED = LED_BUILTIN; // Cámbialo por 4 o 2 si usas uno externo
int idTareaParpadeo;

void parpadearLed() {
    // Lee el estado actual y lo invierte
    digitalWrite(PIN_LED, !digitalRead(PIN_LED));
}

//mover a placa.hpp o algo así, eso de que esté en el main no me gusta
static bool transicion1 = false;
static bool transicion2 = false;

constexpr auto state = make_state(States::STATE1,
        Transition<States>{States::STATE2, []() { return transicion1; }}
    );

constexpr auto state2 = make_state(States::STATE2,
        Transition<States>{States::STATE1, []() { return transicion2; }}
    );

static constinit auto state_machine = [state,state2]()consteval{
    auto sm = make_state_machine(States::STATE1, state,state2);
    sm.add_enter_action([](){
      transicion1 = false;
      Serial.print("Entro al estado 1");
    },state);

    sm.add_enter_action([](){
      transicion2 = false;
      Serial.print("Entro al estado 2");
    },state2);

    using namespace std::chrono_literals;
    
     sm.add_cyclic_action([](){
        parpadearLed();
        Serial.print("parpadeando led cada 100ms en estado 1");
      }, 100ms, state);

      sm.add_cyclic_action([](){
        parpadearLed();
        Serial.print("parpadeando led cada 500ms en estado 1");
      }, 500ms, state2);

      sm.add_exit_action([](){
            Serial.print("Saliendo de estado 2");
        }, state2);

        sm.add_exit_action([](){
            Serial.print("Saliendo de estado 1");
        }, state);

    return sm;

};

void setup() {
  Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);

  idTareaParpadeo = Scheduler::register_task(5000,[](){
    transicion1 = true;
  });

  Scheduler::set_timeout(10000,[](){
      transicion2=true;
  });

   Scheduler::register_task(10, [](){
        state_machine.check_transitions();
    });

  Scheduler::start();

  state_machine.start();

  
  
//  ledcAttach(IN1, PWM_FREQ, PWM_RES);
//  ledcAttach(IN2, PWM_FREQ, PWM_RES);
//  ledcAttach(IN3, PWM_FREQ, PWM_RES);
//  ledcAttach(IN4, PWM_FREQ, PWM_RES);

//  pinMode(INFRA_IZQ, INPUT);
//  pinMode(INFRA_DER, INPUT);
}

void loop() {
  Scheduler::update();
  // int izq = digitalRead(INFRA_IZQ);
  // int der = digitalRead(INFRA_DER);
  // if(izq==1 && der==1)
  // {
  //   moverMotores(VELOCIDAD,VELOCIDAD);
  //   return;
  // }
  // if(izq==0 && der==1)
  // {
  //   moverMotores(VELOCIDAD,0);
  //   return;
  // }
  // if(izq==1 && der==0)
  // {
  //   moverMotores(0,VELOCIDAD);
  //   return;
  // }

  // moverMotores(0,0);
  

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
      Serial.print("Error handler called...");
      delay(0.5);
  };
}