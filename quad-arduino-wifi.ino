
// 舵机与 esp32 引脚接线图, 数据口方向为后 (tail)
// 
//     前 (head)
//         -----               -----
//         |  2  |             |  3  |
//         |pin25|             |Pin18|
//         ----- -----   ----- -----
//             |  0  | |  1  |
//             |Pin12| |Pin16|
//              -----   -----
//             |  4  | |  5  |
//             |Pin13| |Pin17|
//         ----- -----   ----- -----
//         |  6  |             |  7  |
//         |Pin26|             |Pin19|
//         -----               -----
//     后 (tail)

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include <ArduinoJson.h>
#include "minikame.h"

// ESP8266WebServer esp8266_server(80);// 建立网络服务器对象，该对象用于响应HTTP请求。监听端口（80）

const int led = 2;

// WiFi连接信息
const char *ssid = "小亦站";
const char *password = "88889999";

// 设置静态 IP 配置
IPAddress local_ip(192, 168, 2, 192);  // 设置固定 IP 地址
IPAddress gateway(192, 168, 2, 1);     // 路由器网关地址
IPAddress subnet(255, 255, 255, 0);    // 子网掩码

WebServer server(80);

MiniKame robot;
bool running = 0;
int input = 0;

DynamicJsonDocument jsonDoc(1024);

String indexHtml = "<!DOCTYPE html><!DOCTYPE html><html><head>    <meta charset='utf-8'>    <title>OttoArduino</title>    <style>        body {            margin: 0;            padding: 0;            display: flex;            justify-content: center;            align-items: center;        }        .container {            display: flex;            flex-wrap: wrap;            justify-content: center;            align-items: center;            max-width: 100%;            padding: 50px;        }        .left {            display: flex;            flex-direction: column;            justify-content: center;            align-items: center;            max-width: 100%;            margin: 30px;        }        .right {            display: grid;            grid-template-columns: repeat(3, 1fr);            gap: 5px;            margin: 30px;        }        .top-row {            display: flex;            justify-content: center;            align-items: center;        }        .middle-row {            display: flex;            justify-content: space-between;            align-items: center;            width: 100%;            margin-top: 10px;            margin-bottom: 10px;        }        .bottom-row {            display: flex;            justify-content: center;            align-items: center;        }        .button {            background-color: #f0f0f0;            border: 1px solid #ccc;            margin: 5px;            display: flex;            justify-content: center;            align-items: center;            border-radius: 10px;            cursor: pointer;        }        .left .button {            width: 150px;            height: 150px;            font-size: 60px;        }        .right .button {            width: 220px;            height: 100px;            font-size: 26px;        }    </style></head><body>    <div class='container'>        <div class='left'>            <div class='button' id=\"forward\">▴</div>            <div class='middle-row'>                <div class='button direction' id=\"turn_L\">◂</div>                <div class='button direction' id=\"home\">●</div>                <div class='button direction' id=\"turn_R\">▸</div>            </div>            <div class='button' id=\"backward\">▾</div>        </div>        <div class='right'>            <div class='button' id='hello'>Hello</div>            <div class='button' id='omni_walk'>OmniWalk</div>            <div class='button' id='moonwalk_L'>Moonwalk</div>            <div class='button' id='dance'>Dance</div>            <div class='button' id='up_down'>UpDown</div>            <div class='button' id='push_up'>PushUp</div>            <div class='button' id='front_back'>FrontBack</div>            <div class='button' id='wave_hand'>WaveHand</div>            <div class='button' id='hide'>Hide</div>            <div class='button' id='scared'>Scared</div>        </div>    </div>    <script >        function buttonClick(c) {            fetch('/control', {                method: 'POST',                headers: {                    'Content-Type': 'application/json'                },                body: JSON.stringify({ command: c })            })            .then(response => response.json())            .then(data => console.log(data))            .catch(error => console.log(error))}        document.querySelectorAll('.button').forEach(button => {            button.addEventListener('click', () => buttonClick(button.id));            console.log(button.id)        });    </script></body></html>";


void setup(void) {

  Serial.begin(9600);  // 启动串口通讯

  // WiFi.softAP(ssid,password); //建立热点
  //
  // Serial.print("Access Point: ");
  // Serial.println(ssid);
  // Serial.print("IP address: ");
  // Serial.println(WiFi.softAPIP());

  // 设置静态 IP 地址
  if (!WiFi.config(local_ip, gateway, subnet)) {
    Serial.println("配置静态 IP 失败!");
  }

  // 连接到 Wi-Fi 网络
  Serial.print("连接到 WiFi网络: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  // 等待连接
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  // 输出连接的 IP 地址
  Serial.println();
  Serial.print("已连接到 WiFi, IP 地址：");
  Serial.println(WiFi.localIP());

  server.begin();                        // 启动网站服务
  server.on("/", HTTP_GET, handleRoot);  // 设置服务器根目录即'/'的函数'handleRoot'
  // server.on("/m", HTTP_GET, m);
  server.on("/control", HTTP_POST, handlePost);
  server.onNotFound(handleNotFound);  // 设置处理404情况的函数'handleNotFound'

  Serial.println("HTTP server started");  //  告知用户ESP32网络服务功能已经启动
  delay(10);
  robot.init();
}

void loop() {
  server.handleClient();
}


void handleRoot() {
  digitalWrite(led, 0);
  // server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/html", indexHtml);
  digitalWrite(led, 1);
}

void handlePost() {
  // server.sendHeader("Access-Control-Allow-Origin", "*");
  String postBody = server.arg("plain");
  Serial.println(postBody);
  jsonDoc.clear();

  DeserializationError error = deserializeJson(jsonDoc, postBody);

  if (error) {
    server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Invalid JSON\"}");
    return;
  }

  const char *command = jsonDoc["command"];

  // 处理命令
  if (command) {
    Serial.print("Received command: ");
    Serial.println(command);

    bool commandProcessed = processCommand(command);

    if (commandProcessed) {
      server.send(200, "application/json", "{\"status\":\"success\",\"message\":\"Command executed\"}");
    } else {
      server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Unknown command\"}");
    }
  } else {
    server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"No command specified\"}");
  }

  server.send(200, "application/json", "{\"status\":\"success\",\"message\":\"Command received\"}");


  // server send json data
  server.send(200, "application/json", "{\"status\": \"ok\"}");
}

bool processCommand(const char* command) {
  String cmd = String(command);
  
  if (cmd == "forward") {
    robot.walk(2, 1000);
    return true;
  } 
  else if (cmd == "backward") {
    // robot.backward(2, 1000);
    return true;
  }
  else if (cmd == "stop") {
    // robot.stop();
    return true;
  }
  else if (cmd == "turn_L") {
    robot.turnL(2, 1000);
    return true;
  }
  else if (cmd == "turn_R") {
    robot.turnR(2, 1000);
    return true;
  }
  else {
    // 未知命令
    return false;
  }
}

void m() {
  String ip = WiFi.localIP().toString().c_str();
  server.sendHeader("Location", " / ");  // 跳转回页面根目录
  server.send(303);
  if (server.arg("x1") == "1") {
    input = 1;
  }
  if (server.arg("x1") == "2") {
    input = 2;
  }
  if (server.arg("x1") == "3") {
    input = 3;
  }
  if (server.arg("x1") == "4") {
    input = 4;
  }
  if (server.arg("x1") == "5") {
    input = 5;
  }
  if (server.arg("x1") == "6") {
    input = 6;
  }
  if (server.arg("x1") == "7") {
    input = 7;
  }
  if (server.arg("x1") == "8") {
    input = 8;
  }
  if (server.arg("x1") == "9") {
    input = 9;
  }
  if (server.arg("x1") == "10") {
    input = 10;
  }
  if (server.arg("x1") == "11") {
    input = 11;
  }
  if (server.arg("x1") == "12") {
    input = 12;
  }
  if (server.arg("x1") == "13") {
    input = 13;
  }
}

// 设置处理404情况的函数'handleNotFound'
void handleNotFound() {
  server.send(404, "text / plain", "404: Not found");  // 发送 HTTP 状态 404 (未找到页面) 并向浏览器发送文字 "404: Not found"
}
