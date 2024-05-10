#include <Arduino.h>

#include <WiFi.h>
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"

const char* ssid = "gitwheel";
const char* password = "password";

#define MOTOR_FORWARD 3
#define MOTOR_BACKWARD 2

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {
      font-family: Helvetica; 
      display: inline-block; 
      margin: 0px auto; 
      text-align: center;
    }
    .button {
      background-color: #4CAF50; 
      border: none;
      color: white; 
      padding: 16px 40px; 
      width: 80%;
      text-decoration: none;
      font-size: 30px;
      margin: 2px;
      cursor: pointer;
    }
  </style>
<body>
  <h1>gitwheel</h1>
  <p><a href="/przod"><button class="button">PRZOD</button></a></p>
  <p><a href="/tyl"><button class="button">TYL</button></a></p>
  <p><a href="/stop"><button class="button">STOP</button></a></p>
</body>
</html>

)rawliteral";

AsyncWebServer server(80);
void setup() {
  WiFi.softAP(ssid, password);

  pinMode(MOTOR_BACKWARD, OUTPUT);
  pinMode(MOTOR_FORWARD, OUTPUT);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", index_html);
  });

  server.begin();
}

void loop() {
}
