#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "DHT.h"
#include <AntaresESP8266MQTT.h>

#define DHTPIN 5
#define DHTTYPE DHT22 

DHT dht(DHTPIN, DHTTYPE);


#define ACCESSKEY "" 
#define projectName ""
#define deviceName "" 

AntaresESP8266MQTT antares(ACCESSKEY);

const char* ssid = "";
const char* password = ""; 
//const char* ssid = "";
//const char* password = "";


float h;
float t;
float f;
float hic;
float hif;

ESP8266WebServer server(80);

void setup() {
   Serial.begin(115200);

  antares.setDebug(true);   
  antares.wifiConnection(ssid, password);  
  antares.setMqttServer();  
   
   Serial.println("DHT Sensor Activated!");
   dht.begin();

   Serial.println("Connecting to");
   Serial.println(ssid);

   WiFi.begin(ssid, password);

   while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    }
    Serial.println("");
    Serial.println("WIFI CONNECTED..!");
    Serial.print("Got IP : "); Serial.println(WiFi.localIP());

    server.on("/", handle_OnConnect);
    server.onNotFound(handle_NotFound);

    server.begin();
    Serial.println("HTTP Server Started!!!");
    Serial.println("Just copy the IP above into your browser");
    
} 

void loop() {
  
    server.handleClient();

    antares.checkMqttConnection(); 
}

void handle_OnConnect() {  
   float h = dht.readHumidity();
   
   float t = dht.readTemperature();
   
   float f = dht.readTemperature(true);
   
   if (isnan(h) || isnan(t) || isnan(f)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
   }

     
   float hif = dht.computeHeatIndex(f, h);
  
   float hic = dht.computeHeatIndex(t, h, false);

   

  server.send(200, "text/html", SendHTML(h,t,f,hif,hic));

   Serial.println(WiFi.localIP());
   Serial.print ("Humidity: ");
   Serial.print (h);
   Serial.print (" %\t");
   Serial.print ("Temperature: ");
   Serial.print (t);
   Serial.print("\xC2\xB0");
   Serial.print ("C\t ");
   Serial.print (f);
   Serial.print("\xC2\xB0");
   Serial.print ("F\t");
   Serial.print ("Heat index: ");
   Serial.print (hic);
   Serial.print("\xC2\xB0");
   Serial.print ("C\t ");
   Serial.print (hif);
   Serial.print("\xC2\xB0");
   Serial.println ("F ");


//   antares.add("Humidity: ", h," %");
//   antares.add("Celsius: ", t,"\xC2\xB0 C");
//   antares.add("Fahrenheit: ", f,"\xC2\xB0 F");
//   antares.add("Heat Index: ", hic,"\xC2\xB0 C");
//   antares.add("Heat Index: ", hif,"\xC2\xB0 F");

   antares.add("Humidity: ", h);
   antares.add("Celsius: ", t);
   antares.add("Fahrenheit: ", f);
   antares.add("Heat Index: ", hic);
   antares.add("Heat Index: ", hif);

  antares.publish(projectName, deviceName);
  delay(3000);
}  

void handle_NotFound(){
  server.send(404, "text/plain", "Not Found");
  }

String SendHTML(float tstat,float hstat,float fstat,float hifstat,float hicstat){
  String ptr = "<!DOCTYPE html> <html>\n";
   ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
//  ptr +="<meta http-equiv="refresh" content="2" >\n";
  ptr +="<link href=\"https://fonts.googleapis.com/css?family=Open+Sans:300,400,600\" rel=\"stylesheet\">\n";
  ptr +="<title>Temperature & Humidity Status</title>\n";
  ptr +="<style>html { font-family: 'Open Sans', sans-serif; display: block; margin: 0px auto; text-align: center;color: #333333;}\n";
  ptr +="body{margin-top: 50px;}\n";
  ptr +="h1 {margin: 50px auto 30px;}\n";
  ptr +=".side-by-side{display: inline-block;vertical-align: middle;position: relative;}\n";
  
  ptr +=".humidity-icon{background-color: #3498db;width: 30px;height: 30px;border-radius: 50%;line-height: 36px;}\n";
  ptr +=".humidity-text{font-weight: 600;padding-left: 15px;font-size: 19px;width: 160px;text-align: left;}\n";
  ptr +=".humidity{font-weight: 300;font-size: 60px;color: #3498db;}\n";
  
  ptr +=".temperature-icon{background-color: #f39c12;width: 30px;height: 30px;border-radius: 50%;line-height: 40px;}\n";
  ptr +=".temperature-text{font-weight: 600;padding-left: 15px;font-size: 19px;width: 160px;text-align: left;}\n";
  ptr +=".temperature{font-weight: 300;font-size: 60px;color: #f39c12;}\n";

  ptr +=".fahrenheit-icon{background-color: #eff312;width: 30px;height: 30px;border-radius: 50%;line-height: 40px;}\n";
  ptr +=".fahrenheit-text{font-weight: 600;padding-left: 15px;font-size: 19px;width: 160px;text-align: left;}\n";
  ptr +=".fahrenheit{font-weight: 300;font-size: 60px;color: #eff312;}\n";    

  ptr +=".hic-icon{background-color: #12f312;width: 30px;height: 30px;border-radius: 50%;line-height: 40px;}\n";
  ptr +=".hic-text{font-weight: 600;padding-left: 15px;font-size: 19px;width: 160px;text-align: left;}\n";
  ptr +=".hic{font-weight: 300;font-size: 60px;color: #12f312;}\n";

  ptr +=".hif-icon{background-color: #ca12f3;width: 30px;height: 30px;border-radius: 50%;line-height: 40px;}\n";
  ptr +=".hif-text{font-weight: 600;padding-left: 15px;font-size: 19px;width: 160px;text-align: left;}\n";
  ptr +=".hif{font-weight: 300;font-size: 60px;color: #ca12f3;}\n";
 
  ptr +=".superscript{font-size: 17px;font-weight: 600;position: absolute;right: -20px;top: 15px;}\n";
  ptr +=".data{padding: 10px;}\n";
  ptr +="</style>\n";

ptr +="<script>\n";
ptr +="setInterval(loadDoc,200);\n";
ptr +="function loadDoc() {\n";
ptr +="var xhttp = new XMLHttpRequest();\n";
ptr +="xhttp.onreadystatechange = function() {\n";
ptr +="if (this.readyState == 4 && this.status == 200) {\n";
ptr +="document.getElementById(\"webpage\").innerHTML =this.responseText}\n";
ptr +="};\n";
ptr +="xhttp.open(\"GET\", \"/\", true);\n";
ptr +="xhttp.send();\n";
ptr +="}\n";
ptr +="</script>\n";
  
  ptr +="</head>\n";
  ptr +="<body>\n";
  
   ptr +="<div id=\"webpage\">\n";
   
   ptr +="<h1>Temperature & Humidity Status</h1>\n";

   ptr +="<div class=\"data\">\n";
   ptr +="<div class=\"side-by-side humidity-icon\">\n";
   ptr +="<svg version=\"1.1\" id=\"Layer_2\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" x=\"0px\" y=\"0px\"\n\"; width=\"12px\" height=\"17.955px\" viewBox=\"0 0 13 17.955\" enable-background=\"new 0 0 13 17.955\" xml:space=\"preserve\">\n";
   ptr +="<path fill=\"#FFFFFF\" d=\"M1.819,6.217C3.139,4.064,6.5,0,6.5,0s3.363,4.064,4.681,6.217c1.793,2.926,2.133,5.05,1.571,7.057\n";
   ptr +="c-0.438,1.574-2.264,4.681-6.252,4.681c-3.988,0-5.813-3.107-6.252-4.681C-0.313,11.267,0.026,9.143,1.819,6.217\"></path>\n";
   ptr +="</svg>\n";
   ptr +="</div>\n";
   ptr +="<div class=\"side-by-side humidity-text\">Humidity</div>\n";
   ptr +="<div class=\"side-by-side humidity\">";
   ptr +=(float)hstat;
   ptr +="<span class=\"superscript\">%</span></div>\n";
   ptr +="</div>\n";
   
   ptr +="<div class=\"data\">\n";
   ptr +="<div class=\"side-by-side temperature-icon\">\n";
   ptr +="<svg version=\"1.1\" id=\"Layer_1\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" x=\"0px\" y=\"0px\"\n";
   ptr +="width=\"9.915px\" height=\"22px\" viewBox=\"0 0 9.915 22\" enable-background=\"new 0 0 9.915 22\" xml:space=\"preserve\">\n";
   ptr +="<path fill=\"#FFFFFF\" d=\"M3.498,0.53c0.377-0.331,0.877-0.501,1.374-0.527C5.697-0.04,6.522,0.421,6.924,1.142\n";
   ptr +="c0.237,0.399,0.315,0.871,0.311,1.33C7.229,5.856,7.245,9.24,7.227,12.625c1.019,0.539,1.855,1.424,2.301,2.491\n";
   ptr +="c0.491,1.163,0.518,2.514,0.062,3.693c-0.414,1.102-1.24,2.038-2.276,2.594c-1.056,0.583-2.331,0.743-3.501,0.463\n";
   ptr +="c-1.417-0.323-2.659-1.314-3.3-2.617C0.014,18.26-0.115,17.104,0.1,16.022c0.296-1.443,1.274-2.717,2.58-3.394\n";
   ptr +="c0.013-3.44,0-6.881,0.007-10.322C2.674,1.634,2.974,0.955,3.498,0.53z\"/>\n";
   ptr +="</svg>\n";
   ptr +="</div>\n";
   ptr +="<div class=\"side-by-side temperature-text\">Celsius</div>\n";
   ptr +="<div class=\"side-by-side temperature\">";
   ptr +=(float)tstat;
   ptr +="<span class=\"superscript\">&deg;C</span></div>\n";
   ptr +="</div>\n";
   
   ptr +="<div class=\"data\">\n";
   ptr +="<div class=\"side-by-side fahrenheit-icon\">\n";
   ptr +="<svg version=\"1.1\" id=\"Layer_1\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" x=\"0px\" y=\"0px\"\n";
   ptr +="width=\"9.915px\" height=\"22px\" viewBox=\"0 0 9.915 22\" enable-background=\"new 0 0 9.915 22\" xml:space=\"preserve\">\n";
   ptr +="<path fill=\"#FFFFFF\" d=\"M3.498,0.53c0.377-0.331,0.877-0.501,1.374-0.527C5.697-0.04,6.522,0.421,6.924,1.142\n";
   ptr +="c0.237,0.399,0.315,0.871,0.311,1.33C7.229,5.856,7.245,9.24,7.227,12.625c1.019,0.539,1.855,1.424,2.301,2.491\n";
   ptr +="c0.491,1.163,0.518,2.514,0.062,3.693c-0.414,1.102-1.24,2.038-2.276,2.594c-1.056,0.583-2.331,0.743-3.501,0.463\n";
   ptr +="c-1.417-0.323-2.659-1.314-3.3-2.617C0.014,18.26-0.115,17.104,0.1,16.022c0.296-1.443,1.274-2.717,2.58-3.394\n";
   ptr +="c0.013-3.44,0-6.881,0.007-10.322C2.674,1.634,2.974,0.955,3.498,0.53z\"/>\n";
   ptr +="</svg>\n";
   ptr +="</div>\n";
   ptr +="<div class=\"side-by-side fahrenheit-text\">Fahrenheit</div>\n";
   ptr +="<div class=\"side-by-side fahrenheit\">";
   ptr +=(float)fstat;
   ptr +="<span class=\"superscript\">&deg;F</span></div>\n";
   ptr +="</div>\n";

   ptr +="<div class=\"data\">\n";
   ptr +="<div class=\"side-by-side hic-icon\">\n";
   ptr +="<svg version=\"1.1\" id=\"Layer_1\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" x=\"0px\" y=\"0px\"\n";
   ptr +="width=\"9.915px\" height=\"22px\" viewBox=\"0 0 9.915 22\" enable-background=\"new 0 0 9.915 22\" xml:space=\"preserve\">\n";
   ptr +="<path fill=\"#FFFFFF\" d=\"M3.498,0.53c0.377-0.331,0.877-0.501,1.374-0.527C5.697-0.04,6.522,0.421,6.924,1.142\n";
   ptr +="c0.237,0.399,0.315,0.871,0.311,1.33C7.229,5.856,7.245,9.24,7.227,12.625c1.019,0.539,1.855,1.424,2.301,2.491\n";
   ptr +="c0.491,1.163,0.518,2.514,0.062,3.693c-0.414,1.102-1.24,2.038-2.276,2.594c-1.056,0.583-2.331,0.743-3.501,0.463\n";
   ptr +="c-1.417-0.323-2.659-1.314-3.3-2.617C0.014,18.26-0.115,17.104,0.1,16.022c0.296-1.443,1.274-2.717,2.58-3.394\n";
   ptr +="c0.013-3.44,0-6.881,0.007-10.322C2.674,1.634,2.974,0.955,3.498,0.53z\"/>\n";
   ptr +="</svg>\n";
   ptr +="</div>\n";
   ptr +="<div class=\"side-by-side hic-text\">Heat Index</div>\n";
   ptr +="<div class=\"side-by-side hic\">";
   ptr +=(float)hicstat;
   ptr +="<span class=\"superscript\">&deg;C</span></div>\n";
   ptr +="</div>\n";

   ptr +="<div class=\"data\">\n";
   ptr +="<div class=\"side-by-side hif-icon\">\n";
   ptr +="<svg version=\"1.1\" id=\"Layer_1\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" x=\"0px\" y=\"0px\"\n";
   ptr +="width=\"9.915px\" height=\"22px\" viewBox=\"0 0 9.915 22\" enable-background=\"new 0 0 9.915 22\" xml:space=\"preserve\">\n";
   ptr +="<path fill=\"#FFFFFF\" d=\"M3.498,0.53c0.377-0.331,0.877-0.501,1.374-0.527C5.697-0.04,6.522,0.421,6.924,1.142\n";
   ptr +="c0.237,0.399,0.315,0.871,0.311,1.33C7.229,5.856,7.245,9.24,7.227,12.625c1.019,0.539,1.855,1.424,2.301,2.491\n";
   ptr +="c0.491,1.163,0.518,2.514,0.062,3.693c-0.414,1.102-1.24,2.038-2.276,2.594c-1.056,0.583-2.331,0.743-3.501,0.463\n";
   ptr +="c-1.417-0.323-2.659-1.314-3.3-2.617C0.014,18.26-0.115,17.104,0.1,16.022c0.296-1.443,1.274-2.717,2.58-3.394\n";
   ptr +="c0.013-3.44,0-6.881,0.007-10.322C2.674,1.634,2.974,0.955,3.498,0.53z\"/>\n";
   ptr +="</svg>\n";
   ptr +="</div>\n";
   ptr +="<div class=\"side-by-side hif-text\">Heat Index</div>\n";
   ptr +="<div class=\"side-by-side hif\">";
   ptr +=(float)hifstat;
   ptr +="<span class=\"superscript\">&deg;F</span></div>\n";
   ptr +="</div>\n";

  ptr +="</div>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}
  
  
//   Serial.print ("Humidity: ");
//   Serial.print (h);
//   Serial.print (" %\t");
//   Serial.print ("Temperature: ");
//   Serial.print (t);
//   Serial.print("\xC2\xB0");
//   Serial.print ("C\t ");
//   Serial.print (f);
//   Serial.print("\xC2\xB0");
//   Serial.print ("F\t");
//   Serial.print ("Heat index: ");
//   Serial.print (hic);
//   Serial.print("\xC2\xB0");
//   Serial.print ("C\t ");
//   Serial.print (hif);
//   Serial.print("\xC2\xB0");
//   Serial.println ("F ");
