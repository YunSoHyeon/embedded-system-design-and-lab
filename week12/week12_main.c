#include "stm32f10x_adc.h" // ADC 관련 헤더 파일 포함
#include "stm32f10x_rcc.h" // RCC(클록 관리) 관련 헤더 파일 포함
#include "stm32f10x_gpio.h" // GPIO 관련 헤더 파일 포함
#include "stm32f10x_usart.h" // USART 관련 헤더 파일 포함
#include "misc.h" // 기타 유틸리티 헤더 파일 포함
#include "lcd.h" // LCD 제어 관련 헤더 파일 포함
#include "touch.h" // 터치 관련 헤더 파일 포함

#define LCD_TEAM_NAME_X 20 // 팀 이름 표시 X 좌표 설정
#define LCD_TEAM_NAME_Y 50 // 팀 이름 표시 Y 좌표 설정
#define LCD_LUX_VAL_X   20 // 조도 값 표시 X 좌표 설정
#define LCD_LUX_VAL_Y   110 // 조도 값 표시 Y 좌표 설정

volatile uint32_t ADC_Value[1]; // DMA로 읽어온 ADC 값을 저장하는 전역 변수 (volatile 선언으로 값 변경을 즉시 반영)

int main() {
   u16 bgColor = WHITE; // LCD 초기 배경색을 흰색으로 설정

   SystemInit(); // 시스템 초기화 함수 호출

   RccInit(); // RCC 관련 초기화 (클록 활성화)

   GpioInit(); // GPIO 초기화 (조도센서 핀 설정)

   AdcInit(); // ADC 초기화 (아날로그 입력값 설정)

   DMA_Configure(); // DMA 초기화 (ADC 데이터를 메모리로 전달)

   LCD_Init(); // LCD 초기화

   Touch_Configuration(); // 터치 관련 설정 초기화

   Touch_Adjust(); // 터치 보정 수행

   LCD_Clear(WHITE); // LCD 화면을 흰색으로 초기화

   while (1) { // 무한 루프 시작
       if (ADC_Value[0] >= 3900 && bgColor == WHITE) { // 조도 값이 임계값 이상이고 배경이 흰색이면
           bgColor = GRAY; // 배경색을 회색으로 변경
           LCD_Clear(bgColor); // LCD 배경을 회색으로 초기화
       } else if (ADC_Value[0] < 3900 && bgColor == GRAY) { // 조도 값이 임계값 미만이고 배경이 회색이면
           bgColor = WHITE; // 배경색을 흰색으로 변경
           LCD_Clear(bgColor); // LCD 배경을 흰색으로 초기화
       }
       LCD_ShowString(1, 1, "THUR_TEAM04", BLACK, bgColor); // 팀 이름 표시
       LCD_ShowNum(200, 1, ADC_Value[0], 4, RED, bgColor); // ADC 값 출력
   }
   return 0; // 프로그램 종료 (절대 실행되지 않음)
}

void RccInit(void) {
    // DMA1 클록 활성화 : ADC 데이터를 DMA로 전송하기 위해 DMA1이 필요하다
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); 

    // ADC1 클럭 활성화 : 조도 센서 데이터를 디지털로 변환하기 위해 ADC1 사용
    // GPIOB 클럭 활성화 : 조도 센서가 GPIOB 포트의 핀(B0)과 연결되어 있으므로 해당 GPIO 포트를 활성화
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOB, ENABLE); 

    // AFIO 클럭 활성화 : ADC 및 기타 기능을 위해 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); 
}

void GpioInit(void) {
    GPIO_InitTypeDef GPIO_InitStructure; // GPIO 설정 구조체 선언

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; // B0 핀 설정
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 속도 설정
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; // 아날로그 입력 모드 설정
    GPIO_Init(GPIOB, &GPIO_InitStructure); // GPIOB 초기화
}

void AdcInit(void) {
    ADC_InitTypeDef ADC_InitStructure; // ADC 설정 구조체 선언

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; // 독립 모드 설정
    ADC_InitStructure.ADC_ScanConvMode = DISABLE; // 스캔 변환 모드 비활성화
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; // 연속 변환 모드 활성화
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // 외부 트리거 비활성화
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; // 데이터 정렬을 오른쪽으로 설정
    ADC_InitStructure.ADC_NbrOfChannel = 1; // 변환 채널 수 설정
    ADC_Init(ADC1, &ADC_InitStructure); // ADC1 초기화

    ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_239Cycles5); // 채널 설정
    ADC_DMACmd(ADC1, ENABLE); // ADC와 DMA 연결 활성화
    ADC_Cmd(ADC1, ENABLE); // ADC1 활성화
    ADC_ResetCalibration(ADC1); // ADC1 캘리브레이션 리셋
    while (ADC_GetResetCalibrationStatus(ADC1)); // 캘리브레이션 리셋 완료 대기

    ADC_StartCalibration(ADC1); // 캘리브레이션 시작
    while (ADC_GetCalibrationStatus(ADC1)); // 캘리브레이션 완료 대기

    ADC_SoftwareStartConvCmd(ADC1, ENABLE); // ADC 소프트웨어 변환 시작
}

void DMA_Configure(void) {
    DMA_InitTypeDef DMA_InitStructure; // DMA 설정 구조체 선언

    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR; // ADC1 데이터 레지스터 주소 설정
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_Value[0]; // 메모리 주소 설정
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; // 데이터 전송 방향 설정 (주변장치 -> 메모리)
    DMA_InitStructure.DMA_BufferSize = 1; // 버퍼 크기 설정
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // 주변장치 주소 증가 비활성화
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable; // 메모리 주소 증가 비활성화
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word; // 주변장치 데이터 크기 설정 (32비트)
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word; // 메모리 데이터 크기 설정 (32비트)
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; // 순환 모드 설정
    DMA_InitStructure.DMA_Priority = DMA_Priority_High; // 우선순위 높게 설정
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; // 메모리-메모리 전송 비활성화

    DMA_Init(DMA1_Channel1, &DMA_InitStructure); // DMA1 채널1 초기화
    DMA_Cmd(DMA1_Channel1, ENABLE); // DMA1 채널1 활성화
}
