/*************************************************************

  This is a simple demo of sending and receiving some data.
  Be sure to check out other examples!
 *************************************************************/

/** DATASTREAMS
 V0: Fan
 V2: Timer
 V4: Nighttime light
 V5: Filter
 V6: Heating
 V7: Water temperature
 V8: Air temperature
 V9: Daylight
*/

/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID           "TMPL4z5fh3OQ4"
#define BLYNK_TEMPLATE_NAME         "Cheburaqua Template"
#define BLYNK_AUTH_TOKEN            "1GXDzFnEc88ECQ2SlKDXz4tJa8LVMH3G"

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

/* DS18B20 Termometer includes */
#include <OneWire.h>
#include <DallasTemperature.h>

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "sandstrasse";
char pass[] = "derpar0l";

/** Labels to pins mapping
  D4: pin 2
  D2: pin 4
  D1: pin 5
  D5: pin 14
  D6: pin 12
  D7: pin 13
  
*/

#define TEMP_PIN 2
#define reley_daylight_pin 4
#define relay_nightlight_pin 5
#define relay_fan_pin 14
#define relay_heating_pin 12
//#define relay_filter_pin 13

OneWire oneWire(TEMP_PIN);

// is fan active
int fanState = 0;
int fanWorkingTime = 0;
int fanWaitingTime = 0;

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

BlynkTimer timer;

void SwitchRelay(int value, int relay) {
  if (value == 0) {
    digitalWrite(relay, HIGH);
  } else {
    digitalWrite(relay, LOW);
  }
}

// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED(){
  Blynk.syncVirtual(V0, V4, V9);
}

// V9 - Daylight datastrem
BLYNK_WRITE(V9){
  int value = param.asInt();
  SwitchRelay(value, reley_daylight_pin);
}
// V4 - Hightlight datastrem
BLYNK_WRITE(V4) {
  int value = param.asInt();
  SwitchRelay(value, relay_nightlight_pin);
}
// V0 - Fan datastrem
BLYNK_WRITE(V0) {
  int value = param.asInt();
  SwitchRelay(value, relay_fan_pin);
}
// V6 - Heating datastrem
BLYNK_WRITE(V6) {
  int value = param.asInt();
  SwitchRelay(value, relay_heating_pin);
}
// V2 - timer datastream
BLYNK_WRITE(V2) {
  int value = param.asInt();
}

// This function is being called every secont and:
// 1. Sends Arduino's uptime to Virtual Pin 2.
// 2. Sends water temperature to Virtual Pin 7.
// 3. Sends air temperature to Virtual Pin 8.
void myTimerEvent(){
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  
  sensors.requestTemperatures();
  float tempW = sensors.getTempCByIndex(0);
  float tempA = sensors.getTempCByIndex(1);
  if (tempW != DEVICE_DISCONNECTED_C && tempA != DEVICE_DISCONNECTED_C) {
    Serial.print(tempW);
    Serial.println(" C");
    Serial.print(tempA);
    Serial.println(" C");
    Blynk.beginGroup();
    Blynk.virtualWrite(V7, tempW);
    Blynk.virtualWrite(V8, tempA);
    Blynk.endGroup();
  } else {
    Serial.println("Error: Could not read temperature data");
  }
  Blynk.virtualWrite(V2, millis() / 1000);

  if (fanState == 0) {
    if (fanWaitingTime < 60) {
      fanWaitingTime += 1;
    } else {
      Blynk.virtualWrite(V0, 1);
      SwitchRelay(1, relay_fan_pin);
      fanWaitingTime = 0;
      fanState = 1;
    }
  } else {
    if (fanWorkingTime < 10) {
      fanWorkingTime += 1;
    } else {
      Blynk.virtualWrite(V0, 0);
      SwitchRelay(0, relay_fan_pin);
      fanWorkingTime = 0;
      fanState = 0;
    }
  }
}

void setup(){
  // Debug console
  Serial.begin(115200);
  pinMode(reley_daylight_pin, OUTPUT);
  pinMode(relay_nightlight_pin, OUTPUT);
  pinMode(relay_fan_pin, OUTPUT);
  pinMode(relay_heating_pin, OUTPUT);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  sensors.begin();
  // You can also specify server:
  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, IPAddress(192,168,1,100), 8080);

  // Setup a function to be called every second
  timer.setInterval(1000L, myTimerEvent);
}

void loop(){
  Blynk.run();
  timer.run();
  // Remember to avoid delay() function!
}
