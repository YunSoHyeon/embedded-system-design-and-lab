#include "stm32f10x.h"

// 컴파일러의 최적화를 막아 모든 instruction이 실행되도록 volatile 키워드 사용

// RCC base register + clock enable 
#define RCC_ABP2ENR (*(volatile unsigned int*)0x40021018) // 0x40021000 + 0x18

// port D set/reset register
#define GPIO_D_BSRR (*(volatile unsigned int*)0x40011410) // 0x40011400 + 0x10
// port D reset register 
#define GPIO_D_BRR (*(volatile unsigned int*)0x40011414) // 0x40011400 + 0x14

// port C configuration low
#define GPIO_C_CRL (*(volatile unsigned int*)0x40011000) // PC4
// port C configuration high
#define GPIO_C_CRH (*(volatile unsigned int*)0x40011004) // PC13
// port C input data regiser
#define GPIO_C_IDR (*(volatile unsigned int*)0x40011008)

// port B configuration high
#define GPIO_B_CRH (*(volatile unsigned int*)0x40010C04) // PB10
// port B input data regiser
#define GPIO_B_IDR (*(volatile unsigned int*)0x40010C08)

// port A configuration high
#define GPIO_A_CRL (*(volatile unsigned int*)0x40010800) // PA0
// port A input data regiser
#define GPIO_A_IDR (*(volatile unsigned int*)0x40010808)

#define GPIO_D_CRL (*(volatile unsigned int*)0x40011400)

int main(void)
{
  // 1. RCC enable
  // RCC_ABP2ENR |= (1<<2)|(1<<3)|(1<<4)|(1<<5);
  RCC_ABP2ENR |= 0x3C; // IO port D, C, B, A clock enable(bit5, bit4, bit3, bit2)
 
  // 2. GPIOx
  GPIO_D_CRL  &= 0x0FF000FF;    // reset
  GPIO_D_CRL  |= 0x10011100;    // output mode
 
  GPIO_C_CRL &= 0xFFF0FFFF;     
  GPIO_C_CRL |= 0x00080000;     

  GPIO_C_CRH &= 0xFF0FFFFF;
  GPIO_C_CRH |= 0x00800000;

  GPIO_B_CRH &= 0xFFFFF0FF;
  GPIO_B_CRH |= 0x00000800;
 
  GPIO_A_CRL &= 0xFFFFFFF0;
  GPIO_A_CRL |= 0x00000008;
 
 
  // 3. set/reset register
  GPIO_D_BSRR |= (0x4 | 0x80 | 0x8 | 0x10); // LED1, 2, 3, 4 off상태로 초기화
  GPIO_D_BRR &= 0x00000000;

  // 4. input data register
  GPIO_C_IDR &= (1<<4);
  GPIO_B_IDR &= (1<<10);
  GPIO_A_IDR &= (1<<0);



    while(1){
      if(!(GPIO_C_IDR& (1<<4))){ // Button1 눌렀는지 확인
        GPIO_D_BRR |= 0x4;   // PD2 LED(LED1) on
        GPIO_D_BRR |= 0x8;   // PD3 LED(LED2) on
        
      }
      if(!(GPIO_B_IDR& (1<<10))){ // Button2 눌렀는지 확인
        GPIO_D_BSRR |= 0x8;  // PD3 LED(LED2) off
        GPIO_D_BSRR |= 0x4;  // PD2 LED(LED1) off
       
      }
      if(!(GPIO_C_IDR& (1<<13))){ // Button3 눌렀는지 확인
        GPIO_D_BRR |= 0x10;  // PD4 LED(LED3) on
        GPIO_D_BRR |= 0x80;  // PD7 LED(LED4) on
       
      }
      if(!(GPIO_A_IDR& (1<<0))){  // Button4 눌렀는지 확인
        GPIO_D_BSRR |= 0x10; // PD4 LED(LED3) off
        GPIO_D_BSRR |= 0x80; // PD7 LED(LED4) off
      }  
  }
 
  return 0;
}