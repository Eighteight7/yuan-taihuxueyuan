#ifndef __ESP8266_H
#define __ESP8266_H

#include "stm32f10x.h"

// ESP8266 ��ʼ��
void esp8266_init(void);

// ���� WiFi
void esp8266_connect_wifi(const char *ssid, const char *password);

// ���� MQTT ������
void esp8266_connect_mqtt(void);

// ���� MQTT ��Ϣ

void esp8266_publish_mqtt_parameters(int qual, int CO, int humi, int temp);

// ���� AT ָ��ȴ���Ӧ
void esp8266_send_at_cmd(const char *cmd);

void esp8266_publish_fire(int mb007s, int fire);

#endif
