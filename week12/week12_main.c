#include "stm32f10x_adc.h" // ADC ���� ��� ���� ����
#include "stm32f10x_rcc.h" // RCC(Ŭ�� ����) ���� ��� ���� ����
#include "stm32f10x_gpio.h" // GPIO ���� ��� ���� ����
#include "stm32f10x_usart.h" // USART ���� ��� ���� ����
#include "misc.h" // ��Ÿ ��ƿ��Ƽ ��� ���� ����
#include "lcd.h" // LCD ���� ���� ��� ���� ����
#include "touch.h" // ��ġ ���� ��� ���� ����

#define LCD_TEAM_NAME_X 20 // �� �̸� ǥ�� X ��ǥ ����
#define LCD_TEAM_NAME_Y 50 // �� �̸� ǥ�� Y ��ǥ ����
#define LCD_LUX_VAL_X   20 // ���� �� ǥ�� X ��ǥ ����
#define LCD_LUX_VAL_Y   110 // ���� �� ǥ�� Y ��ǥ ����

volatile uint32_t ADC_Value[1]; // DMA�� �о�� ADC ���� �����ϴ� ���� ���� (volatile �������� �� ������ ��� �ݿ�)

int main() {
   u16 bgColor = WHITE; // LCD �ʱ� ������ ������� ����

   SystemInit(); // �ý��� �ʱ�ȭ �Լ� ȣ��

   RccInit(); // RCC ���� �ʱ�ȭ (Ŭ�� Ȱ��ȭ)

   GpioInit(); // GPIO �ʱ�ȭ (�������� �� ����)

   AdcInit(); // ADC �ʱ�ȭ (�Ƴ��α� �Է°� ����)

   DMA_Configure(); // DMA �ʱ�ȭ (ADC �����͸� �޸𸮷� ����)

   LCD_Init(); // LCD �ʱ�ȭ

   Touch_Configuration(); // ��ġ ���� ���� �ʱ�ȭ

   Touch_Adjust(); // ��ġ ���� ����

   LCD_Clear(WHITE); // LCD ȭ���� ������� �ʱ�ȭ

   while (1) { // ���� ���� ����
       if (ADC_Value[0] >= 3900 && bgColor == WHITE) { // ���� ���� �Ӱ谪 �̻��̰� ����� ����̸�
           bgColor = GRAY; // ������ ȸ������ ����
           LCD_Clear(bgColor); // LCD ����� ȸ������ �ʱ�ȭ
       } else if (ADC_Value[0] < 3900 && bgColor == GRAY) { // ���� ���� �Ӱ谪 �̸��̰� ����� ȸ���̸�
           bgColor = WHITE; // ������ ������� ����
           LCD_Clear(bgColor); // LCD ����� ������� �ʱ�ȭ
       }
       LCD_ShowString(1, 1, "THUR_TEAM04", BLACK, bgColor); // �� �̸� ǥ��
       LCD_ShowNum(200, 1, ADC_Value[0], 4, RED, bgColor); // ADC �� ���
   }
   return 0; // ���α׷� ���� (���� ������� ����)
}

void RccInit(void) {
    // DMA1 Ŭ�� Ȱ��ȭ : ADC �����͸� DMA�� �����ϱ� ���� DMA1�� �ʿ��ϴ�
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); 

    // ADC1 Ŭ�� Ȱ��ȭ : ���� ���� �����͸� �����з� ��ȯ�ϱ� ���� ADC1 ���
    // GPIOB Ŭ�� Ȱ��ȭ : ���� ������ GPIOB ��Ʈ�� ��(B0)�� ����Ǿ� �����Ƿ� �ش� GPIO ��Ʈ�� Ȱ��ȭ
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOB, ENABLE); 

    // AFIO Ŭ�� Ȱ��ȭ : ADC �� ��Ÿ ����� ���� 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); 
}

void GpioInit(void) {
    GPIO_InitTypeDef GPIO_InitStructure; // GPIO ���� ����ü ����

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; // B0 �� ����
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // �ӵ� ����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; // �Ƴ��α� �Է� ��� ����
    GPIO_Init(GPIOB, &GPIO_InitStructure); // GPIOB �ʱ�ȭ
}

void AdcInit(void) {
    ADC_InitTypeDef ADC_InitStructure; // ADC ���� ����ü ����

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; // ���� ��� ����
    ADC_InitStructure.ADC_ScanConvMode = DISABLE; // ��ĵ ��ȯ ��� ��Ȱ��ȭ
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; // ���� ��ȯ ��� Ȱ��ȭ
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // �ܺ� Ʈ���� ��Ȱ��ȭ
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; // ������ ������ ���������� ����
    ADC_InitStructure.ADC_NbrOfChannel = 1; // ��ȯ ä�� �� ����
    ADC_Init(ADC1, &ADC_InitStructure); // ADC1 �ʱ�ȭ

    ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_239Cycles5); // ä�� ����
    ADC_DMACmd(ADC1, ENABLE); // ADC�� DMA ���� Ȱ��ȭ
    ADC_Cmd(ADC1, ENABLE); // ADC1 Ȱ��ȭ
    ADC_ResetCalibration(ADC1); // ADC1 Ķ���극�̼� ����
    while (ADC_GetResetCalibrationStatus(ADC1)); // Ķ���극�̼� ���� �Ϸ� ���

    ADC_StartCalibration(ADC1); // Ķ���극�̼� ����
    while (ADC_GetCalibrationStatus(ADC1)); // Ķ���극�̼� �Ϸ� ���

    ADC_SoftwareStartConvCmd(ADC1, ENABLE); // ADC ����Ʈ���� ��ȯ ����
}

void DMA_Configure(void) {
    DMA_InitTypeDef DMA_InitStructure; // DMA ���� ����ü ����

    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR; // ADC1 ������ �������� �ּ� ����
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_Value[0]; // �޸� �ּ� ����
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; // ������ ���� ���� ���� (�ֺ���ġ -> �޸�)
    DMA_InitStructure.DMA_BufferSize = 1; // ���� ũ�� ����
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // �ֺ���ġ �ּ� ���� ��Ȱ��ȭ
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable; // �޸� �ּ� ���� ��Ȱ��ȭ
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word; // �ֺ���ġ ������ ũ�� ���� (32��Ʈ)
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word; // �޸� ������ ũ�� ���� (32��Ʈ)
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; // ��ȯ ��� ����
    DMA_InitStructure.DMA_Priority = DMA_Priority_High; // �켱���� ���� ����
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; // �޸�-�޸� ���� ��Ȱ��ȭ

    DMA_Init(DMA1_Channel1, &DMA_InitStructure); // DMA1 ä��1 �ʱ�ȭ
    DMA_Cmd(DMA1_Channel1, ENABLE); // DMA1 ä��1 Ȱ��ȭ
}
