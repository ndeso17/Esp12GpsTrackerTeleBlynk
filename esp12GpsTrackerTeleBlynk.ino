#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "CTBot.h";
CTBot myBot ;
//pesan telegram Attention
String peringatan1 = "Siapa anda? Jangan coba - coba masuk, atau anda akan berurusan dengan pihak kepolisian...!!!";
String peringatan2 = "Kami telah mencatat ID Telegram dan Username anda, kami akan segera memproses ke jalur hukum jika anda memaksa masuk dalam bot ini.";
//GPS Wiring
static const int RXPin = 4, TXPin = 5;   // GPIO 4=D2(conneect Tx of GPS) and GPIO 5=D1(Connect Rx of GPS
static const uint32_t GPSBaud = 9600; //if Baud rate 9600 didn't work in your case then use 4800
TinyGPSPlus gps; // The TinyGPS++ object
WidgetMap myMap(V0);  // V0 for virtual pin of Map Widget
SoftwareSerial ss(RXPin, TXPin);  // The serial connection to the GPS device
BlynkTimer timer;
//Gps Algoritma
float spd;       //Variable  to store the speed
float sats;      //Variable to store no. of satellites response
String bearing;  //Variable to store orientation or direction of GPS
String lat_str , lng_str;
//Koneksi internet, telegram,dan blynk
char auth[] = "OepxUjy84GLoR-jliayuWKiIq9Funv3V";              //Your Project authentication key
char ssid[] = "KamarInternet";                                       // Name of your network (HotSpot or Router name)
char pass[] = "1234567895";                                      // Corresponding Password
String token = "5088136042:AAGXrAlAdddnTv6H-wAEcDOQI9OcaOVdtrU";
const int id = 5049828136 ; //id User Telegram yang dapat mengakses bot
//relay config
static const uint8_t relayMesin   = D5;
static const uint8_t relayAlarm   = D6;
//unsigned int move_index;         // moving index, to be used later
unsigned int move_index = 1;       // fixed location for now

void setup()
{
  Serial.println("Starting System. Connecting to WiFi");
  Serial.begin(115200);
  Serial.println();
  ss.begin(GPSBaud);
  myBot.wifiConnect(ssid, pass);
    //telegram config
  myBot.setTelegramToken(token);
  if(myBot.testConnection())
    Serial.println("Connencting Succesfully. System Running");
  else
    Serial.println("Error");
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(5000L, checkGPS); // every 5s check if GPS is connected, only really needs to be done once
}

void checkGPS(){
  if (gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
      Blynk.virtualWrite(V4, "GPS ERROR");  // Value Display widget  on V4 if GPS not detected
  }
}

void loop()
{
    while (ss.available() > 0) 
    {
      // sketch displays information every time a new sentence is correctly encoded.
      if (gps.encode(ss.read()))
        displayInfo();
  }
  String link = "https://www.google.com/maps/place/"+lat_str+","+lng_str;
  //Telegram Data Perintah
  TBMessage msg ;
  static const int idp = msg.sender.id;
  String kodtlp = msg.sender.languageCode;
  if(myBot.getNewMessage(msg)){
  Serial.println("Pesan Masuk : " + msg.text);
  
  String  pesan = msg.text;
  if(pesan == "KONTAK OFF"){
    if(msg.sender.id == id){
      myBot.sendMessage(id, "Motor dipateni");
      digitalWrite(relayMesin, HIGH);
    }else{
      myBot.sendMessage(id, "Ada penyusup...!");
      myBot.sendMessage(id, "Username penyusup : " + msg.sender.username);
      myBot.sendMessage(id, "Nama penyusup : " + msg.sender.firstName + msg.sender.lastName);
      myBot.sendMessage(msg.sender.id, peringatan1);
      myBot.sendMessage(msg.sender.id, peringatan2);
    }
  }else if(pesan == "KONTAK ON"){
    if(msg.sender.id == id){
      myBot.sendMessage(id, "Motor urip");
      digitalWrite(relayMesin, LOW);
    }else{
      myBot.sendMessage(id, "Ada penyusup...!");
      myBot.sendMessage(id, "Username penyusup : " + msg.sender.username);
      myBot.sendMessage(id, "Nama penyusup : " + msg.sender.firstName + msg.sender.lastName);
      myBot.sendMessage(msg.sender.id, peringatan1);
      myBot.sendMessage(msg.sender.id, peringatan2);
    }
  }else if(pesan == "Posisi?"){
    if(msg.sender.id == id){
      myBot.sendMessage(id, "Posisi Motor ana ning : " + link);
    }else{
      myBot.sendMessage(id, "Ada penyusup");
    }
  }else if(pesan == "SOS ON"){
    if(msg.sender.id == id){
      myBot.sendMessage(id, "Alarm Urip");
      digitalWrite(relayAlarm, LOW);
    }else{
      myBot.sendMessage(id, "Ada penyusup...!");
      myBot.sendMessage(id, "Username penyusup : " + msg.sender.username);
      myBot.sendMessage(id, "Nama penyusup : " + msg.sender.firstName + msg.sender.lastName);
      myBot.sendMessage(msg.sender.id, peringatan1);
      myBot.sendMessage(msg.sender.id, peringatan2);
    }
  }else if(pesan == "SOS OFF"){
    if(msg.sender.id == id){
      myBot.sendMessage(id, "Alarm Mati");
      digitalWrite(relayAlarm, HIGH);
    }else{
      myBot.sendMessage(id, "Ada penyusup...!");
      myBot.sendMessage(id, "Username penyusup : " + msg.sender.username);
      myBot.sendMessage(id, "Nama penyusup : " + msg.sender.firstName + msg.sender.lastName);
      myBot.sendMessage(msg.sender.id, peringatan1);
      myBot.sendMessage(msg.sender.id, peringatan2);
    }
  }else if(pesan == msg.text){
     myBot.sendMessage(id, "Ada penyusup...!!!" );
     myBot.sendMessage(id, "Username penyusup : " + msg.sender.username);
     myBot.sendMessage(id, "Nama penyusup : " + msg.sender.firstName + msg.sender.lastName);
     myBot.sendMessage(msg.sender.id, "Anda penyusup...!!!");
     myBot.sendMessage(msg.sender.id, peringatan1);
     myBot.sendMessage(msg.sender.id, peringatan2);
     myBot.sendMessage(msg.sender.id, "Username Anda adalah : " + msg.sender.username);
     myBot.sendMessage(msg.sender.id, "Nama Anda adalah : " + msg.sender.firstName + msg.sender.lastName);
  }
 }
  Blynk.run();
  timer.run();
  Serial.println("Latitude = "+lat_str + " longitude = "+lng_str);
}

void displayInfo()
{ 
  if (gps.location.isValid() ) 
  {    
    float latitude = (gps.location.lat());     //Storing the Lat. and Lon. 
    float longitude = (gps.location.lng());
    lat_str = String(latitude , 6); 
    lng_str = String(longitude , 6);
    
    Serial.print("LAT:  ");
    Serial.println(latitude, 6);  // float to x decimal places
    Serial.print("LONG: ");
    Serial.println(longitude, 6);
    Blynk.virtualWrite(V1, String(latitude, 6));   
    Blynk.virtualWrite(V2, String(longitude, 6));  
    myMap.location(move_index, latitude, longitude, "GPS_Location");
    spd = gps.speed.kmph();               //get speed
    Blynk.virtualWrite(V3, spd);
    sats = gps.satellites.value();    //get number of satellites
    Blynk.virtualWrite(V4, sats);
    bearing = TinyGPSPlus::cardinal(gps.course.value()); // get the direction
    Blynk.virtualWrite(V5, bearing);                   
  }
  
 Serial.println();
}
