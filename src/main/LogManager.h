#ifndef LOG_MANAGER_H
#define LOG_MANAGER_H

#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>

// ----- Configuration -----
const int maxLogs = 50;

// ----- Globals -----
extern ESP8266WebServer server;
extern WebSocketsServer webSocket;

// ----- Log storage -----
extern String logs[maxLogs];
extern int logIndex;

// ----- Functions -----
void addLog(String message);
void clearLogs();
void handleRoot();
void handleClear();

#endif


String logs[maxLogs];
int logIndex = 0;

void addLog(String message) {
  logs[logIndex] = message;
  logIndex = (logIndex + 1) % maxLogs;
  webSocket.broadcastTXT(message);
}

void clearLogs() {
  for (int i = 0; i < maxLogs; i++) {
    logs[i] = "";
  }
  logIndex = 0;
}

void handleRoot() {
  String html = "<html><head><meta http-equiv='refresh' content='2'></head><body>";
  html += "<form action='/clear' method='POST'><input type='submit' value='Clear logs'></form>";
  html += "<pre>";
  for (int i = 0; i < maxLogs; i++) {
    int index = (logIndex + i) % maxLogs;
    if (logs[index].length() > 0) {
      html += logs[index] + "\n";
    }
  }
  html += "</pre></body></html>";
  server.send(200, "text/html", html);
}

void handleClear() {
  clearLogs();
  server.sendHeader("Location", "/");
  server.send(303);
}