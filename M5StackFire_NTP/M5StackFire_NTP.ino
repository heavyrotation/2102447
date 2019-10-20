/* BEGIN Includes */
#include <WiFi.h>
#include <WiFiUdp.h>
#include <Time.h>
#include <TimeLib.h>
/* END Includes */

/* BEGIN Wi-Fi connectivity */
const char* ssid     = "Toppppp's iPhone";
const char* password = "poporing";
/* END Wi-Fi connectivity */

/* BEGIN UDP protocol setup */
unsigned int localPort = 2390; 
IPAddress timeServerIP; 				//time.nist.gov NTP server address
const char* ntpServerName = "time.nist.gov"; //ntp.chula.ac.th

const int NTP_PACKET_SIZE = 48;			//NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[NTP_PACKET_SIZE]; 	//buffer to hold incoming and outgoing packets

WiFiUDP udp;
/* END UDP protocol setup */

/* BEGIN Global variables */
char charBuffer[64];					//For use with sprintf
time_t currentTime;
unsigned long epoch;
const unsigned long DEFAULT_TIME = 1483228800UL; // Jan 1 2017 0:00AM
const unsigned long seventyYears = 2208988800UL; 

/* END Global variables */

void setup(){
	Serial.begin(115200);

	/* BEGIN Pin I/O assignments */
	pinMode(BUILTIN_LED, OUTPUT);
	/* END Pin I/O assignments */

	/* BEGIN Wi-Fi Connection */
	Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
	/* END Wi-Fi Connection */

	/* BEGIN Task 1: Synchronize time with an NTP server every 2 minutes. */
	xTaskCreate(
		syncNTP, 
		"syncNTP", 
		5000, 
		NULL, 
		10, 		
		NULL
	);
	/* END Task 1 */

	/* BEGIN Task 2: Send out "I'm alive at xx:yy" to the terminal every minute. */
	xTaskCreate(
		echoAlive, 
		"echoAlive", 
		5000, 
		NULL, 
		2, 
		NULL
	);
	/* END Task 2 */ 

	/* BEGIN Task 3: Blink the LED  every second. */
	xTaskCreate(
		blinkLED, 
		"blinkLED", 
		5000, 
		NULL, 
		1, 
		NULL
	);	
	/* END Task 3 */ 
}

void loop(){
	/* Nothing inside. */
}

void syncNTP(void * pvParameters){
	for(;;){
		WiFi.hostByName(ntpServerName, timeServerIP);
		sendNTPpacket(timeServerIP);
		delay(1000);
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
				Serial.println("Time synchronization is successfully performed.");
				currentTime = now();
				sprintf(charBuffer, "The local time (UTC+07:00) is %02d:%02d:%02d.", hour(currentTime)+7, minute(currentTime), second(currentTime));
				Serial.println(charBuffer);
			}

		}
		vTaskDelay(120 * 1000);	//every 2 minutes
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

void echoAlive(void * pvParameters){
	for(;;){
		if(timeStatus() == timeSet && currentTime > DEFAULT_TIME){
			currentTime = now();
			sprintf(charBuffer, "I'm alive at %02d:%02d.", hour(currentTime) + 7, minute(currentTime));
			Serial.println(charBuffer); 	//"I'm alive at xx:yy"
			vTaskDelay(60 * 1000);			//every 1 minute
		}
	}
}

void blinkLED(void * pvParameters){
	for(;;){
		digitalWrite(BUILTIN_LED, !(digitalRead(BUILTIN_LED)));
		vTaskDelay(1000);
	}
}
