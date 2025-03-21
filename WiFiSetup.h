#ifndef WIFI_SETUP_H
#define WIFI_SETUP_H
#include "Config.h"
#include <WiFi.h>

// 来自Config.h的配置会在这里使用
extern const bool USE_AP_MODE;

// STA模式配置
extern const char *ssid;
extern const char *password;
extern IPAddress local_ip;
extern IPAddress gateway;
extern IPAddress subnet;

// AP模式配置
extern const char *ap_ssid;
extern const char *ap_password;
extern IPAddress ap_local_ip;
extern IPAddress ap_gateway;
extern IPAddress ap_subnet;

// 启动AP模式
bool setupAP() {
  Serial.println("正在启动AP模式...");

  // 切换到AP模式
  WiFi.mode(WIFI_AP);
  delay(100);

  // 配置AP静态IP
  if (!WiFi.softAPConfig(ap_local_ip, ap_gateway, ap_subnet)) {
    Serial.println("AP模式配置静态IP失败!");
    return false;
  }

  // 启动AP，指定加密模式为WPA2-PSK
  if (!WiFi.softAP(ap_ssid, ap_password, 1, false, 4, WIFI_AUTH_WPA2_PSK)) {
    Serial.println("AP模式启动失败!");
    return false;
  }

  Serial.println("AP模式启动成功!");
  Serial.print("SSID: ");
  Serial.println(ap_ssid);
  Serial.print("密码: ");
  Serial.println(ap_password);
  Serial.print("加密模式: WPA2-PSK");
  Serial.println();
  Serial.print("AP IP地址: ");
  Serial.println(WiFi.softAPIP());

  return true;
}

// 启动STA模式
bool setupSTA() {
  Serial.println("正在启动STA模式...");

  // 切换到STA模式
  WiFi.mode(WIFI_STA);
  delay(100);

  // 配置STA静态IP
  if (!WiFi.config(local_ip, gateway, subnet)) {
    Serial.println("STA模式配置静态IP失败!");
    // 继续尝试连接，但可能会使用DHCP分配的IP
  }

  // 连接到WiFi
  Serial.print("连接到WiFi网络: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  // 等待连接
  Serial.print("等待网络连接");
  int retryCount = 0;
  const int maxRetries = 20; // 20秒超时

  while (WiFi.status() != WL_CONNECTED && retryCount < maxRetries) {
    delay(1000);
    Serial.print(".");
    retryCount++;
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("STA模式连接成功!");
    Serial.print("IP地址: ");
    Serial.println(WiFi.localIP());
    return true;
  } else {
    Serial.println("STA模式连接失败!");
    return false;
  }
}

// 初始化WiFi连接
bool initWiFi() {
  // 先关闭WiFi，然后重新配置
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  delay(100);

  bool wifiConnected = false;

  if (USE_AP_MODE) {
    // 使用AP模式
    wifiConnected = setupAP();
  } else {
    // 使用STA模式，如果失败则尝试AP模式
    wifiConnected = setupSTA();
  }

  return wifiConnected;
}

#endif // WIFI_SETUP_H