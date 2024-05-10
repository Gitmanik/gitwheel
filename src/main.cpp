#include <Arduino.h>
#include <WiFi.h>

const char* ssid = "gitwheel";
const char* password = "password";

WiFiServer server(80);

#define MOTOR_FORWARD 3
#define MOTOR_BACKWARD 2

void setup() {
  WiFi.softAP(ssid, password);
    server.begin();
  pinMode(MOTOR_BACKWARD, OUTPUT);
  pinMode(MOTOR_FORWARD, OUTPUT);
}

int xx = 100;
// Variable to store the HTTP request
String header;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void loop() {
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        header += c;
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            if (header.indexOf("GET /przod") >= 0) {
              analogWrite(MOTOR_BACKWARD, 0);
              analogWrite(MOTOR_FORWARD, 200);
            } else if (header.indexOf("GET /tyl") >= 0) {
              analogWrite(MOTOR_FORWARD, 0);
              analogWrite(MOTOR_BACKWARD, 200);
            } else if (header.indexOf("GET /stop") >= 0) {
              analogWrite(MOTOR_FORWARD, 0);
              analogWrite(MOTOR_BACKWARD, 0);
            }
            
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px; width: 80%");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");

            client.println("<body><h1>gitwheel</h1>");
            client.println("<p><a href=\"/przod\"><button class=\"button\">PRZOD</button></a></p>");
            client.println("<p><a href=\"/tyl\"><button class=\"button\">TYL</button></a></p>");
            client.println("<p><a href=\"/stop\"><button class=\"button\">STOP</button></a></p>");

            client.println("</body></html>");
            
            client.println();
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
  }
}
