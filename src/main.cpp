#include <Arduino.h>

#include <WiFi.h>
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"

#include <ESP32Servo.h>
Servo servo;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

const char* ssid = "gitwheel";
const char* password = "password";

#define MOTOR_FORWARD 3
#define MOTOR_BACKWARD 2
#define SERVO 20

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
      height:100%;
      width:100%;
    }
    table {
        width:100%;
    }
    td {
        width:50%;
    }
    .vertical {
        transform: rotate(270deg);
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
  <script>
    
var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

window.addEventListener('load', onload);

function onload(event) {
    document.querySelector("body").requestFullscreen();
    initWebSocket();
}

function getBattery(){
    websocket.send("B?");
}

function sendHorizontal(e)
{
    var val = e.value;

    if (e.value > 45 && e.value < 55)
    {
      val = 50;
    }

    var vv = "H=" + val;
    console.log("Horizontal: " + vv);
    websocket.send(vv);
}
function sendVertical(e)
{
    var val = e.value;

    if (e.value > -10 && e.value < 10)
    {
      val = 50;
    }

    var vv = "V=" + val;
    console.log("Vertical: " + vv);

    websocket.send(vv);
}

function initWebSocket() {
    console.log('Trying to open a WebSocket connection…');
    websocket = new WebSocket(gateway);
    websocket.onopen = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage;
}

function onOpen(event) {
    console.log('Connection opened');
    getBattery();
}

function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}

function onMessage(event) {
    console.log(event.data);
    if (event.data.startsWith("B="))
    {
        document.getElementById('bat-viewer').innerHTML = event.data;
    }
}
</script>
<body>
  <h1>gitwheel</h1>
  <table>
  <tr>
  <td>
    <input id="horiz" type="range" min="45" max="135" value="90" oninput="sendHorizontal(this)"/>
  </td>
  <td>
    <input id="vertical" type="range" class="vertical" min="-255" max="255" value="0" oninput="sendVertical(this)"/>
  </td>
  </tr>
</body>
</html>

)rawliteral";

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    String message = (char*)data;
    if (message.startsWith("V=")) {
      int v = message.substring(2).toInt();
      if (v == 0)
      {
        analogWrite(MOTOR_BACKWARD, 0);
        analogWrite(MOTOR_FORWARD, 0);
      }
      else if (v > 0)
      {
        analogWrite(MOTOR_BACKWARD, 0);
        analogWrite(MOTOR_FORWARD, v);
      }
      else
      {
        analogWrite(MOTOR_FORWARD, 0);
        analogWrite(MOTOR_BACKWARD, -v);
      }
    } else if (message.startsWith("H="))
    {
      int h = message.substring(2).toInt();
      servo.write(h);
    }
  }
}

void onWebsocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    default:
      break;
  }
}

void setup() {
  WiFi.softAP(ssid, password);

  pinMode(MOTOR_BACKWARD, OUTPUT);
  pinMode(MOTOR_FORWARD, OUTPUT);
  servo.attach(SERVO);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", index_html);
  });

  ws.onEvent(onWebsocketEvent);
  server.addHandler(&ws);

  server.begin();
}

void loop() {
  ws.cleanupClients();
}
