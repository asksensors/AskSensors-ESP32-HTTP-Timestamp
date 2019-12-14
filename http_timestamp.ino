/*
  Connect ESP32 to AskSensors
 * Description:  This sketch publishing data with timestamp to IOT cloud (https://asksensors.com) using an ESP32 Wifi module.
 *  Author: https://asksensors.com, 2019
 *  github: https://github.com/asksensors
 */
 
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

WiFiMulti WiFiMulti;
HTTPClient ask;
WiFiUDP ntpUDP;   // UDP client
NTPClient timeClient(ntpUDP); // NTP client

// TODO: user config
const char* ssid     = "............."; //Wifi SSID
const char* password = "............."; //Wifi Password
const char* apiKeyIn = "...................";      // API KEY IN
const unsigned int writeInterval = 25000;   // write interval (in ms)

// ASKSENSORS API host config
const char* host = "api.asksensors.com";  // API host name
const int httpPort = 80;      // port
  
void setup(){
  
  // open serial
  Serial.begin(115200);
  Serial.println("*****************************************************");
  Serial.println("********** Program Start : Connect ESP32 to AskSensors.");
  Serial.println("Wait for WiFi... ");

  // connecting to the WiFi network
  WiFiMulti.addAP(ssid, password);
  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  // connected
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  timeClient.begin(); // init NTP
  timeClient.setTimeOffset(0); // 0= GMT, 3600 = GMT+1
  
}


void loop(){

  // Use WiFiClient class to create TCP connections
  WiFiClient client;


  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }else {
  Serial.println("********** Send data with timestamp to ASKSENSORS over HTTP");
  // timestamp
  while(!timeClient.update()) {
  timeClient.forceUpdate();
}
  // get Epoch time
  Serial.print("> NTP Time:");
  Serial.println(timeClient.getFormattedTime());
  long unsigned int timestamp = timeClient.getEpochTime();
  
    // Create a URL for updating module1 and module 2
  String url = "http://api.asksensors.com/write/";
  url += apiKeyIn;
  url += "?module1=";
  url += random(10, 100);
  url += "&module2=";
  url += random(10, 100);
  url += "&t=";
  url += timestamp;
  
  Serial.print("********** requesting URL: ");
  Serial.println(url);
   // send data 
   ask.begin(url); //Specify the URL
  
    //Check for the returning code
    int httpCode = ask.GET();          
 
    if (httpCode > 0) { 
 
        String payload = ask.getString();
        Serial.println(httpCode);
        Serial.println(payload);
      } else {
      Serial.println("Error on HTTP request");
    }
 
    ask.end(); //End 
    Serial.println("********** End ");
    Serial.println("*****************************************************");

  }

  client.stop();  // stop client
  
  delay(writeInterval);    // delay
}
