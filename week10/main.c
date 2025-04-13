#include "stm32f10x.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_rcc.h"
#include "misc.h"
#include "core_cm3.h"
#include "lcd.h"
#include "touch.h"


/* function prototype */
void RCC_Configure(void);
void GPIO_Configure(void);
void NVIC_Configure(void);

//---------------------------------------------------------------------------------------------------

void RCC_Configure(void)
{
    /* Port A, C, D, E clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE);

    /* UART1 TX/RX port clock enable */
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
    /* ADC1 enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    /* UART2 Enable */
    //RCC_APB1PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB1Periph_USART2, ENABLE);

    /* Alternate Function IO clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    //GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
}

void GPIO_Configure(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    /* ADC1 pin setting */
    //Input
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

}

void ADC_Configure(void)
{
    ADC_InitTypeDef ADC_InitStructure;

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_NbrOfChannel = 1;

    ADC_Init(ADC1, &ADC_InitStructure);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 1, ADC_SampleTime_239Cycles5);
    ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
    ADC_Cmd(ADC1, ENABLE);
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1));
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

void NVIC_Configure(void) {

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;// ADC1_2
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

uint16_t value = 0;
void ADC1_2_IRQHandler() {
    if (ADC_GetITStatus(ADC1, ADC_IT_EOC) != RESET) {
        value = ADC_GetConversionValue(ADC1);
        ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
    }
}
void delay(){
  int i;
  for(i=0; i < 100; i++){}
}

int main(void)
{
    SystemInit();

    RCC_Configure();

    GPIO_Configure();
    
    ADC_Configure();

    NVIC_Configure();

    LCD_Init();

    Touch_Configuration();

    Touch_Adjust();

    LCD_Clear(WHITE);

    uint16_t x, y;
    uint16_t l_x, l_y;
    while (1) {
        LCD_ShowString(50, 30, "THUR_Team04", GREEN, WHITE);
        Touch_GetXY(&x, &y, 1);
        Convert_Pos(x, y, &l_x, &l_y);

        LCD_ShowNum(40, 60, l_x, 3, BLACK, WHITE);
        LCD_ShowNum(40, 80, l_y, 3, BLACK, WHITE);
        LCD_ShowNum(20, 100, value, 4, RED, WHITE);

        LCD_DrawCircle(l_x, l_y, 10);
        

    }
    return 0;
}