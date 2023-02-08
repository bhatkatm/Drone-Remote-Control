#include <SPI.h>
#include <WiFiNINA.h>

char ssid[] = "TanTheMan"; //  your network SSID (name)
char pass[] = "TanTheMan";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
IPAddress server(192, 168, 4, 1); // numeric IP for Google (no DNS)
//char server[] = "www.google.com";    // name address for Google (using DNS)

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):

WiFiClient client;

/*
  Showing numbers, chars and phrases
                             A (seg[0] in this project)
                            ---
  F (seg[5] in this project) |   | B (seg[1] in this project)
                           |   |
                            --- G (seg[6] in this project)
  E (seg[4] in this project) |   |
                           |   | C (seg[2] in this project)
                            ---  . dot or dicimal (seg[7] in this project)
                             D (seg[3] in this project)

*/
#define A 8
#define B 7
#define C 6
#define D 5
#define E 4
#define F 3
#define G 2
#define DP 9 // decimal
#define common_cathode 0
#define common_anode 1
bool segMode = common_cathode; // set this to your segment type, my segment is common_cathode
int seg[] {A, B, C, D, E, F, G, DP}; // segment pins
byte chars = 35; // max value in the array "Chars"

byte Chars[35][9] {
  {'0', 1, 1, 1, 1, 1, 1, 0, 0}, //0
  {'1', 0, 1, 1, 0, 0, 0, 0, 0}, //1
  {'2', 1, 1, 0, 1, 1, 0, 1, 0}, //2
  {'3', 1, 1, 1, 1, 0, 0, 1, 0}, //3
  {'4', 0, 1, 1, 0, 0, 1, 1, 0}, //4
  {'5', 1, 0, 1, 1, 0, 1, 1, 0}, //5
  {'6', 1, 0, 1, 1, 1, 1, 1, 0}, //6
  {'7', 1, 1, 1, 0, 0, 0, 0, 0}, //7
  {'8', 1, 1, 1, 1, 1, 1, 1, 0}, //8
  {'9', 1, 1, 1, 1, 0, 1, 1, 0}, //9
  {'a', 1, 1, 1, 0, 1, 1, 1, 0}, //A/10
  {'b', 0, 0, 1, 1, 1, 1, 1, 0}, //b/11
  {'c', 1, 0, 0, 1, 1, 1, 0, 0}, //C/12
  {'d', 0, 1, 1, 1, 1, 0, 1, 0}, //d/13
  {'e', 1, 0, 0, 1, 1, 1, 1, 0}, //E/14
  {'f', 1, 0, 0, 0, 1, 1, 1, 0}, //F/15
  {'g', 1, 0, 1, 1, 1, 1, 0, 0}, //G/16
  {'h', 0, 1, 1, 0, 1, 1, 1, 0}, //H/17
  {'i', 0, 0, 0, 0, 1, 1, 0, 0}, //I/18
  {'j', 0, 1, 1, 1, 1, 0, 0, 0}, //J/19
  {'l', 0, 0, 0, 1, 1, 1, 0, 0}, //L/20
  {'n', 0, 0, 1, 0, 1, 0, 1, 0}, //n/21
  {'o', 0, 0, 1, 1, 1, 0, 1, 0}, //o/22
  {'p', 1, 1, 0, 0, 1, 1, 1, 0}, //P/23
  {'q', 1, 1, 1, 0, 0, 1, 1, 0}, //q/24
  {'r', 0, 0, 0, 0, 1, 0, 1, 0}, //r/25
  {'s', 1, 0, 1, 1, 0, 1, 1, 0}, //S/26   looks like number 5
  {'t', 0, 0, 0, 1, 1, 1, 1, 0}, //t/27
  {'u', 0, 1, 1, 1, 1, 1, 0, 0}, //U/28
  {'y', 0, 1, 1, 1, 0, 1, 1, 0}, //y/29
  {'-', 0, 0, 0, 0, 0, 0, 1, 0}, //-/30
  {'.', 0, 0, 0, 0, 0, 0, 0, 1}, //./31
  {']', 1, 1, 1, 1, 0, 0, 0, 0}, //]/32
  {'[', 1, 0, 0, 1, 1, 1, 0, 0}, //[/33
  {'_', 0, 0, 0, 1, 0, 0, 0, 0}, //_/34
};

int xPin = A0;
int yPin = A1;

int xPosition = 0;
int yPosition = 0;

// digital pin 2 has a pushbutton attached to it. Give it a name:
int pushButton = 10;
int pushButton1 = 11;
int pushButton2 = 12;
int pushButton3 = 13;

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(74880);
  while (!Serial) {
    

    ; // wait for serial port to connect. Needed for native USB port only

  }

  // check for the presence of the shield:

  if (WiFi.status() == WL_NO_MODULE) {

    Serial.println("WiFi shield not present");

    // don't continue:

    while (true);

  }

  String fv = WiFi.firmwareVersion();

  if (fv != "1.1.0") {

    Serial.println("Please upgrade the firmware");

  }
  // attempt to connect to Wifi network:
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);

  //activate pull-up resistor on the push-button pin  

  // For versions prior to Arduino 1.0.1
  // pinMode(buttonPin, INPUT);
  // digitalWrite(buttonPin, HIGH);


  // set segment pins as OUTPUT
  pinMode(seg[0], OUTPUT);
  pinMode(seg[1], OUTPUT);
  pinMode(seg[2], OUTPUT);
  pinMode(seg[3], OUTPUT);
  pinMode(seg[4], OUTPUT);
  pinMode(seg[5], OUTPUT);
  pinMode(seg[6], OUTPUT);
  pinMode(seg[7], OUTPUT);

  pinMode(pushButton, INPUT);
  pinMode(pushButton1, INPUT);
  pinMode(pushButton2, INPUT);
  pinMode(pushButton3, INPUT);

}

void setState(bool mode) //sets the hole segment state to "mode"
{ for (int i = 0; i <= 6; i++)
  {
    digitalWrite(seg[i], mode);
  }
}


void PrintNum(char Char) // print any character on the segment ( Note : you can't use capital characters )
{
  int charNum = -1;// set search resault to -1
  setState(segMode);//turn off the segment

  for (int i = 0; i < chars ; i++) { //search for the enterd character
    if (Char == Chars[i][0]) { //if the character found
      charNum = i;//set the resault number into charNum ( because this function prints the character using it's number in the array )
    }
  }

  if (charNum == -1 ) // if the character not found
  {
    for (int i = 0; i <= 6; i++)
    {
      digitalWrite(seg[i], HIGH);
      delay(100);
      digitalWrite(seg[i], LOW);
    }
    for (int i = 0; i <= 2; i++)
    {
      delay(100);
      setState(HIGH);
      delay(100);
      setState(LOW);
    }
  } else // else if the character found print it
  {
    for (int i = 0; i < 8; i++)
    { digitalWrite(seg[i], Chars[charNum][i + 1]);
    }
  }
}

void loop() {
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 5 seconds for connection:
    delay(5000);
  }

  Serial.println("Connected to wifi");
  printWifiStatus();
  Serial.println("\nStarting connection to server...");

  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {

    Serial.println("connected to server");

    // read the input pin:
  int buttonState = digitalRead(pushButton);
  int buttonState1 = digitalRead(pushButton1);
  int buttonState2 = digitalRead(pushButton2);
  int buttonState3 = digitalRead(pushButton3);

    xPosition = analogRead(xPin);
    yPosition = analogRead(yPin);    

    // print out the state of the button:
  if(buttonState == HIGH) {
    Serial.println("Button 1: on");
    PrintNum('f');
    client.println("F"); // forward
  }
    
  if(buttonState1 == HIGH) {
    Serial.println("Button 2: on");
    PrintNum('b');
    client.println("B"); // backward
  }
    
  if(buttonState2 == HIGH) {
    Serial.println("Button 3: on");
    PrintNum('n');
    client.println("N"); // Rotate 90 degrees clockwise
  }
    
  if(buttonState3 == HIGH) {
    Serial.println("Button 4: on");
    PrintNum('p');
    client.println("P"); // Flip
  }    

    if (xPosition >= 0 && xPosition <= 10 && yPosition <= 510 && yPosition >= 490) {
      Serial.println("Left");
      client.println("L");
      PrintNum('l');
    }
    else if (xPosition >= 990 && xPosition <= 1023 && yPosition <= 510 && yPosition >= 490) {
      Serial.println("Right");
      PrintNum('r');
      client.println("R");
    }
    else if (xPosition >= 510 && xPosition <= 520 && yPosition <= 10 && yPosition >= 0) {
      Serial.println("Up");
      PrintNum('u');
      client.println("U");
    }
    else if (xPosition >= 510 && xPosition <= 520 && yPosition <= 1023 && yPosition >= 1000) {
      Serial.println("Down");
      PrintNum('d');
      client.println("D");
    }
    else {
      PrintNum('o');
      client.println("H");
    }
    client.println();
  }
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
