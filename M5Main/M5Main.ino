/*
  Project Name: M5Stack FIRE IoT RTOS Template
  File name: M5Main.ino

  Written by: Toppppp Athiwat
  Created on: September 24, 2019
  Last modified on: September 28, 2019
*/
/* Private define ------------------------------------------------------------*/


/* Includes ------------------------------------------------------------------*/
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <M5Stack.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <MicroGear.h>
#include <FastLED.h>
#include <Time.h>
#include <TimeLib.h>
#include "icons.h"
#include "credentials.h"
#include "DHTesp.h"
#include "Ticker.h"
/* USER CODE END Includes */


WiFiUDP udp;
WiFiClient client;

MicroGear microgear(client);

/* FastLED Library */
CRGB pixels[10];

/* DHT22 ESP32 */
DHTesp dht;
/** Pin number for DHT11 data pin */
int dhtPin = 26;

/* Device States */
enum m5State_t: byte{
    RESET,
    FAULTY,
    NONETWORK,
    WIFICONNECTED,
    PROCESSING
};
m5State_t m5State = RESET;
m5State_t m5LastState = RESET;
uint16_t netpieState = 0;
uint16_t screenTimer = 0;
uint16_t timeMinute = 0;
uint16_t batteryLevel = 0;

/* UDP and NTP Setup BEGIN */
IPAddress timeServerIP; 
uint16_t localPort = 2390; 
const char* ntpServerName = "ntp.chula.ac.th";

const uint16_t NTP_PACKET_SIZE = 48;			//NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[NTP_PACKET_SIZE]; 	//buffer to hold incoming and outgoing packets

char charBuffer[64];					//For use with sprintf
time_t currentTime;
uint32_t epoch;
const uint32_t DEFAULT_TIME = 1483228800UL; // Jan 1 2017 0:00AM
const uint32_t seventyYears = 2208988800UL; 

TaskHandle_t vNavBarRefreshTaskHandler, vJoinNetworkTaskHandler, 
vSyncTimeTaskHandler, vButtonReadTaskHandler, vMicroGearLoopTaskHandler, 
vLEDdriverTaskHandler, vSensorReadTaskHandler;

/* UDP and NTP setup END */

void setup(){

  /* M5Stack's object funtionality initialization */
  M5.begin(); 
  M5.Power.begin(); 
  M5.Lcd.drawString("M5", 2, 2, 4);
  m5State = NONETWORK;

  /* FastLED's object initialization */ 
  FastLED.addLeds<WS2812B, 15, GRB>(pixels, 10);
  for(int i=0; i<10; i++) { 
            pixels[i].setRGB(0,   0,   0);      
  }
  FastLED.show();

  /* DHT22 initialization */
  dht.setup(dhtPin, DHTesp::DHT22);

  /* WiFi initialization */
  WiFi.begin(WIFI_SSID, WIFI_PASS);  

  //microgear.resetToken(); 
  microgear.init(KEY, SECRET, ALIAS);   /* Initial with KEY, SECRET and also set the ALIAS here */
  microgear.connect(APPID);             /* connect to NETPIE to a specific APPID */

  /* MicroGear Event listeners initialization */
  microgear.on(MESSAGE, onMsghandler);    /* Call onMsghandler() when new message arraives */
  microgear.on(PRESENT, onFoundgear);     /* Call onFoundgear() when new gear appear */
  microgear.on(ABSENT, onLostgear);       /* Call onLostgear() when some gear goes offline */
  microgear.on(CONNECTED, onConnected);   /* Call onConnected() when NETPIE connection is established */

  xTaskCreate(
    vNavBarRefresh, /* Function to implement the task */
    "NavBarRefresh", /* Name of the task */
    20000,  /* Stack size in words */
    NULL,  /* Task input parameter */
    5,  /* Priority of the task */
    &vNavBarRefreshTaskHandler); 

  xTaskCreate(
    vJoinNetwork,          /* Task function. */
    "JoinNetwork",        /* String with name of task. */
    20000,            /* Stack size in bytes. */
    NULL,             /* Parameter passed as input of the task */
    4,                /* Priority of the task. */
    &vJoinNetworkTaskHandler);            /* Task handle. */

  xTaskCreate(
    vSyncTime,          /* Task function. */
    "SyncTime",        /* String with name of task. */
    20000,            /* Stack size in bytes. */
    NULL,             /* Parameter passed as input of the task */
    3,                /* Priority of the task. */
    &vSyncTimeTaskHandler);            /* Task handle. */

  xTaskCreate(
    vButtonRead,
    "ButtonRead",
    20000,
    NULL,
    10,
    &vButtonReadTaskHandler );

  xTaskCreate(
    vMicroGearLoop,
    "MicroGearLoop",
    20000,
    NULL,
    1,
    &vMicroGearLoopTaskHandler );

  xTaskCreate(
    vLEDdriver,
    "LEDdriver",
    20000,
    NULL,
    1,
    &vLEDdriverTaskHandler );

  xTaskCreate(
    vSensorRead,
    "SensorRead",
    20000,
    NULL,
    1,
    &vSensorReadTaskHandler );

}

void loop(){
  /* Nothing here */
}
