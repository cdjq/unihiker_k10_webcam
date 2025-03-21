/*!
 * @file webcam.ino
 * @brief 这是一个网络摄像头的示例，运行示例代码连接wifi串口输出IP地址，网页上输入
 * @n  IP地址将显示摄像头画面。
 * @copyright   Copyright (c) 2025 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license     The MIT License (MIT)
 * @author [TangJie](jie.tang@dfrobot.com)
 * @version  V1.0
 * @date  2025-03-21 
 * @url https://github.com/DFRobot/unihiker_k10_webcam
 */

#include "unihiker_k10_webcam.h"
#include "unihiker_k10.h"
#include <WiFi.h>

UNIHIKER_K10 k10;
unihiker_k10_webcam webcam;


// WiFi 配置
const char* SSID = "your_wifi_name";
const char* PASSWORD = "your_wifi_password";

// 连接 WiFi
void wifi_init() {
    WiFi.begin(SSID, PASSWORD);
    Serial.print("Connecting to WiFi");

    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }

    Serial.println("\n WiFi Connected!");
    Serial.print("ESP32 IP: ");
    Serial.println(WiFi.localIP());
}

void setup()
{
    Serial.begin(115200);
    
    //初始化K10
    k10.begin();

    //初始化显示
    k10.initScreen();
    wifi_init();
    //初始化摄像头显示在屏幕上
    k10.initBgCamerImage();

}

void loop()
{
   //开启网络摄像头
  Serial.println("enableWebcam");
  webcam.enableWebcam();
  delay(60 * 1000 * 2);
  //关闭网络摄像头
  Serial.println("disableWebcam");
  webcam.disableWebcam();
  delay(60 * 1000 * 2);
}