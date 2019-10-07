#include <M5Stack.h>
#include <WiFi.h>

#define WIFI_SSID "T's iPhone" 
#define WIFI_PASS "poporing" 

WiFiClient client;

void setup() {

  M5.begin();
  M5.Lcd.setTextSize(2);
  M5.Lcd.println("wifi connect begin");
  M5.update();
  
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    M5.Lcd.println("...");
    delay(1000);
  }
  M5.Lcd.println("wifi connected");
  IPAddress ip = WiFi.localIP();
  M5.Lcd.println("IP  Address: " + String(WiFi.localIP().toString()));
  M5.update();

}

void loop() {
  M5.update();
  delay(200);
}
