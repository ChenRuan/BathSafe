#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ezTime.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h>
// Libraries for Telegram Bot
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <Adafruit_NeoPixel.h>

#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

// Sensors - DHT22 and Nails
uint8_t DHTPin = 12;        // on Pin 2 of the Huzzah
uint8_t LEDStrip1Pin = 13;  
uint8_t LEDStrip2Pin = 15; 
uint8_t LEDREDPin = 0; 
uint8_t buzzerPin = 4;
uint8_t PIRMotionPin = 5;
uint8_t buttonPin = 14;
float Temperature;
float Humidity;
int ReminderCount=0;
int DangerCount=0;
int PIRMotionSituation;
int PIRMotionCount;

DHT dht(DHTPin, DHTTYPE);   // Initialize DHT sensor.

// Color of the LED strips
// white, green, blue, pink, yellow, orange, purple, red
int RArray[8] = {255, 170,  85,   0, 125, 250, 253, 255}; 
int GArray[8] = {239, 231, 224, 216, 176, 135,  68,   0}; 
int BArray[8] = {  0,  85, 170, 255, 255, 255, 128,   0}; 

float DangerTemperature = 45.0;
float DangerHumidity = 80;

Adafruit_NeoPixel pixels1(8, LEDStrip1Pin);
Adafruit_NeoPixel pixels2(8, LEDStrip2Pin);

// Wifi and TelegramBot
#include "arduino_secrets.h" 
/*
**** please enter your sensitive data in the Secret tab/arduino_secrets.h
**** using format below

#define SECRET_SSID "ssid name"
#define SECRET_PASS "ssid password"
#define SECRET_CHATID "botID";
#define SECRET_BOTAPITOKEN "botAPItoken";
 */

const char* ssid     = SECRET_SSID;
const char* password = SECRET_PASS;
const char* CHAT_ID  = SECRET_CHATID;
const char* BOTtoken = SECRET_BOTAPITOKEN;

ESP8266WebServer server(80);
WiFiClient espClient;

// Date and time
Timezone GB;

// Telegram Bot Part
X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure BOTClient;
UniversalTelegramBot bot(BOTtoken, BOTClient);
int warningCount = 720;

int botRequestDelay = 1000;
unsigned long lastTimeBotRan;
unsigned long StartTime;
unsigned long PreviousTime;

int TempLEDStripsNumber=7;
int HumLEDStripsNumber=7;
int StartTemperature;
int StartHumidity;
int buttonStatus = 0;

void setup() {
  // setup application Pins
  pinMode(buzzerPin, OUTPUT);
  pinMode(LEDREDPin, OUTPUT);
  pinMode(PIRMotionPin, INPUT);
  pinMode(buttonPin, INPUT);

  // open serial connection for debug info
  Serial.begin(115200);
  delay(100);

  // start DHT sensor
  pinMode(DHTPin, INPUT);
  dht.begin();

  // run initialisation functions
  startWifi();
  syncDate();

  // setup telegram bot
  configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
  BOTClient.setTrustAnchors(&cert); // Add root certificate for api.telegram.org

  pixels1.begin();
  pixels2.begin();
  pixels1.setBrightness(2);
  pixels2.setBrightness(2);
  Temperature = dht.readTemperature();
  Humidity = dht.readHumidity(); // Gets the values of the humidity
  StartTemperature = 15;
  StartHumidity = 20;
  //StartTemperature = dht.readTemperature();
  //StartHumidity = dht.readHumidity();
}

void loop() {
  StartTime = millis();
  if (StartTime - PreviousTime >= 5000) {
    PreviousTime = StartTime;
    DataProcess();
  }
  buttonStatus = !digitalRead(buttonPin);
  if (buttonStatus == 1){
    BathWarning(2);
    Serial.println("Danger!");
  }
  delay(100);
}

void startWifi() {
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  // check to see if connected and wait until you are
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void syncDate() {
  // get real date and time
  waitForSync();
  Serial.println("UTC: " + UTC.dateTime());
  GB.setLocation("Europe/London");
  Serial.println("London time: " + GB.dateTime());
}

// happens when bot recieves new message
void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    //Serial.println(chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    
    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;
  }
}

void SendWarningMessage(int MessageMode){
  int numMinute = StartTime/60000;
  int numSecond = StartTime/1000 - numMinute * 60; 
  String BOTinfo = "Who in the bathroom may be in danger! Please check!";
  BOTinfo += "\nBath time: " + String(numMinute) + ":" + String(numSecond);
  BOTinfo += "\nTemperature: " + String(Temperature);
  BOTinfo += "\nHumidity: " + String(Humidity);
  BOTinfo += "\nWarning Reason: ";
  // High temp and hum for too long 
  if (MessageMode == 0){
    BOTinfo += "Bath time in high temperature/humidity is TOO long! Please pay attention to the condition of who in the bathroom!";
  }
  // No activity
  if (MessageMode == 1){
    BOTinfo += "No activity in the bathroom for a long time! Please enter the bathroom and check the situation!";
  }
  // Button event
  if (MessageMode == 2){
    BOTinfo += "Emergency! Someone in the bathroom press the emergency button! Please check asap!";
  }
  bot.sendMessage(CHAT_ID, BOTinfo, ""); 
}

void DataProcess(){
  delay(300);
  // Read temperature and humidity
  Temperature = dht.readTemperature();
  Humidity = dht.readHumidity();
  // map the data to the number of LEDs
  TempLEDStripsNumber = map(Temperature, StartTemperature, DangerTemperature, 1, 8);
  HumLEDStripsNumber = map(Humidity, StartHumidity, DangerHumidity, 1, 8);
  LEDStripsDisplay(pixels1,TempLEDStripsNumber);
  LEDStripsDisplay(pixels2,HumLEDStripsNumber);
  Serial.print("Temperature:");
  Serial.println(Temperature);
  Serial.print("LEDnumber:");
  Serial.println(TempLEDStripsNumber);
  Serial.print("Humidity:");
  Serial.println(Humidity);
  Serial.print("LEDnumber:");
  Serial.println(HumLEDStripsNumber);
  PIRMotionJudge();
  // high heat index warning
  float HeatIndex = 1.1 * (1.8 * Temperature + 32) + 0.047 * Humidity - 10.3 ;
  if(Temperature >= DangerTemperature or Humidity >= DangerHumidity or HeatIndex >= 125){
    ReminderCount ++ ;
    if(ReminderCount >= 60){
      buzzerReminding();
      ReminderCount = 0;
      DangerCount ++;
      if (DangerCount >= 4){
        BathWarning(0);
      }else if(PIRMotionCount >= 60){
        BathWarning(1);
      }
    }
    digitalWrite(LEDREDPin, HIGH);
  } else{
    digitalWrite(LEDREDPin, LOW);
  }
  Serial.print("ReminderCount:");
  Serial.println(ReminderCount);
  Serial.print("DangerCount:");
  Serial.println(DangerCount);
  Serial.print("PIRMotionCount:");
  Serial.println(PIRMotionCount);
  Serial.println("-------------");
}

// Activitate the chosen LED strips and light up specific number of LEDs
void LEDStripsDisplay(Adafruit_NeoPixel &pixels, int LEDnumber){
  pixels.clear();
  if(LEDnumber >= 1000){
    LEDnumber = 1;
  }
  int minNum = (LEDnumber < 8) ? LEDnumber : 8 ;
  for(int i=0; i < minNum; i++) {
    pixels.setPixelColor(i, RArray[i], GArray[i], BArray[i]);
  }
  pixels.show();
}

void buzzerReminding(){
  tone(buzzerPin, 1000, 1000);
  delay(1000);
  tone(buzzerPin, 1000, 1000);
  delay(1000);
} 

void BathWarning(int WarningMode){
  tone(buzzerPin, 1000, 60000); 
  SendWarningMessage(WarningMode);
} 

void PIRMotionJudge(){
  PIRMotionSituation = digitalRead(PIRMotionPin);
  Serial.print("PIR:");
  Serial.println(PIRMotionSituation);
  if (PIRMotionSituation == 0){
    PIRMotionCount++;
  }else{
    PIRMotionCount = 0;
  }
}
