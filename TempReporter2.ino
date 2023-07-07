#include "max6675.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>



float T_Meas_2;
float T_Filt_2;

char* ssid = "ESP-A3CAC0";
char* password = "";

int ktcSO  = 12;
int ktcCS  = 13;
int ktcCLK = 14;

MAX6675 ktc(ktcCLK, ktcCS, ktcSO);


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

    return (filtered_value + (measured_value - filtered_value) * filter_constant);
}


void setup(void) {
  Serial.begin(115200);

  ConnectToWiFi();
}


void loop(void) {
    T_Meas_2 = (float)ktc.readFahrenheit();
    delay(200);
    T_Filt_2 = lag_filter(T_Filt_2, T_Meas_2, 0.1);
   
    Serial.println(String(T_Filt_2));

  if (WiFi.status() != WL_CONNECTED) {
    ConnectToWiFi();
  }  
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    String url_str = "http://192.168.4.1/update?T_Filt_2=" + String(T_Filt_2) + "&";

Serial.println(url_str);
    
    
    http.begin(url_str);  
    
    int httpResponseCode = http.GET();
  
    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
//      String response = http.getString();
//      Serial.println(response);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
  
    http.end();
  }
  delay(1000);
}



        
