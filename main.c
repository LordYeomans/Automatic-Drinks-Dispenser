#include <stdio.h>
#include "stm32f7xx_hal.h"
#include "GLCD_Config.h"
#include "Board_GLCD.h"
#include "Board_Touch.h"
#include "Board_LED.h"           
#define wait_delay HAL_Delay
extern GLCD_FONT GLCD_Font_6x8;
extern GLCD_FONT GLCD_Font_16x24;

#define TIME 1000000

#ifdef __RTX
extern uint32_t os_time;
uint32_t HAL_GetTick(void) {
	return os_time;
}
#endif

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

void wait(int delay) {
	
	int i;
  for(i=0; i<40000000 ;i++);
}

void checkCoordsDrinks(int x, int y) {
	
		if ((x>=155) && (x<=305) && (y>=120) && (y<=200)) {
			
			GLCD_ClearScreen(); // TESTING
			
		}
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
	
	GLCD_DrawString(0, 230, "V0.11");
	
	wait(5000000); 
	
}

void menuScreen(void) {
	
	HAL_Init(); 					// Init Hardware Abstraction Layer
  SystemClock_Config(); // Config Clocks
  GLCD_Initialize();    // Init GLCD
	
	GLCD_SetBackgroundColor(GLCD_COLOR_BLUE);
  GLCD_SetForegroundColor(GLCD_COLOR_WHITE);
  GLCD_DrawString(0, 0, "  Automatic Drinks Dispenser  ");
  GLCD_SetBackgroundColor(GLCD_COLOR_WHITE);
  GLCD_SetForegroundColor(GLCD_COLOR_BLUE);
	
	GLCD_DrawString(150, 150, "  Drinks  ");
	GLCD_DrawRectangle(155, 120, 150, 80);
	
}

int main(void){
	
	TOUCH_STATE tsc_state; 
	char buffer[128];
	
	HAL_Init(); //Init Hardware Abstraction Layer
  SystemClock_Config(); //Config Clocks
	Touch_Initialize();
  GLCD_Initialize(); //Initialise GLCD
  GLCD_SetFont(&GLCD_Font_16x24);
	
	GLCD_ClearScreen(); // Clears screen on startup
	
  introScreen();
	GLCD_ClearScreen();
	menuScreen();
	
	for(;;) {
			Touch_GetState(&tsc_state);
			if(tsc_state.pressed) {
				sprintf(buffer , "%i , %i", tsc_state.x, tsc_state.y);
				//GLCD_DrawString(10, 60, buffer);	
				checkCoordsDrinks(tsc_state.x, tsc_state.y);
			}
		}
}
