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
#include "can.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "bmp280.h"
#include "adxl345.h"
#include "stdio.h"
#include "mpu9250.h"
#include "stdio.h"
#include "string.h"
#include <stdlib.h>
#include <math.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MAX_BUFFER_SIZE 100 
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
char *token;
char spr_buffer[50];
char rxBuffer[MAX_BUFFER_SIZE];
int temp_diff;
int value;
float Angle;
float coefficient_K = 255;
uint8_t length = 0x02;    
uint8_t Tx_Error = 100 ;
uint8_t rxData;
uint8_t  can_buf[2];
uint16_t rxIndex = 0;
uint16_t *p_gyro = gyro;
uint16_t *p_accel = accel;
uint16_t *p_magnet = magnet;
uint16_t gyro[3], accel[3],magnet[3];
uint32_t comp_temp_ptr = 0;
uint32_t new_comp_temp_ptr = 0;
uint32_t comp_presse_ptr = 0;
struct RawData rawData ;
struct GyroCal gyroCal ;
struct SensorData sensorData;
struct Attitude attitude;
CAN_TxHeaderTypeDef Tx_pHeader;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
uint32_t CAN_TX_Message(uint8_t TxData[], uint8_t length);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/**
 * @brief Processing the string received from a serial port interrup
 * 
 * @param data  rxBuffer
 */
void UART_RX_Complete_Callback(char* data) {
    //printf("Received: %s\r\n", data);
    if(!strcmp( data , "GET_T\n"))
    {
    	  read_bmp280_temperature(&comp_temp_ptr);
    	  memset(spr_buffer, 0, sizeof(spr_buffer));
    	  sprintf(spr_buffer , "T=+%02d.%02d_C\r\n", (int)comp_temp_ptr/100, (int)(comp_temp_ptr % 100));
    	  printf("send_temp_data: %s\n", spr_buffer);
    	  HAL_UART_Transmit(&huart1,(uint8_t *)spr_buffer,strlen(spr_buffer),0xffff);
    }
    else if(!strcmp( data , "GET_P\n"))
    {
    	  read_bmp280_presse(&comp_presse_ptr);
    	  memset(spr_buffer, 0, sizeof(spr_buffer));
    	  sprintf(spr_buffer , "P=%06dPa\r\n",(int)comp_presse_ptr/100);
    	  printf("send_press_data: %s\n", spr_buffer);
    	  HAL_UART_Transmit(&huart1,(uint8_t *)spr_buffer,strlen(spr_buffer),0xffff);
    }
    else if(!strcmp( data , "GET_A\n"))
       {
    	 //MPU_calcAttitude();
       	  memset(spr_buffer, 0, sizeof(spr_buffer));
       	  sprintf(spr_buffer , "A=%7.4f\r\n", Angle);
          printf("send_Angle_data: %s\n", spr_buffer);
       	  HAL_UART_Transmit(&huart1,(uint8_t *)spr_buffer,strlen(spr_buffer),0xffff);
       }
    else if (!strcmp( data , "GET_K\n"))
    {
      	  memset(spr_buffer, 0, sizeof(spr_buffer));
          sprintf(spr_buffer , "K=%7.5f\r\n",coefficient_K);
          printf("k: %s\n", spr_buffer);
          HAL_UART_Transmit(&huart1,(uint8_t *)spr_buffer,strlen(spr_buffer),0xffff);
    }
    else if(sscanf(data, "SET_K=%d", &value) == 1 && value >= 0 && value < 256)
    {
    	  coefficient_K = (float) value / 100 ;
    	  printf("K is valid  %f\n", coefficient_K );
          memset(spr_buffer, 0, sizeof(spr_buffer));
          sprintf(spr_buffer , "SET_K=OK\r\n");
          HAL_UART_Transmit(&huart1,(uint8_t *)spr_buffer,strlen(spr_buffer),0xffff);
    }
    else
    {
		  memset(spr_buffer, 0, sizeof(spr_buffer));
		  sprintf(spr_buffer , "order  err\r\n");
	      printf(" %s\n", spr_buffer);
	      HAL_UART_Transmit(&huart1,(uint8_t *)spr_buffer,strlen(spr_buffer),0xffff);
    }

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        rxBuffer[rxIndex++] = rxData;
       if (rxData == '\n') { 
        rxBuffer[rxIndex] = '\0'; 
        rxIndex = 0;         
        //printf("Received data: %s\n", rxBuffer);
        UART_RX_Complete_Callback(rxBuffer);
        }
        HAL_UART_Receive_IT(&huart1, &rxData, 1);
    }
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

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
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_USART1_UART_Init();
  MX_CAN1_Init();
  /* USER CODE BEGIN 2 */
  MPU9250_Init();
  BMP280_Init();
  HAL_UART_Receive_IT(&huart1, &rxData, 1);
  MPU_calibrateGyro(1500);
  HAL_CAN_Start(&hcan1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  read_bmp280_temperature(&comp_temp_ptr);
  HAL_Delay(1000);
  read_bmp280_temperature(&new_comp_temp_ptr);
  temp_diff = abs(new_comp_temp_ptr - comp_temp_ptr);
  Angle = round(((float)temp_diff/10) * ((float)coefficient_K));
  can_buf[0] = (Angle > UINT8_MAX) ? UINT8_MAX : (uint8_t)Angle;
  printf("can_buf[0]: %x\n",  can_buf[0]);
  can_buf[1] = 0x00;
  CAN_TX_Message(can_buf, length) ;
  can_buf[1] = 0x01;
  printf("can_buf[0]: %x\n",  can_buf[0]);
  CAN_TX_Message(can_buf, length) ;

  //MPU_calcAttitude();
  //printf(" R : %.1f,   P :  %.1f,   Y :  %f\r\n", attitude.r, attitude.p, attitude.y );
  //ADXL345_Data_Show();
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
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
