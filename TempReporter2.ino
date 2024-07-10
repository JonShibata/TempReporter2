
#include "max6675.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>


float T_Meas_2;
float T_Filt_2;

const char* ssid = "Fryganza";
const char* password = "";

int ktcSO  = 12; // D6
int ktcCS  = 15; // D8
int ktcCLK = 14; // D5


MAX6675 ktc(ktcCLK, ktcCS, ktcSO);

WiFiClient client;

void ConnectToWiFi(void) {

    int CntWifiRetries = 0;
    int CntWifiRetryAbort = 5;
    
    int intWiFiCode;

    WiFi.mode(WIFI_STA);
    intWiFiCode = WiFi.begin(ssid, password);

    Serial.println("");
    Serial.println("Connecting to WiFi");
    Serial.println("");
    Serial.printf("WiFi.begin = %d\n", intWiFiCode);

    while ((WiFi.status() != WL_CONNECTED) && (CntWifiRetries < CntWifiRetryAbort)) {
        CntWifiRetries++;
        delay(1000);
        Serial.print(".");
    }

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("");
        Serial.println("WiFi NOT Connected");
    } else {
        Serial.println("");
        Serial.println("WiFi Connected");
    }
}


float lag_filter(float filtered_value, float measured_value, float filter_constant) {
    float output = 0.0;
    if(isnan(measured_value)){
        output = filtered_value;
    } else{
        output = filtered_value + (measured_value - filtered_value) * filter_constant;
    }
    return (output);
}


void setup(void) {
  Serial.begin(9600);

  ConnectToWiFi();
}


void loop(void) {
    T_Meas_2 = (float)ktc.readFahrenheit();
    delay(1000);
    T_Filt_2 = lag_filter(T_Filt_2, T_Meas_2, 0.1F);
   
    Serial.println("filt= " + String(T_Filt_2) + " meas=" + String(T_Meas_2)) ;

  if (WiFi.status() != WL_CONNECTED) {
    ConnectToWiFi();
  }  
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    String url_str = "http://192.168.4.1/update?T_Filt_2=" + String(T_Filt_2) + "&";

    Serial.println(url_str);
    
    http.begin(client, url_str);  
    
    int httpResponseCode = http.GET();
  
    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
  
    http.end();
  }
}



        
