#include "Arduino.h"
#include <Wire.h>
#include "MAX30105.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "SparkFunLIS3DH.h"
#include "Wire.h"
#include "SPI.h"
#include <vector>
using namespace std;

String deviceName = "CS244";
LIS3DH myIMU; //Default constructor is I2C, addr 0x19.

// WiFi settings
const char *ssid = "B585A6";

String serverEndPoint = "http://ec2-35-167-45-82.us-west-2.compute.amazonaws.com/server_acc_code.php";
HTTPClient http;    //Declare object of class HTTPClient
int numReadingsPerBatch = 80;

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

String formJson(float x, float y, float z){

    String result = "{\"X\":";
    result += x;
    result += ",";

    result += "\"Y\":";
    result += y;
    result += ",";

    result += "\"Z\":";
    result += z;
    result += "}";

    jsonReadings.push_back(result);
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Program started");
  printMacAddress();
  //Call .begin() to configure the IMU

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
    // Serial.print("\nAccelerometer:\n");
    // Serial.print(" X = ");
    // Serial.println(myIMU.readFloatAccelX(), 4);
    // Serial.print(" Y = ");
    // Serial.println(myIMU.readFloatAccelY(), 4);
    // Serial.print(" Z = ");
    // Serial.println(myIMU.readFloatAccelZ(), 4);
    formJson(x,y,z);
    delay(1000*1/50); //50Hz
      
      } else {
        Serial.println("Error in WiFi connection");
    }

}