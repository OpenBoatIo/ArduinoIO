#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include "LogManager.h"
#include "TemperatureSensor.h"
#include <time.h>

// WiFi credentials
const char* ssid = "**";
const char* password = "**";

// Timezone offset in seconds (Sweden = UTC+1 = 3600, +1h)
// And with daylight saving = 7200 during summer
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

// Web server on port 80
ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);  // WebSocket server on port 81


void setup() {
  // Start OTA
  ArduinoOTA.begin();
  Serial.begin(115200);  
  pinMode(LED_BUILTIN, OUTPUT);

  // Connect to WiFi
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());

setupTime();

webSocket.begin();
  webSocket.onEvent([](uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    // No handling of incoming messages for now
  });
  addLog("WebSocket server started on port 81");

  // Initial log entries
  addLog("ESP started. IP: " + WiFi.localIP().toString());

  // Setup web server routes
  server.on("/", handleRoot);
  server.on("/clear", HTTP_POST, handleClear);
  server.begin();
  addLog("Web server started on port 80");



  
  // Start temperature sensor
  setupTemperatureSensor(); 
}



void loop() {
  ArduinoOTA.handle();
  webSocket.loop();
  server.handleClient();
  // Read temperature
ReadTempSensors();
}
