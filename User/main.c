// main.c
#include "system.h"
#include "SysTick.h"
#include "led.h"
#include "usart.h"
#include "dht11.h"
#include "esp8266.h"
#include "stm32f10x.h"
#include "string.h"
#include "beep.h"
#include "adc.h"
#include "mq2.h"
#include "mq7.h"
#include "flame_sensor.h"
#include "calibration.h"
#include <math.h> // For fmaxf

// --- ��Ȩ����������س��� (��Ҫ: �û��������ʵ�������ϸ����!) ---
// Ȩ�� (Ӱ�����ٷֱ�), �ܺͽӽ� 1 (�������Ҫ����)
#define WEIGHT_TEMP          (0.30f)  // �¶�Ȩ�� (30%)
#define WEIGHT_HUMI          (-0.05f) // ʪ��Ȩ�� (-5%, ʪ�Ƚ������ӷ���)
#define WEIGHT_CO            (0.35f)  // CO Ȩ�� (35%) - ���Ӱ����
#define WEIGHT_QUAL          (0.35f)  // ��ȼ����Ȩ�� (35%) - ���Ӱ����

// ƫ���������� (������ƫ��ӳ�䵽���� 0-1 ��Χ, ��ʾ�ﵽ��ƫ��������)
#define TEMP_SCALE_FACTOR    (30.0f)  // ���� 30 ������ 1.0 ��ƫ���
#define HUMI_SCALE_FACTOR    (40.0f)  // ʪ���½� 40%RH ���� 1.0 ��ƫ���
#define CO_PPM_SCALE_FACTOR  (200.0f) // CO ���� 200 PPM ���� 1.0 ��ƫ��� (��Ҫ���ݻ�׼����!)
#define QUAL_PPM_SCALE_FACTOR (400.0f) // ��ȼ�������� 400 PPM ���� 1.0 ��ƫ��� (��Ҫ���ݻ�׼����!)

// ����������ֵ (��Ȩ���ֳ�����ֵ�򱨾�)
#define ALARM_SCORE_THRESHOLD (700.0f)   // ʾ����ֵ, ��Ҫ�������Ե���

// ��������
#define STABILIZATION_DELAY_MS   (5000) // �궨���ȶ�ʱ�� (ms)

// --- ȫ�ֱ����洢��׼ֵ ---
float g_baseline_temp = -99.0f;
float g_baseline_humi = -99.0f;
float g_baseline_mq2_ppm = -99.0f;
float g_baseline_mq7_ppm = -99.0f;
volatile uint8_t g_baseline_calculated = 0; // ��׼ֵ������ɱ�־

// --- �û������ȫ�ֱ��� ---
int qual; int CO; int humi; int temp; int firesign; int fire = 0;

// --- ������������ ---
void USART3_SendString(char* str);
void Calculate_Initial_Baselines(void); // �޸ĺ�����

// ... (USART3_SendString ����ʵ�� - ͬǰ) ...
void USART3_SendString(char* str) { /* ... USART3 Send implementation ... */
    while (*str) {
        while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
        USART_SendData(USART3, (uint8_t)(*str));
        str++;
    }
}

int main() {
    // --- �ֲ����� ---
    uint8_t dht_temp_raw = 0;
    uint8_t dht_humi_raw = 0;
    float mq2_ppm_raw = 0.0f;
    float mq7_ppm_raw = 0.0f;
    uint8_t flame_state_raw = FLAME_NOT_DETECTED;
    DHT11_StatusTypeDef dht11_status;
    uint8_t alarm_active = 0;
    float current_risk_score = 0.0f; // ��ǰ��������

    // --- ��ʼ�� ---
    SysTick_Init(72);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
		esp8266_connect_mqtt();
    LED_Init();
    USART3_Init(115200);
    ADC1_Init_MQ();
    MQ2_Init();
    MQ7_Init();
    DHT11_Init();
    FlameSensor_Init();
    BEEP_Init();
    // ... (ESP8266 ��λ �� LED ָʾ - ͬǰ) ...
    delay_ms(1000);
    USART3_SendString("AT+RST\r\n");
    delay_ms(2000);
    LED2 = 0; LED1 = 0; delay_ms(500); LED2 = 1; LED1 = 1;

    // --- MQ �������Զ��궨 ---
    // ** �ؼ�: ȷ����ʱ���ڽྻ���� **
    Calibration_Run_MQ();

    // --- �����ʼ��׼ֵ ---
    // ** �ؼ�: ȷ����ʱ��Ȼ�����ȶ����ྻ�Ŀ����� **
    Calculate_Initial_Baselines(); // ���ü����׼����

    // --- ��ѭ�� ---
    while (1) {
        alarm_active = 0; // ���ñ�����־
        current_risk_score = 0.0f; // ���÷�������

        // --- 1. ��ȡ���д��������� ---
        if (Calibration_Is_Done()) {
            mq2_ppm_raw = MQ2_Get_PPM_Smoke_Auto();
            mq7_ppm_raw = MQ7_Get_PPM_CO_Auto();
        } else {
            mq2_ppm_raw = -1.0f;
            mq7_ppm_raw = -1.0f;
        }
        dht11_status = DHT11_Read_Data(&dht_temp_raw, &dht_humi_raw);
        flame_state_raw = FlameSensor_Read_State();

        // --- 2. ����ȫ�ֱ���  ---
        qual = (int)mq2_ppm_raw;
        CO = (int)mq7_ppm_raw;
        humi = (int)dht_humi_raw;
        temp = (int)dht_temp_raw;
        firesign = (int)flame_state_raw;
				esp8266_publish_mqtt_parameters(qual,CO,humi,temp);
        // --- 3. �����Ȩ�������� (������׼��������Ҷ�����Ч) ---
        if (g_baseline_calculated && Calibration_Is_Done()) {
            float dev_t = 0.0f, dev_h = 0.0f, dev_co = 0.0f, dev_q = 0.0f;

            // �������ź��ƫ�� (������ 0 ������)
            if (dht11_status == DHT11_OK) {
                dev_t = fmaxf(0.0f, ((float)dht_temp_raw - g_baseline_temp)) / TEMP_SCALE_FACTOR;
                dev_h = fmaxf(0.0f, (g_baseline_humi - (float)dht_humi_raw)) / HUMI_SCALE_FACTOR; // ʪ�ȷ���
            }
            if (mq7_ppm_raw >= 0 && g_baseline_mq7_ppm > 0) { // �������
                 dev_co = fmaxf(0.0f, (mq7_ppm_raw - g_baseline_mq7_ppm)) / CO_PPM_SCALE_FACTOR;
                // ���������ƫ��: dev_co = fmaxf(0.0f, (mq7_ppm_raw - g_baseline_mq7_ppm)) / g_baseline_mq7_ppm; ��Ҫ����Ȩ�غ���ֵ
            }
             if (mq2_ppm_raw >= 0 && g_baseline_mq2_ppm > 0) { // �������
                dev_q = fmaxf(0.0f, (mq2_ppm_raw - g_baseline_mq2_ppm)) / QUAL_PPM_SCALE_FACTOR;
                 // ���������ƫ��: dev_q = fmaxf(0.0f, (mq2_ppm_raw - g_baseline_mq2_ppm)) / g_baseline_mq2_ppm;
            }

            // �����Ȩ�ܷ�
            current_risk_score = (WEIGHT_TEMP * dev_t) +
                                 (WEIGHT_HUMI * dev_h) + // ע��ʪ��Ȩ��Ϊ��
                                 (WEIGHT_CO * dev_co) +
                                 (WEIGHT_QUAL * dev_q);

            // ������ֳ�����ֵ, ���ñ�����־
            if (current_risk_score > ALARM_SCORE_THRESHOLD) {
                alarm_active = 1;
            }
        }

        // --- 4. ���ձ����ж� (ֱ�ӻ�������) ---
        if (flame_state_raw == FLAME_DETECTED) {
            alarm_active = 1; // ֱ�ӻ����źŸ��������ж�
        }

        // ����ȫ�� fire ��־ (�������ձ���״̬)
        fire = alarm_active;
				esp8266_publish_fire(flame_state_raw, alarm_active);

        // --- 5. ���Ʒ����� ---
        if (alarm_active == 1) {
            BEEP_ON();
        } else {
            BEEP_OFF();
        }

        // --- 6. ���� LED ָʾ�� (ʾ��) ---
        LED3 = (alarm_active == 1) ? 0 : 1;       // LED1: ����ָʾ��
				LED4 = (alarm_active == 1) ? 1 : 0;       // LED1: ����ָʾ��
        // --- ѭ����ʱ ---
        delay_ms(2000);
    }
    // return 0;
}


/**
 * @brief ���㴫�����ĳ�ʼ��׼ֵ (���ټ�����ֵ)
 * @note Ӧ�� Calibration_Run_MQ() ֮��, �Ҵ������ȶ������һ��.
 * @note ** ����ȷ����ʱ�������ȶ����ྻ��! **
 */
void Calculate_Initial_Baselines(void) { // �������޸�
    uint8_t dht_temp_base = 0;
    uint8_t dht_humi_base = 0;
    float mq2_ppm_base = -1.0f;
    float mq7_ppm_base = -1.0f;
    DHT11_StatusTypeDef dht_status_base;
    int retry_count = 0;
    const int max_retries = 3;

    g_baseline_calculated = 0;

    if (!Calibration_Is_Done()) {
        return;
    }
    delay_ms(STABILIZATION_DELAY_MS); // �ȴ��ȶ�

    // ��ȡ��׼ֵ
    retry_count = 0;
    do {
        dht_status_base = DHT11_Read_Data(&dht_temp_base, &dht_humi_base);
        if (dht_status_base == DHT11_OK) break;
        retry_count++;
        delay_ms(500);
    } while (retry_count < max_retries);

    mq2_ppm_base = MQ2_Get_PPM_Smoke_Auto();
    mq7_ppm_base = MQ7_Get_PPM_CO_Auto();

    // �洢��׼ֵ (ʧ������Ĭ��ֵ)
    if (dht_status_base == DHT11_OK) {
        g_baseline_temp = (float)dht_temp_base;
        g_baseline_humi = (float)dht_humi_base;
    } else {
        g_baseline_temp = 25.0f;
        g_baseline_humi = 50.0f;
    }
    g_baseline_mq2_ppm = (mq2_ppm_base >= 0) ? mq2_ppm_base : 50.0f;
    g_baseline_mq7_ppm = (mq7_ppm_base >= 0) ? mq7_ppm_base : 20.0f;

    g_baseline_calculated = 1; // ��ǻ�׼�������

   
    LED2 = 0; delay_ms(100); LED2 = 1; delay_ms(100);
    LED2 = 0; delay_ms(100); LED2 = 1;
}

