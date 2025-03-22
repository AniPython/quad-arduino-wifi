#ifndef CONFIG_H
#define CONFIG_H

#include <WiFi.h>  // 需要包含以使用IPAddress类型

// 模式选择，true 为 AP 模式，false 为 STA 模式(默认)
const bool USE_AP_MODE = false;

// STA 模式(默认), 需要连路由器, 请使用 2.4G 频段
const char *ssid = "小亦站";
const char *password = "88889999";
IPAddress local_ip(192, 168, 2, 192);  // 设置固定 IP 地址
IPAddress gateway(192, 168, 2, 1);     // 路由器网关地址
IPAddress subnet(255, 255, 255, 0);    // 子网掩码

// AP 模式配置
const char *ap_ssid = "Otto";
const char *ap_password = "88889999";
IPAddress ap_local_ip(192, 168, 20, 192);
IPAddress ap_gateway(192, 168, 20, 1);
IPAddress ap_subnet(255, 255, 255, 0);

#endif
