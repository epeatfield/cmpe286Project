#include <SoftwareSerial.h>
SoftwareSerial s(D6,D5);
#include <ArduinoJson.h>
#include <ArduinoHttpClient.h>
#include "ESP8266WiFi.h"

const char* ssid = "YourWiFi";
const char* pass = "YourPassword";
const char* server = "13.56.213.3";
int port = 8086;

WiFiClient wifi;
HttpClient client = HttpClient(wifi, server, port);

void setup() {
  Serial.begin(9600);
  s.begin(9600);
  WiFi.begin(ssid, pass); //Connect to WiFi
  while (!Serial) continue;

  while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.print("."); //Print . while waiting for connection
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());
}
 
void loop() {
  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(s);
  if (root == JsonObject::invalid())
    return;
 
  Serial.println("JSON received and parsed");
  root.prettyPrintTo(Serial);
  Serial.print("Status ");
  
  Serial.println("");
  String doorStatus = root["status"];
  char copy[10];
  doorStatus.toCharArray(copy, 10);
  Serial.print(doorStatus);
  Serial.print("   Time Open   ");
  int timeOpen = root["time_open"];
  Serial.print(timeOpen);
  Serial.print("   Time Total   ");
  int timeTotal = root["time_total"];
  Serial.print(timeTotal);
  Serial.println("");
  Serial.println("---------------------xxxxx--------------------");

  if (WiFi.status() == WL_CONNECTED) {
    String contentType = "application/x-www-form-urlencoded";
    String postData = "lightData,host=1 door_status=\"";
    postData += doorStatus;
    postData += "\",time_open=\"";
    postData += timeOpen;
    postData += "\",time_total=\"";
    postData += timeTotal;
    postData += "\"";
    
    client.post("/write?db=cmpe286", contentType, postData);
    int statusCode = client.responseStatusCode();
    String response = client.responseBody();
    Serial.print("Status code: ");
    Serial.println(statusCode);
    Serial.print("Response: ");
    Serial.println(response);
    delay(500);
  } else {
    Serial.println("WiFi disconnected");
  }
 
}
