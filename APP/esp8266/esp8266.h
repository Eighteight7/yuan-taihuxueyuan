#ifndef __ESP8266_H
#define __ESP8266_H

#include "stm32f10x.h"

// ESP8266 初始化
void esp8266_init(void);

// 连接 WiFi
void esp8266_connect_wifi(const char *ssid, const char *password);

// 连接 MQTT 服务器
void esp8266_connect_mqtt(void);

// 发布 MQTT 消息

void esp8266_publish_mqtt_parameters(int qual, int CO, int humi, int temp);

// 发送 AT 指令并等待响应
void esp8266_send_at_cmd(const char *cmd);

void esp8266_publish_fire(int mb007s, int fire);

#endif
