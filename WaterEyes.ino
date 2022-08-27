/*
 WiFi Web Server LED Blink

 A simple web server that lets you blink an LED via the web.
 This sketch will print the IP address of your WiFi Shield (once connected)
 to the Serial monitor. From there, you can open that address in a web browser
 to turn on and off the LED on pin 5.

 If the IP address of your shield is yourAddress:
 http://yourAddress/H turns the LED on
 http://yourAddress/L turns it off

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the Wifi.begin() call accordingly.

 Circuit:
 * WiFi shield attached
 * LED attached to pin 5

 created for arduino 25 Nov 2012
 by Tom Igoe

ported for sparkfun esp32 
31.01.2017 by Jan Hendrik Berlin
 
 */

#include <WiFi.h>

const char* ssid     = "The Shire";
const char* password = "LEECINDYCOLINJULIAACTONWILSON";

int IN1 = 13;
int IN2 = 12;
int IN3 = 14;
int IN4 = 27;

int Pin1 = 39;
int Pin2 = 38;
int Pin3 = 37;
int Pin4 = 36;

// Analog input values
float v1 = 0;
float v2 = 0;
float v3 = 0;
float v4 = 0;
float v1_adj = 0;
float v2_adj = 0;
float v3_adj = 0;
float v4_adj = 0;

// Calibration lower and upper bounds
float v1_l = 3509;
float v1_u = 1662;
float v2_l = 3530;
float v2_u = 1675;
float v3_l = 3107;
float v3_u = 2187;
float v4_l = 0;
float v4_u = 4095;

WiFiServer server(80);

void setup()
{
    Serial.begin(115200);
    pinMode(5, OUTPUT);      // set the LED pin mode

    // Set the relay outputs and initiate to high
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);

    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, HIGH);

    pinMode(Pin1, INPUT);
    pinMode(Pin2, INPUT);
    pinMode(Pin3, INPUT);
    pinMode(Pin4, INPUT);


    delay(10);

    // We start by connecting to a WiFi network

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    server.begin();

}

int v = 0;

void loop(){
 WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected

      // Read moisture sensors
      v1 = analogRead(Pin1);
      v2 = analogRead(Pin2);
      v3 = analogRead(Pin3);
      v4 = analogRead(Pin4);

      v1_adj = (v1 - v1_l)/(v1_u - v1_l);
      v2_adj = (v2 - v2_l)/(v2_u - v2_l);
      v3_adj = (v3 - v3_l)/(v3_u - v3_l);
      v4_adj = (v4 - v4_l)/(v4_u - v4_l);

      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("<table>");
            client.print("<tr>");
            client.print("<th>");
            client.print("Plant1");
            client.print("</th>");
            client.print("<th>");
            client.print("Plant2");
            client.print("</th>");
            client.print("<th>");
            client.print("Plant3");
            client.print("</th>");
            client.print("<th>");
            client.print("Plant4");
            client.print("</th>");
            client.print("</tr>");
            client.print("<tr>");
            client.print("<td>");
            client.print(v1_adj);
            client.print("</td>");
            client.print("<td>");
            client.print(v2_adj);
            client.print("</td>");
            client.print("<td>");
            client.print(v3_adj);
            client.print("</td>");
            client.print("<td>");
            client.print(v4_adj);
            client.print("</td>");
            client.print("</tr>");
            client.print("</table>");
            client.print("<br>");
            client.print("Click <a href=\"/H\">here</a> to turn the LED on pin 5 on.<br>");
            client.print("Click <a href=\"/L\">here</a> to turn the LED on pin 5 off.<br>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(5, HIGH);               // GET /H turns the LED on
          digitalWrite(IN1, HIGH);               // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(5, LOW);                // GET /L turns the LED off
          digitalWrite(IN1, LOW);               // GET /H turns the LED on
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
