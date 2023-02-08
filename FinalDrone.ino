/*
  WiFiAccessPoint.ino creates a WiFi access point and provides a web server on it.

  Steps:
  1. Connect to the access point "yourAp"
  2. Point your web browser to http://192.168.4.1/H to turn the LED on or http://192.168.4.1/L to turn it off
     OR
     Run raw TCP "GET /H" and "GET /L" on PuTTY terminal with 192.168.4.1 as IP address and 80 as port
*/

#include <WiFi.h>
#include <WiFiClient.h>
#include <RMTT_Libs.h>
#include <Pixy2I2C.h>
#include <Wire.h>

Pixy2I2C pixy;
RMTT_Protocol tt_sdk;

const int trigPin = 2;
const int echoPin = 12;

//define sound speed in cm/uS
#define SOUND_SPEED 0.034

long duration;
float distanceCm;

void WaitTelloReady()
{
  while (1)
  {
    if (Serial1.available())
    {
      String ret = Serial1.readString();
      if (!strncmp(ret.c_str(), "ETT ok", 6))
      {
        Serial.println(ret.c_str());
        return;
      }
    }
    delay(500);
    tt_sdk.SDKOn();
  }
}


// Set these to your desired credentials.
const char *ssid = "TanTheMan";
const char *password = "TanTheMan";

WiFiServer server(80);


void setup() {

  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");

  // You can remove the password parameter if you want the AP to be open.
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();

  Serial.println("Server started");
    pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  Wire.begin(5,4,100000);
  //Wire.begin(27, 26);
  pixy.init();
  Serial1.begin(1000000, SERIAL_8N1, 23, 18);
  RMTT_RGB::Init();
  RMTT_RGB::SetRGB(255, 0, 0);
  tt_sdk.SDKOn();
  WaitTelloReady();
  
}

void loop() {
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  distanceCm = duration * SOUND_SPEED/2;
  pixy.ccc.getBlocks();
  //pixy.setLamp(1,0);
  // put your main code here, to run repeatedly:
  
  //RMTT_RGB::SetRGB(0, 255, 0); // Uncomment this
  RMTT_RGB::SetRGB(0, 0, 0);
   if (pixy.ccc.numBlocks)
  {    
      if(pixy.ccc.blocks[0].m_signature == 1 && distanceCm <= 200) {
        RMTT_RGB::SetRGB(255, 0, 0);
        tt_sdk.TakeOff();
        //tt_sdk.sendTelloCtrlMsg("takeoff");
        Serial.println("Takeoff");
        delay(3000);
        RMTT_RGB::SetRGB(0, 0, 0);
      }
      else if(pixy.ccc.blocks[0].m_signature == 2 && distanceCm <= 200) {
        RMTT_RGB::SetRGB(0, 0, 255);
        tt_sdk.Land();
        // && pixy.ccc.blocks[j].m_width > 10 && pixy.ccc.blocks[j].m_height > 10
        //tt_sdk.sendTelloCtrlMsg("land");
        Serial.println("land");
        delay(3000);
        RMTT_RGB::SetRGB(0, 0, 0);
      }
      else {
        Serial.println("Not in either");
      }                   
  }
  
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if(c == 'U') {
          Serial.println("Inside the rremote");
          client.print("OK");
          tt_sdk.Forward(100);
          delay(300);
        } else if(c == 'D') {
          Serial.println("Inside the rremote");
          tt_sdk.Back(100);
          delay(300);
        } else if(c == 'L') {
          Serial.println("Inside the rremote");
          tt_sdk.Left(100);
          delay(300);
        } else if(c == 'R') {
          Serial.println("Inside the rremote");
          tt_sdk.Right(100);
          delay(300);
        } else if(c == 'l') {
          Serial.println("Inside the rremote");
          tt_sdk.Land();
          delay(300);
        } else if(c == 'P') {
          Serial.println("Inside the rremote");
          tt_sdk.CCW(90);
          delay(300);
        } 
        else if(c == 'F') {
          Serial.println("Inside the rremote");
          tt_sdk.Up(100);
          delay(300);
        }
        else if(c == 'B') {
          Serial.println("Inside the rremote");
          tt_sdk.Down(100);
          delay(300);
        }
        else if(c == 'N') {
          Serial.println("Inside the rremote");
          tt_sdk.CW(90);
          delay(300);
        }
        
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
            client.print("Click <a href=\"/H\">here</a> to turn ON the LED.<br>");
            client.print("Click <a href=\"/L\">here</a> to turn OFF the LED.<br>");

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
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
