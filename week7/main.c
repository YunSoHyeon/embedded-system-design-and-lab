
#include "stm32f10x.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"

#include "misc.h"

/* function prototype */
void RCC_Configure(void);
void GPIO_Configure(void);
void EXTI_Configure(void);
void USART1_Init(void);
void NVIC_Configure(void);

void EXTI15_10_IRQHandler(void);

void Delay(void);

void sendDataUART1(uint16_t data);

//---------------------------------------------------------------------------------------------------

void RCC_Configure(void)
{
	// TODO: Enable the APB2 peripheral clock using the function 'RCC_APB2PeriphClockCmd'
	
	/* UART TX/RX port clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	/* Button 1,2,3 port clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
        
	/* LED port clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	/* USART1 clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	/* Alternate Function IO clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
}

void GPIO_Configure(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure_btn13;
    GPIO_InitTypeDef GPIO_InitStructure_btn2;
    GPIO_InitTypeDef GPIO_InitStructure_UART;
    GPIO_InitTypeDef GPIO_InitStructure_TX;
    GPIO_InitTypeDef GPIO_InitStructure_RX;

    

	// TODO: Initialize the GPIO pins using the structure 'GPIO_InitTypeDef' and the function 'GPIO_Init'
	
    /* Button 1,2,3 pin setting */
    GPIO_InitStructure_btn13.GPIO_Pin = GPIO_Pin_4| GPIO_Pin_13;
    GPIO_InitStructure_btn13.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure_btn13.GPIO_Mode = GPIO_Mode_IPU;
    
    GPIO_Init(GPIOC, &GPIO_InitStructure_btn13);
    
    GPIO_InitStructure_btn2.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure_btn2.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure_btn2.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure_btn2);

    /* LED pin setting*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
	
    /* UART pin setting */
    //TX
    GPIO_InitStructure_TX.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure_TX.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure_TX.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure_TX);
    //RX
    GPIO_InitStructure_RX.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure_RX.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure_RX.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStructure_RX.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure_RX);
	
}

void EXTI_Configure(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;

    // TODO: Select the GPIO pin (button) used as EXTI Line using function 'GPIO_EXTILineConfig'
    // TODO: Initialize the EXTI using the structure 'EXTI_InitTypeDef' and the function 'EXTI_Init'
	
    /* Button 1 */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource4);
    EXTI_InitStructure.EXTI_Line = EXTI_Line4;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Button 2 */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource10);
    	EXTI_InitStructure.EXTI_Line = EXTI_Line10;
        EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
      EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
      EXTI_InitStructure.EXTI_LineCmd = ENABLE;
      EXTI_Init(&EXTI_InitStructure);
        
    /* Button 3 */
        GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource13);
        EXTI_InitStructure.EXTI_Line = EXTI_Line13;
        EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
      EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
      EXTI_InitStructure.EXTI_LineCmd = ENABLE;
      EXTI_Init(&EXTI_InitStructure);
    // NOTE: do not select the UART GPIO pin used as EXTI Line here
}

void USART1_Init(void)
{
	USART_InitTypeDef USART1_InitStructure;

	// Enable the USART1 peripheral
	USART_Cmd(USART1, ENABLE);
	
	// TODO: Initialize the USART using the structure 'USART_InitTypeDef' and the function 'USART_Init'
    // USART1_InitStructure.USART_BaudRate = ???;
        USART1_InitStructure.USART_BaudRate = 9600;
        USART1_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
        USART1_InitStructure.USART_WordLength = USART_WordLength_8b;
        USART1_InitStructure.USART_StopBits = USART_StopBits_1;
        USART1_InitStructure.USART_Parity =  USART_Parity_No;
        USART1_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        
    // ...
    // USART_Init(???);
        USART_Init(USART1, &USART1_InitStructure);
	// TODO: Enable the USART1 RX interrupts using the function 'USART_ITConfig' and the argument value 'Receive Data register not empty interrupt'
	// USART_ITConfig(???);
        USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}

void NVIC_Configure(void) {

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure_Btn1;
    NVIC_InitTypeDef NVIC_InitStructure_Btn2;
    NVIC_InitTypeDef NVIC_InitStructure_Btn3;

    
    // TODO: fill the arg you want
    // NVIC_PriorityGroupConfig(???);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    // TODO: Initialize the NVIC using the structure 'NVIC_InitTypeDef' and the function 'NVIC_Init'
     
    // Button1
    NVIC_InitStructure_Btn1.NVIC_IRQChannel = EXTI4_IRQn;
    NVIC_InitStructure_Btn1.NVIC_IRQChannelPreemptionPriority = 0x00; // TODO
    NVIC_InitStructure_Btn1.NVIC_IRQChannelSubPriority = 0x01; // TODO
    NVIC_InitStructure_Btn1.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure_Btn1);
    // Button2,3
    NVIC_InitStructure_Btn2.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure_Btn2.NVIC_IRQChannelPreemptionPriority = 0x00; // TODO
    NVIC_InitStructure_Btn2.NVIC_IRQChannelSubPriority = 0x01; // TODO
    NVIC_InitStructure_Btn2.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure_Btn2);
    
    // UART1
    // 'NVIC_EnableIRQ' is only required for USART setting
    NVIC_EnableIRQ(USART1_IRQn);
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; // TODO
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01; // TODO
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

uint16_t idx = 0;

void USART1_IRQHandler() {
	uint16_t word;
    if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET){
    	// the most recent received data by the USART1 peripheral
        word = USART_ReceiveData(USART1);

        // TODO implement
        if(word == 'a') {idx = 0;}
        else if (word == 'b') {idx = 1;}
        // clear 'Read data register not empty' flag
    	USART_ClearITPendingBit(USART1,USART_IT_RXNE);
    }
}

void EXTI15_10_IRQHandler(void) { // when the Button 2,3 is pressed
    char msg[] = "Team04\r\n";
    if (EXTI_GetITStatus(EXTI_Line10) != RESET) {
		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) == Bit_RESET) {
			idx = 1;
		}
		EXTI_ClearITPendingBit(EXTI_Line10);
	}
    if (EXTI_GetITStatus(EXTI_Line13) != RESET) {
		if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == Bit_RESET) {
                  for(int i=0; i<8; i++) {
                    sendDataUART1(msg[i]);
                  }
		}
        EXTI_ClearITPendingBit(EXTI_Line13);
	}
}

// TODO: Button 1 interrupt handler implement, referent above function
void EXTI4_IRQHandler(void) { // when the Button 2,3 is pressed

    if (EXTI_GetITStatus(EXTI_Line4) != RESET) {
		if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4) == Bit_RESET) {
			idx = 0;
		}
		EXTI_ClearITPendingBit(EXTI_Line4);
	}
    }


void Delay(void) {
	int i;
	for (i = 0; i < 2000000; i++) {}
}

void sendDataUART1(uint16_t data) {
	/* Wait till TC is set */
	while ((USART1->SR & USART_SR_TC) == 0);
	USART_SendData(USART1, data);
}

int main(void)
{

    SystemInit();

    RCC_Configure();

    GPIO_Configure();

    EXTI_Configure();

    USART1_Init();

    NVIC_Configure();
    uint16_t arr[4] = {GPIO_Pin_2,GPIO_Pin_3,GPIO_Pin_4,GPIO_Pin_7};
    int led;
    led = 3;
    while (1) {
    	// TODO: implement 
      if (idx == 0){
        GPIO_SetBits(GPIOD, arr[led%=4]);
        led ++;
        GPIO_ResetBits(GPIOD, arr[led%=4]);
      }else{
        GPIO_SetBits(GPIOD, arr[led%=4]);
        led --;
        if(led < 0){led = 3;}
        GPIO_ResetBits(GPIOD, arr[led%=4]);
      }    	// Delay
    	Delay();
    }
    return 0;
}
