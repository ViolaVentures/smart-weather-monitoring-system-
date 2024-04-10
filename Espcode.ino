#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include "DHT.h"
#define DHTTYPE DHT11 // type of the temperature sensor
const int DHTPin = 5; //--> The pin used for the DHT11 sensor is Pin D1 = GPIO5
const int capteur_D = 4;// rain
DHT dht(DHTPin, DHTTYPE); //--> Initialize DHT sensor, DHT dht(Pin_used, Type_of_DHT_Sensor);

#define ON_Board_LED 2  //--> Defining an On Board LED, used for indicators when the process of connecting to a wifi router

//MQ7
float RS_gas = 0;
float ratio = 0;
float sensorValue = 0;
float sensor_volt = 0;
float R0 = 35235.48;


const char* ssid = "Jnana's Galaxy M32"; //--> Your wifi name or SSID.
const char* password = "fjqb5196"; //--> Your wifi password.


//----------------------------------------Host & httpsPort
const char* host = "script.google.com";
const int httpsPort = 443;
//----------------------------------------

WiFiClientSecure client; //--> Create a WiFiClientSecure object.

String GAS_ID = "AKfycbxa8PETASBUntRoZ2u4WS67Rdvp3Z8IxFw_PUuhMOKcK1CL-sQlCpNIVGvaDP9axmM"; //--> spreadsheet script ID

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(500);

   pinMode(capteur_D, INPUT);//rain
  dht.begin();  //--> Start reading DHT11 sensors
  delay(500);
  
  WiFi.begin(ssid, password); //--> Connect to your WiFi router
  Serial.println("");
    
  pinMode(ON_Board_LED,OUTPUT); //--> On Board LED port Direction output
  digitalWrite(ON_Board_LED, HIGH); //--> Turn off Led On Board

  //----------------------------------------Wait for connection
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    //----------------------------------------Make the On Board Flashing LED on the process of connecting to the wifi router.
    digitalWrite(ON_Board_LED, LOW);
    delay(250);
    digitalWrite(ON_Board_LED, HIGH);
    delay(250);
    //----------------------------------------
  }
  //----------------------------------------
  digitalWrite(ON_Board_LED, HIGH); //--> Turn off the On Board LED when it is connected to the wifi router.
  Serial.println("");
  Serial.print("Successfully connected to : ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  //----------------------------------------

  client.setInsecure();
}

void loop() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  int h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  //MQ7
  sensorValue = analogRead(A0);
   sensor_volt = sensorValue/1024*5.0;
   RS_gas = (5.0-sensor_volt)/sensor_volt;
   ratio = RS_gas/R0; //Replace R0 with the value found using the sketch above
   float mx = 1538.46 * ratio;
   float mppm = pow(mx,-1.709);

   //rain
   bool r;
   r=digitalRead(capteur_D);
   {
if(r== LOW) 
  {
    Serial.println("Digital value : wet"); 
    delay(10); 
  }
else
  {
    Serial.println("Digital value : dry");
    delay(10); 
  }
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor !");
    delay(500);
    return;
  }
  String Temp = "Temperature : " + String(t) + " °C";
  String Humi = "Humidity : " + String(h) + " %";
  String mpp = "MQ7 : " + String(mppm) + " ppm";
  String rain ="Rain : " + String(r) ;
  Serial.println(Temp);
  Serial.println(Humi);
  Serial.println(mpp);
  Serial.println(rain);
   }
  sendData(t, h,mppm,r); //--> Calls the sendData Subroutine
}

// Subroutine for sending data to Google Sheets
void sendData(float tem, int hum, float mp,bool ra ) {
  Serial.println("==========");
  Serial.print("connecting to ");
  Serial.println(host);
  
  //----------------------------------------Connect to Google host
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }
  //----------------------------------------

  //----------------------------------------Processing data and sending data
  String string_temperature =  String(tem);
  // String string_temperature =  String(tem, DEC); 
  String string_humidity =  String(hum, DEC);
  String string_CO =  String(mp, DEC);
  String string_Rain =  String(ra); 
  String url = "/macros/s/" + GAS_ID + "/exec?temperature=" + string_temperature + "&humidity=" + string_humidity+"&MQ7=" + string_CO+ "&Rain=" + string_Rain;
  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
         "Host: " + host + "\r\n" +
         "User-Agent: BuildFailureDetectorESP8266\r\n" +
         "Connection: close\r\n\r\n");

  Serial.println("request sent");
  //----------------------------------------

  //----------------------------------------Checking whether the data was sent successfully or not
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("esp8266/Arduino CI successfull!");
  } else {
    Serial.println("esp8266/Arduino CI has failed");
  }
  Serial.print("reply was : ");
  Serial.println(line);
  Serial.println("closing connection");
  Serial.println("==========");
  Serial.println();
  delay(6000);
  //----------------------------------------
} 
//==============================================================================