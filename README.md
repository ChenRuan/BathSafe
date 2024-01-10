# BathSafe
## Introduction

Bathing or showering in a bathroom with high temperature and humidity for a long time can lead to physical discomfort or even worse. This project mainly designed an online physical device to judge the safety of users in the bathroom by obtaining ambient temperature and humidity and capturing environmental actions. With these processed data, it can termly remind people who have been in the high-temperature and high-humidity bathroom for a long time, and send messages with detailed environment information and the alert reason to emergency contacts in case of emergency.

## Functions

+ Real-time monitor the environment data and remind those in high-temperature or high-humidity environment regularly.
+ Provide emergency button that is easy to reach, which can send a message to the emergency contacts.
+ Send emergency message automatically when there is no continuous activities in the bathroom or when the high temperature or high humidity lasted for too long.

## Materials Needed

### Circuit Part
+ Adafruit Feather HUZZAH ESP8266
+ DHT22
+ LED
+ LED strip
+ PIR Motion Sensor
+ Button
+ Buzzer
+ Battery
+ Switch

### Packaging Part
+ Laser cut wooden board
+ Laser cut transparent plastic board
+ Hot glue gun

### Others
+ Mobile phone with Telegram (Need to subscribe specific channel)

## Hardware Installation Guide
### Circuit Diagram
Connect the hardware according to the Circuit Diagram. The connection between each component is shown in the figures, where red represents the live line, black represents the neutral line, yellow represents the input signal line, and blue represents the output signal line.
![BathSafeSketch](https://github.com/ChenRuan/BathSafe/assets/145383140/f97106d6-d692-4701-866f-5ccb34bd4a84)

![BathSafeSketch](https://github.com/ChenRuan/BathSafe/assets/145383140/ea6c965e-a232-4f7d-95b4-c231c4325566)

### Enclosure Design
The packaging consists of six laser cut transparent plastic shell and one laser cut wooden board. 

The tenon structure of the plastic plate is made of MakerCase[1], and the bottom of the box has no such structure, which make it easy to disassemble to change programs or replace batteries. The front hole is designed for button and the side one for switch. The two small holes at the bottom are used to exchange gas with the outside world while reducing splashing water into the box, which is used to ensure the normal operation of DHT22.

![image](https://github.com/ChenRuan/BathSafe/assets/145383140/568e6e12-47ab-44aa-814a-946b295baf21)

Multiple plastic sheets can be joined together to form a transparent box. To improve its water resistance and robustness, hot melt glue is used for pasting from board to board, and also used to fill the gaps between two exposed components and packaging.

![image](https://github.com/ChenRuan/BathSafe/assets/145383140/2822b8c4-d9af-43ba-80a2-a3806e970d71)

The wooden board is planned on the location of each component, and drilled so that the wire can pass through, so as to achieve the effect of isolating the front and back circuits to better protect the main circuit. After all the components are linked and soldered, the wooden board is fixed into plastic boxes and hot-glued.

![image](https://github.com/ChenRuan/BathSafe/assets/145383140/a4f774f9-d04f-4edc-8b4d-b0242cf9bc6a)


## Software Installation Guide

Several libraries that are written to the program header need to be installed first

First, make sure that all the libraries in the program are installed.
Then, create a file named ```secret.h``` in the same directory with the following content:
```
#define SECRET_SSID "YOUR_WIFI_NAME_HERE"
#define SECRET_PASS "YOUR_WIFI_PASSWORD_HERE" 
#define SECRET_CHATID "YOUR_TELEGRAM_ID_HERE"
#define SECRET_BOTAPITOKEN "YOUR_TELEGRAM_BOT_API_TOKEN_HERE"
```
The telegram id and bot api token content can be obtained from the following telegram module.

After that, transfer the code to the ESP8266 and run. After successfully connecting to Wifi, the LED strips will light up and the device will start working.

If the function or display effect need to be modified, refer to the following special module details:

### PIR Motion Sensor Module

This module uses the PIR Motion Sensor to obtain whether there are moving objects, and if there are, the PIRMotionCount is cleared to zero. This section in the project is to verify that the person in the bathroom is still active.
```
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
```

### Button Detection

In order to ensure the function of the button, the monitoring of the button must be carried out in real-time in loop(), and ensure that it can be regularly detected at a high frequency. After pressing the button, the device will contact the emergency contact directly.

```
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
```

### Data Process Module

This module is used to acquire and process data, including temperature and humidity data from the DHT22 and environmental activity situation from the PIR Motion Sensor. Risk counts are performed based on different environmental data, and feedback is provided when theses count exceed a certain threshold, including lighting, buzzing, and sending messages to emergency contacts.

```
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
```

### Telegram Module

The Telegram module mainly refers to [2]https://randomnerdtutorials.com/telegram-request-esp32-esp8266-nodemcu-sensor-readings/, and uses Telegram bots and subscription functions to realize information interaction between ESP8266 and mobile phones. When there is a need to send, the device will send the message directly to the specific user. The content of the message can be customized using the following function.
```
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
```

### Colorful LED Strip Display Module

This module is used to visualize temperature and humidity data in the form of the colorful light strips. The colors of the strips can be customized.

```
// Color of the LED strips
// white, green, blue, pink, yellow, orange, purple, red
int RArray[8] = {255, 170,  85,   0, 125, 250, 253, 255}; 
int GArray[8] = {239, 231, 224, 216, 176, 135,  68,   0}; 
int BArray[8] = {  0,  85, 170, 255, 255, 255, 128,   0}; 
```

```
  //Trigger the following periodically
  TempLEDStripsNumber = map(Temperature, StartTemperature, DangerTemperature, 1, 8);
  HumLEDStripsNumber = map(Humidity, StartHumidity, DangerHumidity, 1, 8);
  LEDStripsDisplay(pixels1,TempLEDStripsNumber);
  LEDStripsDisplay(pixels2,HumLEDStripsNumber);
```

```
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
```

### Buzzer Module

The buzzer is mainly used to alert the user, including a reminder when the shower is too long and feedback after pressing the emergency contact button.

## Instructions for Use

将设备安装到墙上，打开开关即可开启装置。等待装置连接wifi，连接完毕后即可开始进行温湿度的安全监测。设备将在紧急情况下自动发送紧急消息，按压按钮也可以手动发送紧急消息。

## Application Demonstration

The device can be mounted on the bathroom wall.

![8922ec5312ebe3afce2ef80c68dfd65](https://github.com/ChenRuan/BathSafe/assets/145383140/5da9253a-44cf-4453-abe1-ff45a5b805d0)

![d94f3ea2763c5a948f784be0c97771d](https://github.com/ChenRuan/BathSafe/assets/145383140/655eb56d-9dac-4981-a522-286872433c07)

When in a safe condition, the device should be displayed as shown below.

![image](https://github.com/ChenRuan/BathSafe/assets/145383140/55aae1c4-c4f1-4ba4-8a03-7599710dff19)

When the temperature is high or the humidity is high, the red light will light up.

![image](https://github.com/ChenRuan/BathSafe/assets/145383140/98635cda-f435-49e3-bbbe-8a3d0fdb8fbb)

If the application sends emergency messages, the Telegram app will receive like this.

![image](https://github.com/ChenRuan/BathSafe/assets/145383140/f478f332-7233-4cfd-8706-afd688db9437)


# References
[1] MakerCase - https://en.makercase.com/
[2] Telegram: Request ESP32/ESP8266 Sensor Readings (Arduino IDE) - https://randomnerdtutorials.com/telegram-request-esp32-esp8266-nodemcu-sensor-readings/
