#pragma once

#include <WiFi.h>
#include <ArduinoJson.h>
#include <PicoMQTT.h>
#include "Data.hpp"

extern void ErrorHandler(String s);

/* ---------- WIFI ---------- */
static constexpr char* WIFI_SSID = "RAFAGUAPO";
static constexpr char* WIFI_PASS = "12345678";

/* ---------- MQTT ---------- */
static constexpr char* BROKER_IP = "192.168.4.1";
static constexpr int BROKER_PORT = 1883;

class Comms
{
  private: 
  inline static bool auth_flag = false;
  inline static PicoMQTT::Client mqtt{BROKER_IP, BROKER_PORT, "coche1"};
  inline static portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

  inline static String auth_topic = String("vehiculo/") + String(ID_COCHE) + String("/solicitud");

  public:
  
  static void init()
  {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    
    String topic = String("gestor/") + String(ID_COCHE) + String("/autorizacion");

    mqtt.subscribe(topic, [](const char *payload) {
      String message = payload;
      static JsonDocument dic;         
      DeserializationError errores = deserializeJson(dic, message);

      if (errores) {
          ErrorHandler("Diccionario JSON no ha podido ser creado");
          return;
      }

      if(dic["auth"] == true) {
        portENTER_CRITICAL(&timerMux);
          auth_flag = true;
        portEXIT_CRITICAL(&timerMux);
      }
    });
    mqtt.begin();
  }

  static bool get_aut_flag()
  {
    portENTER_CRITICAL(&timerMux);
    return auth_flag;
    portEXIT_CRITICAL(&timerMux);
  }

  static bool is_connected(){
    return WiFi.status() == WL_CONNECTED;
  }
  
  static void update(void* parameters)
  {
    mqtt.loop();
  }

  static void send_auth_request()
  {
    static JsonDocument jsonBuffer; 
    jsonBuffer["id"] = ID_COCHE; 
    
    char payload[128];
    serializeJson(jsonBuffer, payload);
    
    mqtt.publish(auth_topic, payload);
  }

  static void send_time(uint32_t time_in_ms){
    static JsonDocument jsonBuffer;
    jsonBuffer["id_device"]= ID_COCHE;
    jsonBuffer["time"]=time_in_ms;
    
    char payload[128];
    serializeJson(jsonBuffer, payload);
    mqtt.publish("test/topic", payload);
  }
};