#include "Arduino.h"
#include <Wire.h>
#include "MAX30105.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "SparkFunLIS3DH.h"
#include "Wire.h"
#include "SPI.h"
#include <vector>
#include <string>
using namespace std;

String deviceName = "CS244";
MAX30105 particleSensor;
LIS3DH myIMU; //Default constructor is I2C, addr 0x19.

// WiFi settings
const char *ssid = "26D-Comrades2.4";
const char *password = "AisDhaDivHarNit26D";

String serverEndPoint = "http://ec2-35-166-5-213.us-west-2.compute.amazonaws.com/server_acc_code.php";
HTTPClient http;    //Declare object of class HTTPClient
int numReadingsPerBatch = 50;

int numSamples = 0;
vector<String> jsonReadings;

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

    WiFi.begin(ssid,password);
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

String combineJson() {

    String jsonArray = "";
    for(int i = 0; i<jsonReadings.size() ; i++){
        
        jsonArray+=jsonReadings[i];
        if(i == jsonReadings.size()-1) {

            continue;
        }
        
        jsonArray+= ",";

    }
    String result = "{\"data\":[";
    result+=jsonArray;
    result+="]}";
    return result;

}

String formJson(float x, float y, float z,  uint32_t Red, uint32_t IR){

    String result = "{\"X\":";
    result += x;
    result += ",";

    result += "\"Y\":";
    result += y;
    result += ",";

    result += "\"Z\":";
    result += z;
    result += ",";

    result += "\"Red\":";
    result += Red;
    result += ",";

    result += "\"IR\":";
    result += IR;
    result += "}";

    jsonReadings.push_back(result);
    //Serial.println(result);
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Program started");
  printMacAddress();
  //Call .begin() to configure the IMU

  if (!particleSensor.begin(Wire, I2C_SPEED_FAST))
    {
        Serial.println("MAX30105 was not found. Please check wiring/power. ");
        while (1);
    }

//     //LED Pulse Amplitude Configuration
//     //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//     //Default is 0x1F which gets us 6.4mA
//     //powerLevel = 0x02; // 0.4mA - Presence detection of ~4 inch
//     //powerLevel = 0x1F, 6.4mA - Presence detection of ~8 inch
//     //powerLevel = 0x7F, 25.4mA - Presence detection of ~8 inch
//     //powerLevel = 0xFF, 50.0mA - Presence detection of ~12 inch

    particleSensor.setPulseAmplitudeRed(0x1F);
    particleSensor.setPulseAmplitudeGreen(0x1F);

    particleSensor.setup(0x1F,4, 3, 400, 411, 4096); //Configure sensor. Use 6.4mA for LED drive

  myIMU.begin();
}

void loop()
{
    if (WiFi.status() == WL_CONNECTED) {
        
        
        if(numReadingsPerBatch == numSamples) {
            
        http.begin(serverEndPoint);  //Specify request destination
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        String request = combineJson();
        Serial.println("Sent to server");
         Serial.println(request);
        int httpCode = http.POST(request);   //Send the request
        http.end();  //Close connection
        numSamples=0;
        jsonReadings.clear();
        return;
    }

    numSamples++;
    //Get all parameters
    float x = myIMU.readFloatAccelX();
    float y = myIMU.readFloatAccelY();
    float z = myIMU.readFloatAccelZ();
    uint32_t Red = particleSensor.getRed();
    uint32_t IR = particleSensor.getIR();
    // Serial.println(Red); 
    // Serial.println(IR);
    formJson(x,y,z,Red,IR);
    delay(1000*1/50); //50Hz
      
      } else {
        Serial.println("Error in WiFi connection");
    }

}