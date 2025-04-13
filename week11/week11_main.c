#include "stm32f10x_adc.h"    // ADC 관련 라이브러리
#include "stm32f10x_rcc.h"    // RCC(시계 제어) 관련 라이브러리
#include "stm32f10x_gpio.h"   // GPIO 핀 제어 관련 라이브러리
#include "stm32f10x_usart.h"  // USART(직렬 통신) 관련 라이브러리
#include "stm32f10x_tim.h"    // 타이머 제어 관련 라이브러리
#include "misc.h"             // 인터럽트 관련 유틸리티 라이브러리
#include "lcd.h"              // LCD 제어 라이브러리
#include "touch.h"            // 터치 스크린 관련 라이브러리
#include "math.h"             // 수학 연산 관련 라이브러리
#include "stdio.h"            // 표준 입출력 라이브러리

// LCD 상의 팀 이름과 상태 표시 위치 정의
#define LCD_TEAM_NAME_X 20
#define LCD_TEAM_NAME_Y 50
#define LCD_STATUS_X 20
#define LCD_STATUS_Y 70

// LCD 상의 버튼 위치 및 크기 정의
#define LCD_BUTTON_X    30
#define LCD_BUTTON_Y    100
#define LCD_BUTTON_W    50
#define LCD_BUTTON_H    50

// 함수 프로토타입 선언
void Init(void);
void RccInit(void);
void GpioInit(void);
void TIM_Configure(void);
void NvicInit(void);
void ledToggle(int num);

// LED 색상을 위한 배열 (12가지 색)
const int color[12] = {WHITE, CYAN, BLUE, RED, MAGENTA, LGRAY, GREEN, YELLOW, BROWN, BRRED, GRAY};

// 타이머 카운터와 LED 상태 변수
int timer_counter = 0; // 타이머 카운터
char ledOn;            // LED 상태 (ON/OFF)

// 서보모터 제어를 위한 구조체
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef TIM_OCInitStructure;

// 서보모터 각도와 방향 변수
int motorAngle = 0;    // 서보모터 각도
int motorDir = 0;      // 서보모터 방향

// 메인 함수
int main() {
    uint16_t pos_x, pos_y;  // 터치 스크린의 좌표
    uint16_t pix_x, pix_y;  // 변환된 화면 좌표

    // 초기화 함수 호출
    Init();

    ledOn = 0;  // LED 초기 상태 설정 (OFF)
        
    LCD_Clear(WHITE);  // LCD 화면 초기화 (배경색: WHITE)

    // 팀 이름 표시
    LCD_ShowString(LCD_TEAM_NAME_X, LCD_TEAM_NAME_Y, "THU_04", BLUE, WHITE);

    // 버튼 생성
    LCD_DrawRectangle(LCD_BUTTON_X, LCD_BUTTON_Y, LCD_BUTTON_X + LCD_BUTTON_W, LCD_BUTTON_Y + LCD_BUTTON_H);
    LCD_ShowString(LCD_BUTTON_X + (LCD_BUTTON_W / 2), LCD_BUTTON_Y + (LCD_BUTTON_H / 2), "BUT", RED, WHITE);

    // 메인 루프
    while (1) {
        // LED 상태에 따른 LCD 출력 및 서보모터 방향 설정
        if (ledOn == 0) {
            LCD_ShowString(LCD_STATUS_X, LCD_STATUS_Y, "OFF", RED, WHITE);
            motorDir = 0;  // OFF 상태에서는 서보모터 반대 방향
        } else {
            LCD_ShowString(LCD_STATUS_X, LCD_STATUS_Y, "ON ", RED, WHITE);
            motorDir = 1;  // ON 상태에서는 서보모터 정방향
        }

        // 터치 스크린 좌표 가져오기
        Touch_GetXY(&pos_x, &pos_y, 1);
        Convert_Pos(pos_x, pos_y, &pix_x, &pix_y);

        // 버튼 영역을 터치하면 LED 상태 변경
        if (
            pix_x >= LCD_BUTTON_X &&
            pix_x <= LCD_BUTTON_X + LCD_BUTTON_W &&
            pix_y >= LCD_BUTTON_Y &&
            pix_x <= LCD_BUTTON_Y + LCD_BUTTON_H
        ) {
            ledOn = !ledOn;  // LED 상태 토글
        }
    }
}

// 시스템 초기화 함수
void Init(void) {
   SystemInit();        // 기본 시스템 초기화
   RccInit();           // RCC(클럭) 초기화
   GpioInit();          // GPIO 초기화
   TIM_Configure();     // 타이머 초기화
   NvicInit();          // 인터럽트 컨트롤 초기화

   LCD_Init();          // LCD 초기화
   Touch_Configuration();  // 터치 인터페이스 설정
   Touch_Adjust();         // 터치 캘리브레이션

   // 초기 LED 설정 (OFF)
   GPIO_SetBits(GPIOD, GPIO_Pin_2);
   GPIO_SetBits(GPIOD, GPIO_Pin_3);
}

// RCC(시계) 초기화
void RccInit(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);  // GPIO AFIO 클럭 활성화
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE); // GPIOD 클럭 활성화
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // GPIOA 클럭 활성화
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); // GPIOC 클럭 활성화 안 쓴 듯? 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // GPIOB 클럭 활성화
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);  // TIM2 클럭 활성화
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  // TIM3 클럭 활성화
}

// GPIO 초기화
void GpioInit(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    // LED1, LED2 초기화 (PD2, PD3)
    GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_2 | GPIO_Pin_3); // PD2와 PD3 핀 사용
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;        // 출력 모드 (Push-Pull)
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       // 속도 설정 (50MHz)
    GPIO_Init(GPIOD, &GPIO_InitStructure);                  // 설정 적용

    // PWM 모터 제어를 위한 TIM3(PB0) 핀 초기화
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;               // PB0 핀 사용
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;         // 대체 기능 (Alternate Function)
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       // 속도 설정 (50MHz)
    GPIO_Init(GPIOB, &GPIO_InitStructure);                  // 설정 적용

    // TIM2(PA1) 초기화
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;               // PA1 핀 사용
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;         // 대체 기능
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       // 속도 설정
    GPIO_Init(GPIOA, &GPIO_InitStructure);                  // 설정 적용
}

// 타이머 구성
void TIM_Configure(void) {
    // LED 토글을 위한 TIM2 설정
    TIM_TimeBaseInitTypeDef TIM2_InitStructure;

    TIM2_InitStructure.TIM_Period = 10000;                  // 주기 설정
    TIM2_InitStructure.TIM_Prescaler = 7200;               // 프리스케일러 (클럭 나누기)
    TIM2_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;   // 클럭 분할
    TIM2_InitStructure.TIM_CounterMode = TIM_CounterMode_Down; // 다운 카운터 모드

    TIM_TimeBaseInit(TIM2, &TIM2_InitStructure);           // TIM2 초기화
    TIM_ARRPreloadConfig(TIM2, ENABLE);                    // 자동 재로드 설정
    TIM_Cmd(TIM2, ENABLE);                                 // TIM2 활성화
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);             // 인터럽트 활성화

    // PWM 모터 제어를 위한 TIM3 설정
    TIM_TimeBaseInitTypeDef TIM3_InitStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    TIM3_InitStructure.TIM_Period = 20000;                 // PWM 주기 설정 (20ms)
    TIM3_InitStructure.TIM_Prescaler = 72;                // 프리스케일러
    TIM3_InitStructure.TIM_ClockDivision = 0;             // 클럭 분할 없음
    TIM3_InitStructure.TIM_CounterMode = TIM_CounterMode_Down; // 다운 카운터 모드

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;      // PWM 모드
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; // High 폴라리티
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 출력 활성화
    TIM_OCInitStructure.TIM_Pulse = 1500;                 // 초기 PWM 듀티 사이클 (1500us)

    TIM_OC3Init(TIM3, &TIM_OCInitStructure);              // TIM3 채널 3 초기화

    TIM_TimeBaseInit(TIM3, &TIM3_InitStructure);          // TIM3 초기화
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Disable);    // 프리로드 설정 비활성화
    TIM_ARRPreloadConfig(TIM3, ENABLE);                   // 자동 재로드 설정
    TIM_Cmd(TIM3, ENABLE);                                // TIM3 활성화
}

// 인터럽트 설정
void NvicInit(void) {
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;        // TIM2 인터럽트 채널
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // 최우선 순위
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;     // 서브 우선 순위
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;        // 인터럽트 활성화
    NVIC_Init(&NVIC_InitStructure);                        // 설정 적용
}

// 서보모터 제어
void moveMotor() {
    // motorAngle을 조정하여 서보모터 회전 제어
    if (motorDir == 1) {
        motorAngle += 100; // 방향에 따라 각도 증가
        if (motorAngle >= 2000) motorAngle = 1500; // 각도 범위 제한
    } else {
        motorAngle -= 100; // 방향에 따라 각도 감소
        if (motorAngle <= 1000) motorAngle = 1500; // 각도 범위 제한
    }

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;      // PWM 모드
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; // High 폴라리티
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 출력 활성화
    TIM_OCInitStructure.TIM_Pulse = motorAngle;           // 듀티 사이클 설정

    TIM_OC3Init(TIM3, &TIM_OCInitStructure);              // TIM3 채널 3 업데이트
}

// TIM2 인터럽트 핸들러
void TIM2_IRQHandler(void) {
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {  // TIM2 인터럽트 발생 여부 확인
        timer_counter++;                                  // 타이머 카운터 증가
        moveMotor();                                      // 서보모터 이동
        ledToggle(timer_counter);                        // LED 토글
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);       // 인터럽트 플래그 클리어
    }
}

// LED 토글 함수
void ledToggle(int num) {
    if (ledOn == 1) { // LED가 켜져 있는 경우
        if (num % 2 == 1) { // 홀수 주기마다 LED1 ON
            GPIO_SetBits(GPIOD, GPIO_Pin_2);
        } else {            // 짝수 주기마다 LED1 OFF
            GPIO_ResetBits(GPIOD, GPIO_Pin_2);
        }
        if (num % 10 >= 5) { // 주기 조건에 따라 LED2 ON
            GPIO_SetBits(GPIOD, GPIO_Pin_3);
        } else {            // 주기 조건에 따라 LED2 OFF
            GPIO_ResetBits(GPIOD, GPIO_Pin_3);
        }
    } else { // LED가 꺼져 있는 경우 모든 LED OFF
        GPIO_ResetBits(GPIOD, GPIO_Pin_2);
        GPIO_ResetBits(GPIOD, GPIO_Pin_3);
    }
}
