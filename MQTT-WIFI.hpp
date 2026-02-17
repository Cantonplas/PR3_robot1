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
  static inline PicoMQTT::Client mqtt{BROKER_IP, BROKER_PORT, "coche1"};

  public: 
  static inline constinit bool auth_flag{false};

  static void init()
  {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    mqtt.subscribe("gestor"+std::to_string(ID_COCHE)+"autorizacion", [](const char *topic,const void *payload,size_t payload_size) 
    {
      String message = static_cast<char*>payload;
      static StaticJsonDocument<200> dic; 
      DeserializationError errores = deserializeJson(dic, message);

      if (errores) 
      {
        ErrorHandler("Diccionario JSON no ha podido ser creado");
        return;
      }

      if(dic["auth"]==true)
      {
        auth_flag = true;
      }
    }
    );
    mqtt.begin();
  }

  static bool is_connected(){
    return WiFi.status() == WL_CONNECTED;
  }
  
  static void update()
  {
    mqtt.update();
  }

  static void send_auth_request()
  {
    static JsonDocument jsonBuffer; 
    jsonBuffer["id"] = ID_COCHE; 
    
    char payload[256];
    serializeJson(jsonBuffer, payload);
    mqtt.publish("vehiculo/<id>/solicitud", payload);
  }

  static void send_time(uint32_t time_in_ms){
    static StaticJsonDocument doc;
    doc["id_device"]= ID_COCHE;
    doc["time"]=time_in_ms;
    // doc[""]
    char jsonBuffer;
    serializeJson(doc, jsonBuffer);
    mqtt.publish("test/topic", payload);
  }
};