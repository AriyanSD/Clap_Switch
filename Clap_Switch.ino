// #include <Wire.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_SH110X.h>
// #include "RTClib.h"
#include <IRremote.h>

// RTC_DS1307 rtc;
// #define SCREEN_I2C_ADDR 0x3C
// #define SCREEN_WIDTH 128
// #define SCREEN_HEIGHT 64
// #define OLED_RST_PIN -1

int receiverpin = 10;
IRrecv irrecv(receiverpin);
IRsend irsend;
decode_results results;


// Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST_PIN);

int soundSensor = 3;
int LED = 4;
boolean LEDStatus = false;
unsigned long secondClapTime = 0;
unsigned long firstClapTime = 0;
int clapWindow = 2000;  // Set the time window for detecting claps (in milliseconds)
boolean firstClapDetection = false;
int delayAfterClaps = 2000;  // Set the delay after two claps are detected (in milliseconds)
String lightStatue = " ";
boolean activeMicrophone = true;
int microphonePower=2;
void setup() {
  Serial.begin(9600);
  pinMode(receiverpin, INPUT);
  irrecv.enableIRIn();
  pinMode(soundSensor, INPUT);
  pinMode(LED, OUTPUT);
  pinMode(microphonePower, OUTPUT);
  digitalWrite(microphonePower, HIGH);
  // display.begin(SCREEN_I2C_ADDR, true);
  delay(250);
  // if (!rtc.begin()) {
  //   Serial.println("Couldn't find RTC");
  //   Serial.flush();
  //   abort();
  // }

  // if (!rtc.isrunning()) {
  //   Serial.println("RTC is NOT running, let's set the time!");
  //   rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // }
}

void loop() {
  // rtc and oled codes are very slow and it will not very sutable with our sound sensor clap window
  // DateTime now = rtc.now();
  // display.clearDisplay();
  // display.setTextSize(2);
  // display.setTextColor(1);
  // display.setCursor(0, 0);
  // display.print(now.year(), DEC);
  // display.print('/');
  // display.print(now.month(), DEC);
  // display.print('/');
  // display.print(now.day(), DEC);
  // display.print(" ");
  // display.println();
  // display.print(now.hour(), DEC);
  // display.print(':');
  // display.print(now.minute(), DEC);
  // display.println();
  // display.println(lightStatue);
  // display.display();
  if (irrecv.decode(&results)) {
    Serial.println("decoded");
    Serial.println(results.value, HEX);
    if (results.value == 0XB4B40CF3) {
      Serial.println("LightsON");
      if (LEDStatus == false) {
        LEDStatus = true;
        digitalWrite(LED, HIGH);
        lightStatue = "Ligh Statue: ON";
        delay(2000);  // Delay to avoid rapid toggling
      } else {
        LEDStatus = false;
        digitalWrite(LED, LOW);
        lightStatue = "Ligh Statue: OFF";
        delay(2000);  // Delay to avoid rapid toggling
      }
      delay(100);
      irrecv.resume();
    }
     if (results.value == 0XB4B46C93) {
      Serial.println("MicroPhone switched");
      if (activeMicrophone == false) {
        activeMicrophone = true;
        digitalWrite(microphonePower, HIGH);
        delay(2000);  // Delay to avoid rapid toggling
      } else {
        activeMicrophone = false;
        digitalWrite(microphonePower, LOW);
        delay(2000);// Delay to avoid rapid toggling
      }
      delay(100);
      irrecv.resume();
    }
    irrecv.resume();
  }
  if (activeMicrophone) {
    if (digitalRead(soundSensor) == HIGH) {  // Sound detected
      if (!firstClapDetection) {
        firstClapTime = millis();
        firstClapDetection = true;
        Serial.println("first");
      } else if (firstClapDetection) {

        // Two claps detected within the time window
        Serial.println("second");
        if ((millis() - firstClapTime) < clapWindow) {
          Serial.println("on");
          if (LEDStatus == false) {
            LEDStatus = true;
            digitalWrite(LED, HIGH);
            lightStatue = "Ligh Statue: ON";
            delay(500);  // Delay to avoid rapid toggling
          } else {
            LEDStatus = false;
            digitalWrite(LED, LOW);
            lightStatue = "Ligh Statue: OFF";
            delay(500);  // Delay to avoid rapid toggling
          }
          firstClapDetection = false;
          firstClapTime = 0;
        }
      }
      delay(50);
    }
    if ((millis() - firstClapTime) > clapWindow) {
      firstClapDetection = false;
      firstClapTime = 0;
    }
  }
}
