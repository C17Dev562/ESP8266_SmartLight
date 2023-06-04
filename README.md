# ESP8266 存在感应灯项目

该存在感应灯在ESP8266开发板的基础上使用毫米波雷达检测人员存在情况，进而根据预设参数 控制WS2812LED灯带

该存在感应灯通过2.4gWIFI进行与服务器的连接，并具有配网功能。
通过MQTT协议与服务器交互，接收服务器下发的设置信息，同时上传设备的状态，达到远程控制的目的

### 硬件设备

ESP8266 最小系统板

R24DVD1模块（UART连接）

WS2812灯带

### 功能

WS2812灯带相关

- 开/关
- 调节亮度

毫米波雷达相关

- 有无人检测

网络相关

- 配网（ESP8266 AP 网页配网）

远程控制相关

- 状态上报
  - 灯光状态上报
  - 雷达状态上报
- 接收控灯指令
  - 开关
  - 亮度调节

> 附带MQTT连接用户协议

