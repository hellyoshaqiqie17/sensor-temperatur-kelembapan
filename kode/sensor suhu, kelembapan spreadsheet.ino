#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include "DHT.h"
#define DHTTYPE DHT11 // tipe sensor
const int DHTPin = D2; //sambungan sensor ke pin d2
DHT dht(DHTPin, DHTTYPE); //inisiasi

#define Led_kontrol 2  //led kelap kelip

const char* username_wifi = ",,"; //username_wifi wifi
const char* password_wifi_user = "hahaha111"; //password_wifi_usere


//----------------------------------------
const char* host = "script.google.com";
const int httpsPort = 443;
//----------------------------------------

WiFiClientSecure client; //gae objek wificlient

String GAS_ID = "AKfycbxo1ir9FN26eUsuAfLekrYoaUVlWZTJlEQn7nzpP7YTEO4rTLOd3b8v4kyahrrZwLeRfg"; //spreadsheet script ID

void setup() {
 
  Serial.begin(115200);
  delay(500);

  dht.begin();  //start baca sensor dht11
  delay(500);
  
  WiFi.begin(username_wifi, password_wifi_user); //inisiasi konek wifi
  Serial.println("");
    
  pinMode(Led_kontrol,OUTPUT); 
  digitalWrite(Led_kontrol, HIGH); //led mati
  //----------------------------------------
  Serial.print("menyambungkan");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    //---------------------------------------flash led
  
    digitalWrite(Led_kontrol, LOW);
    delay(250);
    digitalWrite(Led_kontrol, HIGH);
    delay(250);
    //----------------------------------------
  }
  //----------------------------------------
  digitalWrite(Led_kontrol, HIGH); //led mati jika terkoneksi dengan wifi
  Serial.println("");
  Serial.print("dapat menyambung : ");
  Serial.println(username_wifi);
  Serial.print("alamat ip: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  //----------------------------------------

  client.setInsecure();
}

void loop() {
  // baca temperatur dan kelembapan 250 milidetik
  
  int k = dht.readkelembapan();
  
  float t = dht.readtemperatur();
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(k) || isnan(t)) {
    Serial.println("tidak dapat membaca sensor");
    delay(500);
    return;
  }
  String temperaturp = "temperatur : " + String(t) + " °C";
  String kelembapani = "kelembapan : " + String(k) + " %";
  Serial.println(temperatur);
  Serial.println(kelembapan);
  
  sendData(t, k); 
}

// Subroutine for sending data to Google Sheets
void sendData(float temperatur, int kelembapan) {
  Serial.println("==========");
  Serial.print("connecting to ");
  Serial.println(host);
  
  //----------------------------------------menyambungkan ke google host yaitu
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }
  //----------------------------------------

  //---------------------------------------proses data pengiriman data
  String string_temperaturperatur =  String(temperatur);

  String string_kelembapan =  String(kelembapan, DEC); 
  String url = "/macros/s/" + GAS_ID + "/exec?temperatur=" + string_temperatur + "&kelembapan=" + string_kelembapan;
  Serial.print("url: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
         "Host: " + host + "\r\n" +
         "User-Agent: BuildFailureDetectorESP8266\r\n" +
         "Connection: close\r\n\r\n");

  Serial.println("request terkirm");
  //----------------------------------------

  //----------------------------------------Chek data sudah terkirim atau belum
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("sukses");
  } else {
    Serial.println("gagal");
  }
  Serial.print("reply was : ");
  Serial.println(line);
  Serial.println("koneksi gagal");
  Serial.println("==========");
  Serial.println();
  //----------------------------------------
} 
//==============================================================================