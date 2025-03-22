# Quad 四足机器人 arduino-wifi 版代码

## 相关链接
- **本项目的 micropython-wifi 版本**: [仓库链接](https://github.com/AniPython/quad-mpy)
- **拼多多**: [商品链接](https://mobile.yangkeduo.com/goods2.html?goods_id=703833751916)
- **B站**: [视频链接](https://www.bilibili.com/video/BV1vePeepEV4/)

## 依赖的库
1. ESP32Servo  
2. ArduinoJson  

## `Config.h` 文件中 wifi 信息修改说明

### 1. 选择 wifi 模式
在 `Config.h` 文件中，需要根据自己的需求选择 wifi 模式。
默认是 `STA` 模式，即连接到路由器。
如果没有路由器, 需要使用 esp32 创建一个热点。
```C++
// 模式选择，true 为 AP 模式，false 为 STA 模式(默认)
const bool USE_AP_MODE = false;
```

### 2. 修改 IP 配置信息
#### 2.1 如果是 AP 模式
在 `Config.h` 文件中，需要根据自己的路由器修改 wifi 信息。
```C++
const char *ssid = "wifi名称";
const char *password = "wifi密码";
IPAddress local_ip(192, 168, x, 192);  // 设置固定 IP 地址
IPAddress gateway(192, 168, x, 1);     // 路由器网关地址
IPAddress subnet(255, 255, 255, 0);    // 子网掩码
```
1) 修改 `ssid` 和 `password` 为自己的路由器信息。  
2) 将 x 值修改为与路由器一致。例如，在 Windows 系统下查看 IP 信息的步骤如下：

打开 设置 -> 网络和 Internet -> 以太网。
查看 IPv4 地址 和 IPv4 DNS 服务器。
示例：
```
IPv4地址: 192.168.2.10
IPv4 DNS 服务器: 192.168.2.1
```
那么 `Config.h` 中就应该是:
```
IPAddress local_ip(192, 168, 2, 192);  // 设置固定 IP 地址
IPAddress gateway(192, 168, 2, 1);     // 路由器网关地址
```
然后可以以电脑或手机上打开浏览器, 输入 `192.168.2.192` 就可以看到控制界面了

#### 2.2 如果是 STA 模式
```C++
const char *ap_ssid = "Otto";
const char *ap_password = "88889999";
IPAddress ap_local_ip(192, 168, 20, 192);
IPAddress ap_gateway(192, 168, 20, 1);
IPAddress ap_subnet(255, 255, 255, 0);
```
可以根据自己的需求修改 `ap_ssid` 和 `ap_password`, `ap_local_ip`, `ap_gateway`, 等信息, 不改也可以。  
然后使用手机或电脑连接这个 `Otto` 热点, 在浏览器中输入 `192.168.20.192` 就可以看到控制界面了
