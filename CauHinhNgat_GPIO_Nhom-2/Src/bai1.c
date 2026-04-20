/* File name: bai1.c
*
* Description: bai1.c Thuc Tap Co So Thay Hoa Nhom 2
*
*
* Last Changed By: $Author: NGUYENHAI $
* Revision: $Revision: 1.0 $
* Last Changed: $Date: Apr 20, 2026 $
*
* Code sample:
*******************************************************************************/
/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/
#include <misc.h>
#include <stdio.h>
#include <stdint.h>
#include <stm32f401re_exti.h>
#include "stm32f401re_rcc.h"
#include "stm32f401re_syscfg.h"
#include "stm32f401re_gpio.h"


/******************************************************************************/
/*                     PRIVATE TYPES and DEFINITIONS                         */
/******************************************************************************/
#define GPIO_PIN_SET				1
#define GPIO_PIN_RESET				0
#define GPIO_PIN_HIGH				1
#define GPIO_PIN_LOW				0

#define LED_GPIO_PORT				GPIOA
#define LED_GPIO_PIN				GPIO_Pin_5
#define LEDControl_SetClock			RCC_AHB1Periph_GPIOA

#define BUTTON_GPIO_PORT			GPIOC
#define BUTTON_GPIO_PIN				GPIO_Pin_13
#define BUTTONControl_SetClock		RCC_AHB1Periph_GPIOC

#define SYSCFG_Clock				RCC_APB2Periph_SYSCFG
uint8_t Status = 0;

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/


/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/

/******************************************************************************/
/*                            PRIVATE FUNCTIONS                               */
/******************************************************************************/
static void delay(void);
static void Led_Init(void);
static void Interrupt_Init(void);
static void LedControl_SetStatus(GPIO_TypeDef *GPIOx, uint16_t GPIO_PIN, uint8_t Status);
/**
 * @brief   Create a simple delay using busy-wait loop
 *
 * @param   None
 *
 * @return  None
 */
static void delay(void)
{
	for(uint32_t i = 0; i < 500000; i++);
}
/**
 * @brief   Initialize LED GPIO (PA5)
 *
 * @param   None
 *
 * @return  None
 */
static void Led_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(LEDControl_SetClock, ENABLE);

	GPIO_InitStructure.GPIO_Pin = LED_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;

	GPIO_Init(LED_GPIO_PORT, &GPIO_InitStructure);
}
/**
 * @brief   Initialize external interrupt for button (PC13)
 *
 * @param   None
 *
 * @return  None
 */
static void Interrupt_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(BUTTONControl_SetClock, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Pin = BUTTON_GPIO_PIN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(BUTTON_GPIO_PORT, &GPIO_InitStructure);

	//Enable Clock Syscfg, Connect EXTI Line13 to PC13 Pin
	RCC_APB2PeriphClockCmd(SYSCFG_Clock, ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource13);

	//Configure EXTI Line13
	EXTI_InitStructure.EXTI_Line = EXTI_Line13;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;

	EXTI_Init(&EXTI_InitStructure);

	//Enable and set EXTI Line13 Interrupt to the lowest priority
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
/**
 * @brief   Set LED status (ON/OFF)
 *
 * @param   GPIOx    GPIO port (e.g., GPIOA)
 * @param   GPIO_PIN GPIO pin number
 * @param   Status   LED state (GPIO_PIN_SET or GPIO_PIN_RESET)
 *
 * @return  None
 */
static void LedControl_SetStatus(GPIO_TypeDef *GPIOx, uint16_t GPIO_PIN, uint8_t Status)
{
	if(Status == GPIO_PIN_SET)
	{
		GPIOx->BSRRL = GPIO_PIN;
	}
	if(Status == GPIO_PIN_RESET)
	{
		GPIOx->BSRRH = GPIO_PIN;
	}
}

/******************************************************************************/
/*                            INTERRUPT HANDLERS                              */
/******************************************************************************/
/**
 * @brief   Initialize external interrupt for button (PC13)
 *
 * @param   None
 *
 * @return  None
 */
void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetFlagStatus(EXTI_Line13) == SET)
	{
		//Kiem tra su kien ngat co dung tren chan PC13 hay khong
		Status = !Status;
	}
	//Xoa co ngat sau khi thuc hien chuong trinh ngat
	EXTI_ClearITPendingBit(EXTI_Line13);
}

/******************************************************************************/
/*                            EXPORTED FUNCTIONS                              */
/******************************************************************************/
int main()
{
	SystemCoreClockUpdate();
	Led_Init();
	Interrupt_Init();
	while(1)
	{
		if(Status == 0)
		{
			delay();
			LedControl_SetStatus(LED_GPIO_PORT, LED_GPIO_PIN, GPIO_PIN_LOW);
		}
		else
		{
			LedControl_SetStatus(LED_GPIO_PORT, LED_GPIO_PIN, GPIO_PIN_HIGH);
		}
	}

	return 0;
}
/******************************************************************************/
