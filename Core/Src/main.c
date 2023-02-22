/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "protocol.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  char protocol;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  protocol = 0x00;
	  //protocol vom Terminal empfangen. Die länge der empfangene Protocol ist 1
	  HAL_UART_Receive(&huart1, &protocol, 1, 100);

	  if(protocol == PROTOCOL_READ_TEMP){
		  char ch[] = "Sie haben die Temperatur abgefragt.\n\rDruecken Sie die Taste \"Enter\" um den Temperaturwert zu lesen.\n\r";
		  HAL_UART_Transmit(&huart1, ch, sizeof(ch), 100);

	  }else if(protocol == PROTOCOL_WRITE_TEMP){
		  char ch[] = "Sie versuchen gerade ein neuen Temperaturwert zu schreiben.\n\rDruecken Sie die Leertaste dann geben Sie den Wert ein \n\rund druecken Sie zum Schluss die Taste \"Enter\".\n\r";
		  HAL_UART_Transmit(&huart1, ch, sizeof(ch), 100);
	  }else if(protocol != 0x00){
		  //ungültige Protokoll
		 char error[] = "Ungueltige Protokoll. Geben ein gueltiger Protokoll ein. \n\r- Geben Sie \"r\" (0x72) ein, um der Temperaturwert zu lesen\n\r \
 oder \"w\" (0x77) um der neuen Temperaturwert zu schreiben.\n\n\r";
		 HAL_UART_Transmit(&huart1, error, sizeof(error), 100);
	  }

	  switch(protocol){

	  case PROTOCOL_WRITE_TEMP:
	  {
		  char c = 0;
		  uint32_t beforeComma = 0;
		  uint32_t afterComma = 0;
		  uint32_t counterBefore = 0;
		  uint32_t counterAfter = 0;
		  uint32_t digitCntAfter;
		  uint8_t firstCount = 0;

		  while(c != LEERTASTE){
			 //geben sie die Leertaste ein
			 HAL_UART_Receive(&huart1, &c, 1, 100);
			 if((c != 0) && (c != LEERTASTE)){
				 char error[] = "Falsche Taste. Druecken Sie die Leertaste\n\r";
				 HAL_UART_Transmit(&huart1, error, sizeof(error), 100);
				 c = 0;
			 }
		  }
		  uint8_t commaCount = 0;
		  while(c != ENTER){
			 //geben sie den Wert ein
			  c = 0;
			 HAL_UART_Receive(&huart1, &c, 1, 100);
			 firstCount++;

			 if(firstCount == 1){
				 continue;
			 }
			 if(((c >= 0x30) && (c <= 0x39)) || (c == 0x2E) || (c == 0x2C)){
				 //es darf nur ein Komma im dezimalzahl sein
				 if((c == 0x2E) || (c == 0x2C)){
					 commaCount++;
					 if(commaCount > 1){
						 //invalid
						 char error[] = "Es darf nicht mehr als ein komma geben\n\r";
						 HAL_UART_Transmit(&huart1, error, sizeof(error), 100);
						 commaCount = 1;
						 continue;
					 }
				 }else{

					 if(commaCount == 0){
						 uint32_t x = pow(10, counterBefore);
						 uint32_t y = c - '0';
						 beforeComma = beforeComma*10 + (c - 0x30);
						 counterBefore++;

					 }else if (commaCount == 1){
						 afterComma = afterComma*10 + (c - 0x30);
						 counterAfter++;
					 }

				 }

			 }else if((c != 0x00) && (c != ENTER)){
				 //ungültige wert
				 char error[] = "Ungueltige wert. Geben ein gueltige Zahl ein\n\r";
				 HAL_UART_Transmit(&huart1, error, sizeof(error), 100);
			 }

		  }

		  digitCntAfter = getDigitCount(afterComma);
		  float value = beforeComma; // + (afterComma /(10^counterAfter));
		  float temp = (float)afterComma;
		  uint8_t i;
		  for(i = 0; i<digitCntAfter; i++){
			  temp /= 10;
		  }
		  if(afterComma != 0)
			  value += temp;

		  setTemp(value);
		  {
			uint32_t totalSize = counterBefore + counterAfter + 1;
			char result[totalSize];
			char str1[] = "Die Tempeartur wird gesetzt auf ";
			char str2[] = "°C\n\n\r";
			decToCharArry(beforeComma, counterBefore, afterComma, counterAfter, result);
			if(afterComma == 0) totalSize = counterBefore;
			HAL_UART_Transmit(&huart1, str1, sizeof(str1), 100);
			HAL_UART_Transmit(&huart1, result, totalSize, 100);
			HAL_UART_Transmit(&huart1, str2, sizeof(str2), 100);

		  }
		  break;
	  }
	  case PROTOCOL_READ_TEMP:
	  {
		  //Temperatur lesen
		  char c = 0;
		  float value;
		  uint8_t digitCount;
		  uint32_t beforeComma, afterComma;
		  while(c != ENTER){
			  //drücken sie die taste Enter
			  c = 0x00;
			  HAL_UART_Receive(&huart1, &c, 1, 100);
			  if(c == 0x0D){
				 char str[] = "Die gelesene Temperatur ist: ";
				 char end[] = "°C\n\n\r";
				 value = readTemp();
				 beforeComma =  getHexBeforeComma(value);
				 afterComma = getHexAfterComma(value);
				 digitCount = getDigitCount(beforeComma) + getDigitCount(afterComma) + 1;

				 char floatStr[digitCount];
				 //floatStr[digitCount] = '\0';

				 toArray(beforeComma, afterComma, digitCount, floatStr);

				 //HAL_UART_Transmit(&huart1, end, sizeof(end), 100);
				 HAL_UART_Transmit(&huart1, str, sizeof(str), 100);
				 HAL_UART_Transmit(&huart1, floatStr, digitCount, 100);
				 HAL_UART_Transmit(&huart1, end, sizeof(end), 100);
			  }
		  }
		  //HAL_UART_Transmit(&huart1, "Die Tempeartur wird gelesen\n\r", 29, 100);
		  break;
	  }
	  default:
		  break;

	  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
