#include <SoftwareSerial.h>
#include <ArduinoJson.h>

const int DOOR_SENSOR_PIN = 13; // Arduino pin connected to door sensor's pin

int doorState;
int t1; // Time Open Interval
int t2; // Time when door was opened
int dt; // Time change
int timeTotal;
int lastState;
SoftwareSerial s(5,6); 
 
void setup() {
  s.begin(9600);
  Serial.begin(9600);
  pinMode(DOOR_SENSOR_PIN, INPUT_PULLUP); //Initialize Door Sensor Pin
  lastState = doorState;
  timeTotal = 0;
  t1 = 0;
  digitalWrite(4, LOW); //Initialize light to OFF
}
 
void loop() {
  doorState = digitalRead(DOOR_SENSOR_PIN);
  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  if (doorState == HIGH) {
    Serial.println("Door is open");
    if (t1 >= 900) {
        digitalWrite(4, LOW); // Turn the light off after 15 min. or 900 sec.
    } else {
      digitalWrite(4, HIGH); // Light on
    }
    root["status"] = "Open!";
    if (lastState == LOW) {
      // If door was just opened from a closed state
      unsigned long t2mil = millis();
      t2 = t2mil / 1000; // Time that door was opened
      t1 = 0;
    } else {
      // Door has been open 
      unsigned long nowt = millis() / 1000;
      dt = nowt - t2; // Change in time, since door was opened
      t1 = t1 + dt; // Update time open
      timeTotal = timeTotal + dt; // Update total time
      t2 = millis() / 1000;
    }
    root["time_open"] = t1;
    root["time_total"] = timeTotal;
  } else {
    digitalWrite(4, LOW); //Turn light off
    t1 = 0;
    root["status"] = "Closed!";
    root["time_open"] = t1;
    root["time_total"] = timeTotal;
  }
  lastState = doorState;
  if(s.available()>0)
  {
    root.printTo(s); //Send data by Serial Communication to Node MCU
  }
  delay(500);
}
