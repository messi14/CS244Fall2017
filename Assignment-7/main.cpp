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
const char *ssid = "***";
const char *password = "****";

String serverEndPoint = "http://ec2-35-164-223-99.us-west-2.compute.amazonaws.com/server_acc_code_copy.php";
HTTPClient http;    //Declare object of class HTTPClient

int numReadingsPerBatch = 90;
int totalSamples = 450;
int numSamples = 0;

float *  xv = (float*) malloc(totalSamples * sizeof(float)) ;
float *  yv = (float*) malloc(totalSamples * sizeof(float)) ;
float *  zv = (float*) malloc(totalSamples * sizeof(float)) ;
uint32_t *  Redv = (uint32_t*) malloc(totalSamples * sizeof(uint32_t)) ;
uint32_t *  IRv = (uint32_t*) malloc(totalSamples * sizeof(uint32_t)) ;

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

    WiFi.begin(ssid, password);
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
    particleSensor.setup(0x1F, 1, 3, 50, 411, 4096); //Configure sensor. Use 6.4mA for LED drive

  myIMU.begin();
  http.addHeader("Content-Type", "application/json"); 
  http.begin(serverEndPoint);
   
}

void loop()
{
    if (WiFi.status() == WL_CONNECTED) {
        
        if(totalSamples == numSamples) {
        String result= "{\"data\":{";
        String x_str = "\"X\":[";
        String y_str = "\"Y\":[";
        String z_str = "\"Z\":[";
        String red_str = "\"Red\":[";
        String ir_str = "\"IR\":[";
       // Serial.println(sizeof(request));
        //Serial.println(request);
        Serial.println("server");
        for(int i = 0 ; i < totalSamples; i++){
            x_str +=  xv[i];
            y_str +=  yv[i];
            z_str +=  zv[i];
            red_str +=  Redv[i];
            ir_str +=  IRv[i];
            if( (i+1) % numReadingsPerBatch != 0) {
                x_str += ",";
                y_str += ",";
                z_str += ",";
                red_str += ",";
                ir_str += ",";
            }
            else
            {
                x_str += "]";
                y_str += "]";
                z_str += "]";
                red_str += "]";
                ir_str += "]";

                result+=x_str;
                result+=",";
                result+=y_str;
                result+=",";
                result+=z_str;
                result+=",";
                result+=red_str;
                result+=",";
                result+=ir_str;

                result+="}}";
                //Serial.println(result);
                int httpCode = http.POST(result); 
            
                Serial.println(httpCode);
                http.end();  //Close connection
                numSamples=0;
                result= "{\"data\":{";
                x_str = "\"X\":[";
                y_str = "\"Y\":[";
                z_str = "\"Z\":[";
                red_str = "\"Red\":[";
                ir_str = "\"IR\":[";
            }
            
    
        }
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
      
      } else {
        Serial.println("Error in WiFi connection");
    }

}