/*

University of East Anglia
Year 3 - Embedded Systems Coursework
Thomas Yeomans
Automatic Drinks Dispenser

Useful links:
https://easyeda.com/components/5011AS-7-Segment-Display_0a0ce76fbaa8485891155d0c10b4e91b
https://www.st.com/resource/en/user_manual/dm00190424-discovery-kit-for-stm32f7-series-with-stm32f746ng-mcu-stmicroelectronics.pdf - Page 23
https://www.electronics-tutorials.ws/blog/7-segment-display-tutorial.html
http://www.soatmon.com/2021/09/no-contact-fluid-sensor.html
https://www.st.com/resource/en/application_note/an4759-using-the-hardware-realtime-clock-rtc-and-the-tamper-management-unit-tamp-with-stm32-microcontrollers-stmicroelectronics.pdf

Components list:
1 - GLCD Touchscreen, STM32F746NG board
2 - 12V air pumps
1 - Piezo Buzzer
2 - Non Contact Liquid Level Sensors
2/4 - LEDs
2 - 5V Relays

Pins:
D0 -> Water pump on/off
D1 -> Water level sensor
D2 -> Buzzer on/off
D3 -> Apple pump on/off
D4 -> Apple level sensor

*/

#include <stdio.h>
#include <string.h>
#include "stm32f7xx_hal.h"
#include "GLCD_Config.h"
#include "Board_GLCD.h"
#include "Board_Touch.h"
#include "Board_LED.h"           
#define wait_delay HAL_Delay
extern GLCD_FONT GLCD_Font_6x8;
extern GLCD_FONT GLCD_Font_16x24;

#ifdef __RTX
extern uint32_t os_time;
uint32_t HAL_GetTick(void) {
	return os_time;
}
#endif

// Pins D0, D1, D2, D3 & D4
GPIO_InitTypeDef gpioD0; // Water pump
GPIO_InitTypeDef gpioD1; // Water level sensor
GPIO_InitTypeDef gpioD2; // Buzzer
GPIO_InitTypeDef gpioD3; // Apple pump
GPIO_InitTypeDef gpioD4; // Apple level sensor

void initialisePins(void) {
	
	// Enabling clock for C, G and B base
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	// Set mode as output, nopull
	gpioD0.Mode = GPIO_MODE_OUTPUT_PP;
  gpioD0.Pull = GPIO_NOPULL;
  gpioD0.Speed = GPIO_SPEED_HIGH;
  gpioD0.Pin = GPIO_PIN_7;
	
	// Set mode as output, nopull
	gpioD1.Mode = GPIO_MODE_OUTPUT_PP;
  gpioD1.Pull = GPIO_NOPULL;
  gpioD1.Speed = GPIO_SPEED_HIGH;
  gpioD1.Pin = GPIO_PIN_6;
	
	// Set mode as output, nopull
	gpioD2.Mode = GPIO_MODE_OUTPUT_PP;
  gpioD2.Pull = GPIO_NOPULL;
  gpioD2.Speed = GPIO_SPEED_HIGH;
  gpioD2.Pin = GPIO_PIN_6;
	
	// Set mode as output, nopull
	gpioD3.Mode = GPIO_MODE_OUTPUT_PP;
  gpioD3.Pull = GPIO_NOPULL;
  gpioD3.Speed = GPIO_SPEED_HIGH;
  gpioD3.Pin = GPIO_PIN_4;
	
	// Set mode as output, nopull
	gpioD4.Mode = GPIO_MODE_OUTPUT_PP;
  gpioD4.Pull = GPIO_NOPULL;
  gpioD4.Speed = GPIO_SPEED_HIGH;
  gpioD4.Pin = GPIO_PIN_7;
	
	// Initialising pins (D0, D1, D2, D3 & D4)
	HAL_GPIO_Init(GPIOC, &gpioD0);
	HAL_GPIO_Init(GPIOC, &gpioD1);
	HAL_GPIO_Init(GPIOG, &gpioD2);
	HAL_GPIO_Init(GPIOB, &gpioD3);
	HAL_GPIO_Init(GPIOG, &gpioD4);
	
}

void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	/* Enable Power Control clock */
	__HAL_RCC_PWR_CLK_ENABLE();
	/* The voltage scaling allows optimizing the power
	consumption when the device is clocked below the
	maximum system frequency. */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	/* Enable HSE Oscillator and activate PLL
	with HSE as source */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 25;
	RCC_OscInitStruct.PLL.PLLN = 336;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 7;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);
	/* Select PLL as system clock source and configure
	the HCLK, PCLK1 and PCLK2 clocks dividers */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK | 
	RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}

// Simple delay 
void wait(int delay) {
	
	int i;
  for(i=0; i<delay ;i++);
}

void introScreen(void) {
	
	// Declares build date and time
	char date[] = __DATE__;
	char time[] = __TIME__;
	
	GLCD_DrawString(0, 0,  "EMBEDDED SYSTEMS COURSEWORK");
	GLCD_DrawString(0, 30, "THOMAS YEOMANS - UEA");
	
	GLCD_DrawString(0, 100, "BUILD DATE AND TIME:");
	GLCD_DrawString(0, 130, date);
	GLCD_DrawString(0, 160, time);
	
	GLCD_DrawString(0, 230, "V0.4");
	
	wait(200000000); 
	
}

void menuScreen(void) {
	
	HAL_Init(); 					// Initialise Hardware Abstraction Layer
  SystemClock_Config(); // Config Clocks
  GLCD_Initialize();    // Initialise GLCD
	
	GLCD_SetFont(&GLCD_Font_16x24);
	
	GLCD_SetBackgroundColor(GLCD_COLOR_BLUE);
  GLCD_SetForegroundColor(GLCD_COLOR_WHITE);
	
  GLCD_DrawString(0, 0, "  Automatic Drinks Dispenser  ");
	
	GLCD_SetBackgroundColor(GLCD_COLOR_WHITE);
  GLCD_SetForegroundColor(GLCD_COLOR_BLUE);
	
	GLCD_DrawRectangle(105, 135, 105, 50);
	GLCD_DrawString(120, 150, "Water");

	GLCD_DrawRectangle(265, 135, 105, 50);
	GLCD_DrawString(280, 150, "Apple");
	
}

void playBuzzer() {

	// Turn pin D2 on
	HAL_GPIO_WritePin(GPIOG, gpioD2.Pin, GPIO_PIN_SET);
	
	// Play buzzer after pouring for 1 second
	wait(100000000);

	// Turn pin D2 off to stop pouring
	HAL_GPIO_WritePin(GPIOG, gpioD2.Pin, GPIO_PIN_RESET);
	
}

void pourWater() {

	// Turn pin D0 on
	HAL_GPIO_WritePin(GPIOC, gpioD0.Pin, GPIO_PIN_SET);
	
	// Wait for 10 seconds for the drink to pour
	wait(1000000000);
	
	// Turn pin D0 off to stop pouring
	HAL_GPIO_WritePin(GPIOC, gpioD0.Pin, GPIO_PIN_RESET);
	
}

void pourApple() {

	// Turn pin D3 on
	HAL_GPIO_WritePin(GPIOB, gpioD3.Pin, GPIO_PIN_SET);
	
	// Wait for 10 seconds for the drink to pour
	wait(1000000000);
	
	// Turn pin D3 off to stop pouring
	HAL_GPIO_WritePin(GPIOB, gpioD3.Pin, GPIO_PIN_RESET);
	
}

void pouringDrink(int drink) {

	if (drink == 1) {
		GLCD_DrawString(100, 150, " Pouring water... ");
		
		GLCD_SetBackgroundColor(GLCD_COLOR_BLUE);
		GLCD_SetForegroundColor(GLCD_COLOR_WHITE);
		GLCD_DrawString(0, 0, "  Automatic Drinks Dispenser  ");
		
		pourWater();
		
		GLCD_SetBackgroundColor(GLCD_COLOR_WHITE);
		GLCD_SetForegroundColor(GLCD_COLOR_BLUE);
		GLCD_DrawString(70, 150, " Successfully poured! ");
		
		playBuzzer();
		
		wait(300000000);
	}
	
	if (drink == 2) {
		
		GLCD_DrawString(100, 150, " Pouring apple... ");
		
		GLCD_SetBackgroundColor(GLCD_COLOR_BLUE);
		GLCD_SetForegroundColor(GLCD_COLOR_WHITE);
		GLCD_DrawString(0, 0, "  Automatic Drinks Dispenser  ");
		
		pourApple();
		
		GLCD_SetBackgroundColor(GLCD_COLOR_WHITE);
		GLCD_SetForegroundColor(GLCD_COLOR_BLUE);
		GLCD_DrawString(70, 150, " Successfully poured! ");
		
		playBuzzer();
		
		wait(300000000);
	
	}
}

void checkCoordsDrinks(int x, int y) {
		
		// Check for Water option
		if ((x>=115) && (x<=200) && (y>=140) && (y<=180)) {
			GLCD_ClearScreen();
			pouringDrink(1);
			GLCD_ClearScreen();
			menuScreen();
		}
		
		// Check for Apple option
		if ((x>=270) && (x<=370) && (y>=140) && (y<=180)) {
			GLCD_ClearScreen();
			pouringDrink(2);
			GLCD_ClearScreen();
			menuScreen();
		}
}

int main(void){
	
	TOUCH_STATE tsc_state; 
	char buffer[128];
	
	HAL_Init(); //Init Hardware Abstraction Layer
	SystemClock_Config(); //Config Clocks
	Touch_Initialize();
	GLCD_Initialize(); //Initialise GLCD
	GLCD_SetFont(&GLCD_Font_16x24);
	
	initialisePins();
	
	GLCD_ClearScreen(); // Clears screen on startup
	
	introScreen();
	GLCD_ClearScreen();
	menuScreen();	
	
	// Turn pin D1 on - water level sensor
	HAL_GPIO_WritePin(GPIOC, gpioD1.Pin, GPIO_PIN_SET);
	
	// Turn pin D4 on - apple level sensor
	HAL_GPIO_WritePin(GPIOG, gpioD4.Pin, GPIO_PIN_SET);
	
	// Constantly check for user input - GLCD Touchscreen
	for(;;) {
			Touch_GetState(&tsc_state);
			if(tsc_state.pressed) {
				sprintf(buffer , "%i , %i", tsc_state.x, tsc_state.y);
				//GLCD_DrawString(10, 60, buffer); // Shows coordinates on screen, useful for checking input
				checkCoordsDrinks(tsc_state.x, tsc_state.y);
			}
		}
}
