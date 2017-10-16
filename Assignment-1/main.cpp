#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
String deviceName = "CS244";

// WiFi settings
const char *ssid = "UCInet Mobile Access";

String serverEndPoint = "http://ec2-52-39-105-140.us-west-2.compute.amazonaws.com/test2.php";
HTTPClient http;    //Declare object of class HTTPClient
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

    WiFi.begin(ssid); //, password);
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
}

void loop()
{
    if (WiFi.status() == WL_CONNECTED) {
        http.begin(serverEndPoint);  //Specify request destination
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        String request = "temperature=70&humidity=33&heartRate=72";
        int httpCode = http.POST(request);   //Send the request
        String response = http.getString();      //Get the response payload
        Serial.println("Response : " + response);    //Print request response payload
        http.end();  //Close connection
    } else {
        Serial.println("Error in WiFi connection");
    }
    delay(600*1000); // delay of 1 min

}