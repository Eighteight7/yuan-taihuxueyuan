#include "esp8266.h"
#include "usart.h"
#include "string.h"
#include "SysTick.h"
#include "led.h"

extern uint8_t USART_Rx_Buffer[256];  // 外部缓冲区

extern void USART_SendString(USART_TypeDef* USARTx, const char *str);
extern uint8_t USART_Receive(USART_TypeDef* USARTx);


// 连接 WiFi



// 连接到 MQTT 服务器
void esp8266_connect_mqtt()
{

    // 连接 WiFi 
    
    //更改为自己的WiFi用户名和密码
		printf("AT+CWJAP=\"TPLink\",\"12341234\"\r\n");
		delay_ms(5000); 
    //delay用于确保该步骤阻塞完成


    // MQTT 用户配置
   
		printf("AT+MQTTUSERCFG=0,1,\"NULL\",\"led1&k1z5kGfZTTU\",\"97d4398b307d9c54b995257320c621c623bddc4b859264cffc3fbcb90cb79073\",0,0,\"\"\r\n");
    // 配置 MQTT 用户
		delay_ms(2000); 
    

    // 设置 MQTT 客户端 ID
	  printf("AT+MQTTCLIENTID=0,\"k1z5kGfZTTU.led1|securemode=2\\,signmethod=hmacsha256\\,timestamp=1731513162585|\"\r\n");
		delay_ms(2000); 

    // 连接到 MQTT 服务器
		printf("AT+MQTTCONN=0,\"iot-06z00dha4aug5dk.mqtt.iothub.aliyuncs.com\",1883,1\r\n");
    // 连接到 MQTT 服务器
		delay_ms(2000); 
    

    // 订阅 MQTT 主题
		printf("AT+MQTTSUB=0,\"/sys/k1z5kGfZTTU/led1/thing/service/property/set\",1\r\n");
		delay_ms(2000); 

}
//发送常用的参数MQ2，MQ7,DHT11数据来源，分别qual 可燃气体含量  CO一氧化碳含量， HUMI湿度， TEMP温度
void esp8266_publish_mqtt_parameters(int qual, int CO, int humi, int temp)
{
    // 发送 qual 参数
    printf("AT+MQTTPUB=0,\"/k1z5kGfZTTU/led1/user/led1\",\"{\\\"firegas\\\":%d}\",1,0\r\n", qual);
    delay_ms(500); // 等待一段时间以确保指令被处理

    // 发送 CO 参数
    printf("AT+MQTTPUB=0,\"/k1z5kGfZTTU/led1/user/led1\",\"{\\\"coThreshold\\\":%d}\",1,0\r\n", CO);
    delay_ms(500); // 等待一段时间以确保指令被处理

    // 发送 humi 参数
    printf("AT+MQTTPUB=0,\"/k1z5kGfZTTU/led1/user/led1\",\"{\\\"humi\\\":%d}\",1,0\r\n", humi);
    delay_ms(500); // 等待一段时间以确保指令被处理

    // 发送 temp 参数
    printf("AT+MQTTPUB=0,\"/k1z5kGfZTTU/led1/user/led1\",\"{\\\"temp\\\":%d}\",1,0\r\n", temp);
    delay_ms(500); // 等待一段时间以确保指令被处理
}

//发送MB007报警标志位以及报警标志位，不常用参数；
void esp8266_publish_fire(int mb007s, int fire)
{
    // 发送 正点原子火灾报警器 参数
    printf("AT+MQTTPUB=0,\"/k1z5kGfZTTU/led1/user/led1\",\"{\\\"firesign\\\":%d}\",1,0\r\n", mb007s);
    delay_ms(500); // 等待一段时间以确保指令被处理
	    // 发送 融合后火灾判定标志位 参数
    printf("AT+MQTTPUB=0,\"/k1z5kGfZTTU/led1/user/led1\",\"{\\\"fire\\\":%d}\",1,0\r\n", fire);
    delay_ms(500); // 等待一段时间以确保指令被处理
	
}






