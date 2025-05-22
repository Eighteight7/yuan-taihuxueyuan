#include "esp8266.h"
#include "usart.h"
#include "string.h"
#include "SysTick.h"
#include "led.h"

extern uint8_t USART_Rx_Buffer[256];  // �ⲿ������

extern void USART_SendString(USART_TypeDef* USARTx, const char *str);
extern uint8_t USART_Receive(USART_TypeDef* USARTx);


// ���� WiFi



// ���ӵ� MQTT ������
void esp8266_connect_mqtt()
{

    // ���� WiFi 
    
    //����Ϊ�Լ���WiFi�û���������
		printf("AT+CWJAP=\"TPLink\",\"12341234\"\r\n");
		delay_ms(5000); 
    //delay����ȷ���ò����������


    // MQTT �û�����
   
		printf("AT+MQTTUSERCFG=0,1,\"NULL\",\"led1&k1z5kGfZTTU\",\"97d4398b307d9c54b995257320c621c623bddc4b859264cffc3fbcb90cb79073\",0,0,\"\"\r\n");
    // ���� MQTT �û�
		delay_ms(2000); 
    

    // ���� MQTT �ͻ��� ID
	  printf("AT+MQTTCLIENTID=0,\"k1z5kGfZTTU.led1|securemode=2\\,signmethod=hmacsha256\\,timestamp=1731513162585|\"\r\n");
		delay_ms(2000); 

    // ���ӵ� MQTT ������
		printf("AT+MQTTCONN=0,\"iot-06z00dha4aug5dk.mqtt.iothub.aliyuncs.com\",1883,1\r\n");
    // ���ӵ� MQTT ������
		delay_ms(2000); 
    

    // ���� MQTT ����
		printf("AT+MQTTSUB=0,\"/sys/k1z5kGfZTTU/led1/thing/service/property/set\",1\r\n");
		delay_ms(2000); 

}
//���ͳ��õĲ���MQ2��MQ7,DHT11������Դ���ֱ�qual ��ȼ���庬��  COһ����̼������ HUMIʪ�ȣ� TEMP�¶�
void esp8266_publish_mqtt_parameters(int qual, int CO, int humi, int temp)
{
    // ���� qual ����
    printf("AT+MQTTPUB=0,\"/k1z5kGfZTTU/led1/user/led1\",\"{\\\"firegas\\\":%d}\",1,0\r\n", qual);
    delay_ms(500); // �ȴ�һ��ʱ����ȷ��ָ�����

    // ���� CO ����
    printf("AT+MQTTPUB=0,\"/k1z5kGfZTTU/led1/user/led1\",\"{\\\"coThreshold\\\":%d}\",1,0\r\n", CO);
    delay_ms(500); // �ȴ�һ��ʱ����ȷ��ָ�����

    // ���� humi ����
    printf("AT+MQTTPUB=0,\"/k1z5kGfZTTU/led1/user/led1\",\"{\\\"humi\\\":%d}\",1,0\r\n", humi);
    delay_ms(500); // �ȴ�һ��ʱ����ȷ��ָ�����

    // ���� temp ����
    printf("AT+MQTTPUB=0,\"/k1z5kGfZTTU/led1/user/led1\",\"{\\\"temp\\\":%d}\",1,0\r\n", temp);
    delay_ms(500); // �ȴ�һ��ʱ����ȷ��ָ�����
}

//����MB007������־λ�Լ�������־λ�������ò�����
void esp8266_publish_fire(int mb007s, int fire)
{
    // ���� ����ԭ�ӻ��ֱ����� ����
    printf("AT+MQTTPUB=0,\"/k1z5kGfZTTU/led1/user/led1\",\"{\\\"firesign\\\":%d}\",1,0\r\n", mb007s);
    delay_ms(500); // �ȴ�һ��ʱ����ȷ��ָ�����
	    // ���� �ںϺ�����ж���־λ ����
    printf("AT+MQTTPUB=0,\"/k1z5kGfZTTU/led1/user/led1\",\"{\\\"fire\\\":%d}\",1,0\r\n", fire);
    delay_ms(500); // �ȴ�һ��ʱ����ȷ��ָ�����
	
}






