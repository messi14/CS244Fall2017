#include "Arduino.h"
#include <Wire.h>
#include "MAX30105.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
String deviceName = "CS244";

// WiFi settings
const char *ssid = "B585A6";

String serverEndPoint = "http://ec2-35-163-218-145.us-west-2.compute.amazonaws.com/test2.php";
HTTPClient http;    //Declare object of class HTTPClient

MAX30105 particleSensor;
long samplesTaken = 0;

//#define debug Serial //Uncomment this line if you're using an Uno or ESP
//#define debug SerialUSB //Uncomment this line if you're using a SAMD21

void printMacAddress()
{
    byte mac[6];
    WiFi.macAddress(mac);
    char MAC_char[18]="";

   for (int i = 0; i < sizeof(mac); ++i)
    {
        sprintf(MAC_char, "%s%02x:", MAC_char, mac[i]);
    }

    Serial.print("Mac address : ");
    Serial.print(MAC_char);

    WiFi.begin(ssid,"ajrocks@123");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    // Print the IP address
    Serial.println(WiFi.localIP());
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Program started");
  printMacAddress();

  //Serial.begin(115200);
  Serial.println("Initializing...");

  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  //void setup(byte powerLevel = 0x1F, byte sampleAverage = 4, byte ledMode = 3, int sampleRate = 400, int pulseWidth = 411, int adcRange = 4096);
  particleSensor.setup(0x1F,4, 3, 50, 411, 4096); //Configure sensor. Use 6.4mA for LED drive
  
    //LED Pulse Amplitude Configuration
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    //Default is 0x1F which gets us 6.4mA
    //powerLevel = 0x02; // 0.4mA - Presence detection of ~4 inch
    //powerLevel = 0x1F, 6.4mA - Presence detection of ~8 inch
    //powerLevel = 0x7F, 25.4mA - Presence detection of ~8 inch
    //powerLevel = 0xFF, 50.0mA - Presence detection of ~12 inch
  
    particleSensor.setPulseAmplitudeRed(0xFF);
    particleSensor.setPulseAmplitudeIR(0xFF);
}

void loop()
{
    if (WiFi.status() == WL_CONNECTED) {
      if(samplesTaken == (long)50*120) {
        Serial.println("Done ");
            return;
          }
          samplesTaken++;
        http.begin(serverEndPoint);  //Specify request destination
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        uint32_t red = particleSensor.getRed();
        uint32_t IR = particleSensor.getIR();
        // Serial.print(" R[");
        // Serial.print(red);
        // Serial.print("] IR[");
        // Serial.print(IR);
        // Serial.print("]");
        // Serial.println();

        String request = "red=";
        request += red;
        request +=  "&IR=";
        request += IR;
        int httpCode = http.POST(request);   //Send the request
        //String response = http.getString();      //Get the response payload
        //Serial.println("Response : " + response);    //Print request response payload
        http.end();  //Close connection
      
      } else {
        Serial.println("Error in WiFi connection");
    }

}