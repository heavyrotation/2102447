/*

  Project Name: M5Stack FIRE IoT RTOS Template
  File name: RTOSTasks.ino

  Written by: Toppppp Athiwat
  Created on: September 24, 2019
  Last modified on: September 28, 2019

*/

/* FreeRTOS tasks ------------------------------------------------------------*/

void vNavBarRefresh(void* pvParameters){
  for(;;){

    /* WiFi Status & Signal Strength Indicator */
    if(m5State == WIFICONNECTED){
      if(m5LastState != WIFICONNECTED){
        M5.Lcd.pushImage(40, 3, 27, 18, (uint16_t *)wifi3);
        m5LastState = WIFICONNECTED;
      }
    }else if(m5State == PROCESSING){
      if(m5LastState != PROCESSING){
        M5.Lcd.pushImage(40, 3, 27, 18, (uint16_t *)wifi3);
        m5LastState = PROCESSING;
      }
    }else{
      if(m5LastState != NONETWORK){
        M5.Lcd.fillRect(40, 3, 27, 18, 0x0000);
        m5LastState = NONETWORK;
      }
    }

    if(microgear.connected()){
      if(netpieState != microgear.connected()){
        M5.Lcd.pushImage(70, 4, 18, 18, (uint16_t *)netpie);
        netpieState = microgear.connected();
      }
    }else{
      if(netpieState == microgear.connected()){
        M5.Lcd.fillRect(70, 4, 18, 18, 0x0000); //NETPIE icon
        netpieState = microgear.connected();
      }
    }

    /* Time */
    if(timeStatus() == timeSet){
      currentTime = now();
      if(timeMinute != minute(currentTime)){
        timeMinute = minute(currentTime);
        sprintf(charBuffer, "%02d:%02d", (hour(currentTime)+7)%24, minute(currentTime));
        M5.Lcd.drawString(charBuffer , M5.Lcd.width()/2-30, 2, 4);
      }
    }

    /* Power/Battery Status Indicator */
    if(M5.Power.isCharging()){
      if(batteryLevel != 200){
        M5.Lcd.pushImage(M5.Lcd.width()-39, 3, 38, 18, (uint16_t *)battchg);
        batteryLevel = 200;
      }
    }else{
      if(M5.Power.getBatteryLevel() == 100){
        if(batteryLevel != 100){
          M5.Lcd.pushImage(M5.Lcd.width()-39, 3, 38, 18, (uint16_t *)batt100);
          batteryLevel = 100;
        }
      }else if(M5.Power.getBatteryLevel() == 75){
        if(batteryLevel != 75){
          M5.Lcd.pushImage(M5.Lcd.width()-39, 3, 38, 18, (uint16_t *)batt75);
          batteryLevel = 75;
        }
      }else if(M5.Power.getBatteryLevel() == 50){
        if(batteryLevel != 50){
          M5.Lcd.pushImage(M5.Lcd.width()-39, 3, 38, 18, (uint16_t *)batt50);
          batteryLevel = 50;
        }
      }else if(M5.Power.getBatteryLevel() == 25){
        if(batteryLevel != 25){
          M5.Lcd.pushImage(M5.Lcd.width()-39, 3, 38, 18, (uint16_t *)batt25);
          batteryLevel = 25;
        }
      }
    }

    /* Clear Screen when Idle */
    if(m5State == WIFICONNECTED){
      screenTimer++;
      if(screenTimer > 40){
        //M5.Lcd.fillRect(70, 4, 18, 18, 0x0000); //NETPIE icon
        M5.Lcd.fillRect(0,  40, M5.Lcd.width(), 65, 0x0000);
        M5.Lcd.fillRect(2,  M5.Lcd.height()-30, M5.Lcd.width()-2, 30, 0x0000);
        screenTimer = 0;
      }
    }else if(m5State == PROCESSING){
      screenTimer = 0;
    }
    vTaskDelay(500);
  }
}

void vJoinNetwork(void* pvParameters){
  for(;;){
    if(WiFi.status() != WL_CONNECTED){
      m5State = NONETWORK;
      WiFi.reconnect();
    }else{
      // Once connected, do once before state update.
      if(m5State == NONETWORK){
        m5State = WIFICONNECTED;
      }

    }
    
    vTaskDelay(3000);
  }
}

void vSyncTime(void* pvParameters){
	for(;;){
    /* Do only when having an internet connection: m5State == WIFICONNECTED */
    if(m5State == WIFICONNECTED){
      WiFi.hostByName(ntpServerName, timeServerIP);
      sendNTPpacket(timeServerIP);
      vTaskDelay(1000);
      int packetSize = udp.parsePacket();
      if (packetSize) {
        Serial.print("packet received, length=");
        Serial.println(packetSize);
        udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

        unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
        unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);

        /* NTP time = seconds since Jan 1 1900 */
        unsigned long secsSince1900 = highWord << 16 | lowWord; //Shift 16 bits (1 word) to the left then OR with lowWord

        /* Unix time = seconds since Jan 1 1970 */
        epoch = secsSince1900 - seventyYears;

        if(epoch > DEFAULT_TIME){
          setTime(epoch);
        }

        if(timeStatus() == timeSet){
          vTaskDelay(5 * 60 * 1000); // once synced, request synchronization every 5 minutes
        }
      }
    }
    vTaskDelay(5000);
	}
}

unsigned long sendNTPpacket(IPAddress &address)
{
	Serial.println("sending NTP packet...");
	memset(packetBuffer, 0, NTP_PACKET_SIZE);

	// Initialize values needed to form NTP request

	// (see URL above for details on the packets)

	packetBuffer[0] = 0b11100011;   // LI, Version, Mode
	packetBuffer[1] = 0;     // Stratum, or type of clock
	packetBuffer[2] = 6;     // Polling Interval
	packetBuffer[3] = 0xEC;  // Peer Clock Precision

	// 8 bytes of zero for Root Delay & Root Dispersion

	packetBuffer[12]  = 49;
	packetBuffer[13]  = 0x4E;
	packetBuffer[14]  = 49;
	packetBuffer[15]  = 52;

	// all NTP fields have been given values, now

	// you can send a packet requesting a timestamp:

	udp.beginPacket(address, 123); //NTP requests are to port 123
	udp.write(packetBuffer, NTP_PACKET_SIZE);
	udp.endPacket();

}

void vMicroGearLoop(void* pvParameters){
  for(;;){
    microgear.loop();
    if(m5State == PROCESSING){
      m5State = WIFICONNECTED;
    }
    if(!microgear.connected()){
      microgear.connect(APPID);
    }
    vTaskDelay(1000);

  }
}

void vLEDdriver(void* pvParameters){
  for(;;){
    if(m5State == NONETWORK){
      for(int i=0; i<10; i++) { 
          pixels[i].setRGB(0,   50,   0);       
      }
      FastLED.show();
      vTaskDelay(500);
      for(int i=0; i<10; i++) { 
          pixels[i].setRGB(0,   0,   0);       
      }
      FastLED.show();
      vTaskDelay(500);
    }else if(m5State == WIFICONNECTED){
      for(int i=0; i<10; i++) { 
          pixels[i].setRGB(0,   40,   0);       
      }
      FastLED.show();
    }else if(m5State == PROCESSING){
      for(int i=0; i<10; i++) { 
          pixels[i].setRGB(0,   0,   50);      
      }
      FastLED.show();
      vTaskDelay(100);
      for(int i=0; i<10; i++) { 
          pixels[i].setRGB(0,   0,   0);      
      }
      FastLED.show();
      vTaskDelay(100);
      for(int i=0; i<10; i++) { 
          pixels[i].setRGB(0,   0,   50);      
      }
      FastLED.show();
      vTaskDelay(100);
      for(int i=0; i<10; i++) { 
          pixels[i].setRGB(0,   0,   0);      
      }
      FastLED.show();
      vTaskDelay(100);
    }
  }
}

void vButtonRead(void* pvParameters){
  for(;;){
    M5.update();

    /* USER CODE BEGIN *******************************************************/

    if(M5.BtnA.wasPressed()) {
      microgear.chat("Toppppp", "Hello! Toppppp" + String(second(currentTime))); 
      M5.Lcd.fillRect(2,  M5.Lcd.height()-30, M5.Lcd.width()-2, 30, 0x0000);
      M5.Lcd.drawString("<Hello! Toppppp " + String(second(currentTime)), 2, M5.Lcd.height()-30, 4);
      m5State = PROCESSING;
    }

    if(M5.BtnB.wasPressed()) {
      microgear.chat("Suree", "Hello! Suree" + String(second(currentTime))); 
      M5.Lcd.fillRect(2,  M5.Lcd.height()-30, M5.Lcd.width()-2, 30, 0x0000);
      M5.Lcd.drawString("<Hello! Suree " + String(second(currentTime)), 2, M5.Lcd.height()-30, 4);
      m5State = PROCESSING;
    }

    if(M5.BtnC.wasPressed()) {
      microgear.chat("Wanchalerm", "Hello! Wanchalerm" + String(second(currentTime))); 
      M5.Lcd.fillRect(2,  M5.Lcd.height()-30, M5.Lcd.width()-2, 30, 0x0000);
      M5.Lcd.drawString("<Hello! Wanchalerm  " + String(second(currentTime)), 2, M5.Lcd.height()-30, 4);
      m5State = PROCESSING;
    }

    /* USER CODE END *********************************************************/
  
    vTaskDelay(50);
  }
}

void vSensorRead(void* pvParameters){
  for(;;){
    /* USER CODE BEGIN *******************************************************/
    M5.Lcd.drawString(String(analogRead(36)) + "      ", 2, 80, 4);

    /* USER CODE END *********************************************************/
    vTaskDelay(500);
  }
}