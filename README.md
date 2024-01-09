# BathSafe
## Introduction
The production is a multi-sensor device which can avoid the risks caused by high temperature and humidity during bathing,

## Functions

+ Real-time monitor the environment data and remind those in high-temperature or high-humidity environment regularly.
+ Provide emergency button that is easy to reach, which can send a message to the emergency contacts.
+ Send emergency message automatically when there is no continuous activities in the bathroom or when the high temperature or high humidity lasted for too long.

![image](https://github.com/ChenRuan/BathSafe/assets/145383140/f478f332-7233-4cfd-8706-afd688db9437)


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
Connect the hardware according to the Circuit Diagram.
![image](https://github.com/ChenRuan/BathSafe/assets/145383140/8d452b1f-f2b8-462e-a0aa-6742acee5927)

![image](https://github.com/ChenRuan/BathSafe/assets/145383140/502d494e-ae0d-468b-bac7-ee0e2ce7c53a)

### Packaging Production
The packaging consists of six laser cut transparent plastic shell and one laser cut wooden board. 

The tenon structure of the plastic plate is made of MakerCase[1], and the bottom of the box has no such structure, which make it easy to disassemble to change programs or replace batteries. The front hole is designed for button and the side one for switch. The two small holes at the bottom are used to exchange gas with the outside world while reducing splashing water into the box, which is used to ensure the normal operation of DHT22.

![image](https://github.com/ChenRuan/BathSafe/assets/145383140/568e6e12-47ab-44aa-814a-946b295baf21)

Multiple plastic sheets can be joined together to form a transparent box. To improve its water resistance and robustness, hot melt glue is used for pasting from board to board, and also used to fill the gaps between two exposed components and packaging.

**!!! picture here !!!**

The wooden board is planned on the location of each component, and drilled so that the wire can pass through, so as to achieve the effect of isolating the front and back circuits to better protect the main circuit. After all the components are linked and soldered, the wooden board is fixed into plastic boxes and hot-glued.

**!!! picture here !!!**

## Software Installation Guide

Several libraries that are written to the program header need to be installed first

首先，需要确保程序中所有库都已经被安装。
然后，请在同一目录下创建名为secret.h的文件，内容为：
```
#define SECRET_SSID "YOUR_WIFI_NAME_HERE"
#define SECRET_PASS "YOUR_WIFI_PASSWORD_HERE" 
#define SECRET_CHATID "YOUR_TELEGRAM_ID_HERE"
#define SECRET_BOTAPITOKEN "YOUR_TELEGRAM_BOT_API_TOKEN_HERE"
```
其中telegram的id和bot api token内容可以从下文telegram模块中获取使用教程

在这之后，将代码传输到ESP8266上运行。在成功连接Wifi后，灯带即会亮起，设备开始工作。

如果需要修改功能或者显示效果，可以参考以下特别的模块的详细介绍：

### Data Process Module

### Telegram Module

### Colorful LED Strip Display Module

### PIR Motion Sensor Module

### Buzzer Module

## Instructions for Use

## Application Demonstration
电池问题！
switch

# References
[1] MakerCase - https://en.makercase.com/
