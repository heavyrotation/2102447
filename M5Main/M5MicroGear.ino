/* BEGIN MicroGear event functions */

/* If a new message arrives, do this */
void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {
  //Serial.print("Incoming message --> ");
  msg[msglen] = '\0';
  //Serial.println((char *)msg);
  
  m5State = PROCESSING;


  /* USER CODE BEGIN */
  M5.Lcd.fillRect(2,  40, M5.Lcd.width()-2, 30, 0x0000);
  M5.Lcd.drawString(">" + String((char *)msg), 2, 40, 4);
  //Serial.println((char *)msg);
  
  /* USER CODE END */

}

void onFoundgear(char *attribute, uint8_t* msg, unsigned int msglen) {
  m5State = PROCESSING;
  //Serial.print("Found new member --> ");
  //for (int i = 0; i < msglen; i++)
    //Serial.print((char)msg[i]);
  //Serial.println();
}

void onLostgear(char *attribute, uint8_t* msg, unsigned int msglen) {
  m5State = PROCESSING;
  //Serial.print("Lost member --> ");
  //for (int i = 0; i < msglen; i++)
    //Serial.print((char)msg[i]);
  //Serial.println();
}

/* When a microgear is connected, do this */
void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
  m5State = PROCESSING;
  //Serial.println("Connected to NETPIE...");
  /* Set the alias of this microgear ALIAS */
  microgear.setAlias(ALIAS);
}

/* END MicroGear event functions */