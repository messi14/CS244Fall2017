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
//const char *password = "UCI360114D";

String serverEndPoint = "http://ec2-54-71-163-121.us-west-2.compute.amazonaws.com/server_acc_code.php";
HTTPClient http;    //Declare object of class HTTPClient

  //Specify request destination
int numReadingsPerBatch = 50;
int totalSamples = 1500;
int numSamples = 0;
//vector<String> jsonReadings;
//char *  jsonReadings = (char*) malloc(6000 * sizeof(char)) ;
float *  xv = (float*) malloc(totalSamples * sizeof(float)) ;
float *  yv = (float*) malloc(totalSamples * sizeof(float)) ;
float *  zv = (float*) malloc(totalSamples * sizeof(float)) ;
uint32_t *  Redv = (uint32_t*) malloc(totalSamples * sizeof(uint32_t)) ;
uint32_t *  IRv = (uint32_t*) malloc(totalSamples * sizeof(uint32_t)) ;
// vector<float> yv;
// vector<float> zv;
// vector<uint32_t> Redv;
// vector<uint32_t> IRv;
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
    // Serial.println("inside combinejson");
    // String jsonArray = "";
    // for(int i = 0; i<jsonReadings.size() ; i++){
        
    //     jsonArray+=jsonReadings[i];
    //     if(i == jsonReadings.size()-1) {

    //         continue;
    //     }
        
    //     jsonArray+= ",";

    // }
    // String result = "{\"data\":[";
    // result+=jsonArray;
    // result+="]}";
    // return result;

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

    //jsonReadings.push_back(result);
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
    //void setup(byte powerLevel = 0x1F, byte sampleAverage = 4, byte ledMode = 3, int sampleRate = 400, int pulseWidth = 411, int adcRange = 4096);
    particleSensor.setup(0x1F, 1, 3, 50, 411, 4096); //Configure sensor. Use 6.4mA for LED drive

  myIMU.begin();
  
   
}

void loop()
{
    if (WiFi.status() == WL_CONNECTED) {
        
        
        if(totalSamples == numSamples) {
        String result= "{\"data\":[";
        
       // Serial.println(sizeof(request));
        //Serial.println(request);
        Serial.println("server");
        for(int i = 0 ; i < totalSamples; i++){
            //formJson(xv[i],yv[i],zv[i],Redv[i],IRv[i]);
            result += "{\"X\":";
            result += xv[i];
            result += ",";

            result += "\"Y\":";
            result += yv[i];
            result += ",";

            result += "\"Z\":";
            result += zv[i];
            result += ",";

            result += "\"Red\":";
            result += Redv[i];
            result += ",";

            result += "\"IR\":";
            result += IRv[i];
            result += "}";
            if( i % numReadingsPerBatch != 0)
                result += ",";

            if( i % numReadingsPerBatch == 0) {
            //String request = combineJson();
            result+="]}";
           
            //Serial.println(result);
            http.begin(serverEndPoint);
            http.addHeader("Content-Type", "application/json"); 
            int httpCode = http.POST(result); 
           
            Serial.println(httpCode);
            http.end();  //Close connection
            numSamples=0;
            result= "{\"data\":[";
            }
            
    
        }
        //Serial.println("Sent all to server");
    }
    //Get all parameters
    float x = myIMU.readFloatAccelX();
    float y = myIMU.readFloatAccelY();
    float z = myIMU.readFloatAccelZ();

    uint32_t Red,IR; 
    particleSensor.getRedIRSample(Red,IR);

    xv[numSamples] = x;
    yv[numSamples] = y;
    zv[numSamples] = z;
    Redv[numSamples] = Red;
    IRv[numSamples] = IR;
    numSamples++;
    // Serial.println(sizeof(Red)); 
    // Serial.println(sizeof(x));
    // if(numSamples%100==0){
    //     Serial.println(numSamples);
    // }
    

    // xv.push_back(x);
    // yv.push_back(y);
    // zv.push_back(z);
    // Redv.push_back(Red);
    // IRv.push_back(IR);

    //formJson(x,y,z,Red,IR);
    //delay(1000*1/50); //50Hz
      
      } else {
        Serial.println("Error in WiFi connection");
    }

}

// #include "Arduino.h"
// #include <Wire.h>
// #include "MAX30105.h"
// #include <ESP8266WiFi.h>
// #include <ESP8266HTTPClient.h>
// #include <vector>
// #include <string>
// using namespace std;
// String deviceName = "CS244";

// // WiFi settings
// const char *ssid = "26D-Comrades2.4";
// const char *password = "AisDhaDivHarNit26D";

// String serverEndPoint = "http://ec2-35-166-5-213.us-west-2.compute.amazonaws.com/server_RED_IR_code.php";
// HTTPClient http;    //Declare object of class HTTPClient

// MAX30105 particleSensor;
// long samplesTaken = 0;

// vector<String> jsonReadings;

// void printMacAddress()
// {
//     byte mac[6];
//     WiFi.macAddress(mac);
//     char MAC_char[18]="";

//    for (int i = 0; i < sizeof(mac); ++i)
//     {
//         sprintf(MAC_char, "%s%02x:", MAC_char, mac[i]);
//     }

//     Serial.print("Mac address : ");
//     Serial.print(MAC_char);

//     WiFi.begin(ssid,password);
//     while (WiFi.status() != WL_CONNECTED)
//     {
//         delay(500);
//         Serial.print(".");
//     }
//     Serial.println("");
//     Serial.println("WiFi connected");
//     // Print the IP address
//     Serial.println(WiFi.localIP());
// }

// void setup()
// {
//   Serial.begin(115200);
//   Serial.println("Program started");
//   printMacAddress();

//   //Serial.begin(115200);
//   Serial.println("Initializing...");

//   // Initialize sensor
//   if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
//   {
//     Serial.println("MAX30105 was not found. Please check wiring/power. ");
//     while (1);
//   }
//   //void setup(byte powerLevel = 0x1F, byte sampleAverage = 4, byte ledMode = 3, int sampleRate = 400, int pulseWidth = 411, int adcRange = 4096);
//   particleSensor.setup(0x1F,4, 3, 50, 411, 4096); //Configure sensor. Use 6.4mA for LED drive
  
//     //LED Pulse Amplitude Configuration
//     //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//     //Default is 0x1F which gets us 6.4mA
//     //powerLevel = 0x02; // 0.4mA - Presence detection of ~4 inch
//     //powerLevel = 0x1F, 6.4mA - Presence detection of ~8 inch
//     //powerLevel = 0x7F, 25.4mA - Presence detection of ~8 inch
//     //powerLevel = 0xFF, 50.0mA - Presence detection of ~12 inch
  
//     particleSensor.setPulseAmplitudeRed(0xFF);
//     particleSensor.setPulseAmplitudeIR(0xFF);
// }

// String combineJson() {

//     String jsonArray = "";
//     for(int i = 0; i<jsonReadings.size() ; i++){
        
//         jsonArray+=jsonReadings[i];
//         if(i == jsonReadings.size()-1) {

//             continue;
//         }
        
//         jsonArray+= ",";

//     }
//     String result = "{\"data\":[";
//     result+=jsonArray;
//     result+="]}";
//     return result;

// }

// String formJson(float r, float ir){

//     String result = "{\"Red\":";
//     result += r;
//     result += ",";

//     result += "\"IR\":";
//     result += ir;
    
//     result += "}";

//     //Serial.println(result);
//     jsonReadings.push_back(result);
// }


// void loop()
// {
//     if (WiFi.status() == WL_CONNECTED) {
//       if(samplesTaken == 50) {
//         Serial.println("Done ");
//         http.begin(serverEndPoint);  //Specify request destination
//         http.addHeader("Content-Type", "application/x-www-form-urlencoded");
//         String request = combineJson();
//         //Serial.println(request);
//         int httpCode = http.POST(request);   //Send the request
//         samplesTaken=0;
//         jsonReadings.clear();
//         return;
//         }
//         samplesTaken++;
        
//         uint32_t Red = particleSensor.getRed();
//         uint32_t IR = particleSensor.getIR();
//         formJson(Red, IR);

//         } else {
//         Serial.println("Error in WiFi connection");
//     }

// }
