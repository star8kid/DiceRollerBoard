/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#include <stdio.h>
#include <stdint.h>
#include "ssd1306_Custom.h"
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
// Define "#defs" for the MPU

/* MPU-6050 Defs */
#define MPU6050_ADDR_I2C (0x68 << 1)
#define MPU6050_PROFILER_THRESH 25

/* MPU-6050 Register Map Consts */

// I2C Registers
#define MPU6050_REG_I2C_MST_CTRL 0x24
#define MPU6050_REG_I2C_MST_STATUS 0x36

// Control / Power Management Registers
#define MPU6050_REG_USER_CTRL 0x6A
#define MPU6050_REG_PWR_MGMT_1 0x6B

// Accelerometer Registers
#define MPU6050_REG_ACCEL_CONFIG 0x1C
#define MPU6050_REG_ACCEL_XOUT_H 0x3B
#define MPU6050_REG_ACCEL_XOUT_L 0x3C
#define MPU6050_REG_ACCEL_YOUT_H 0x3D
#define MPU6050_REG_ACCEL_YOUT_L 0x3E
#define MPU6050_REG_ACCEL_ZOUT_H 0x3F
#define MPU6050_REG_ACCEL_ZOUT_L 0x40
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
// MPU-6050 Acceleration Variables Struct
typedef struct{
	int16_t accel_X_RAW, accel_Y_RAW, accel_Z_RAW;
} mpu6050_AccelStruct;

uint8_t mpu6050_profiler = 0;
uint8_t mpu6050_measurement_flag = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

// Retargetting Code for UART (Only for testing purposes across pins PA2 and PA3)
int __io_putchar(int ch){
	HAL_UART_Transmit(&huart2, (uint8_t*)&ch, 1, HAL_MAX_DELAY);
	return ch;
}
/* MPU-6050 Function Defs */
HAL_StatusTypeDef mpu6050_Wake(){
	// To wake the device, write a 0 to the SLEEP bit field in the PWR_MGMT_1 register in the memory map
	uint8_t dataW = 0x00;
	// Write to the register and return the state of the I2C Communication
	// Here, `I2C_MEMADD_SIZE_8BIT` means the total size of the memory space of the register
	// Blocking Mode
	return HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR_I2C, MPU6050_REG_PWR_MGMT_1, I2C_MEMADD_SIZE_8BIT, &dataW, 1, HAL_MAX_DELAY);
	// Use Master TX Function instead
//	return HAL_I2C_Master_Transmit(hi2c, DevAddress, pData, Size, Timeout)
//	return HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDR_I2C, MPU6050_REG_PWR_MGMT_1, &dataW, 1, HAL_MAX_DELAY);
	// Non-Blocking (Interrupt) Mode
//	return HAL_I2C_Mem_Write_IT(&hi2c1, MPU6050_ADDR_I2C, MPU6050_REG_PWR_MGMT_1, 1, &dataW, 1);
}

HAL_StatusTypeDef mpu6050_Sleep(){
	// To sleep the device, write a 1 to the SLEEP bit field in the PWR_MGMT_1 register in the memory map
	uint8_t dataW = 0x70;
	return HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR_I2C, MPU6050_REG_PWR_MGMT_1, I2C_MEMADD_SIZE_8BIT, &dataW, 1, HAL_MAX_DELAY);
//	return HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDR_I2C, MPU6050_REG_PWR_MGMT_1, &dataW, 1, HAL_MAX_DELAY);
}


HAL_StatusTypeDef mpu6050_MeasureAccel(mpu6050_AccelStruct *mpuStructPtr){
	// To receive data, create a 6-Byte buffer to hold it
	uint8_t dataBuff[6];
	HAL_StatusTypeDef status;

	// Next, write to the first acceleration register
	status = HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR_I2C, MPU6050_REG_ACCEL_XOUT_H, I2C_MEMADD_SIZE_8BIT, dataBuff, 6, HAL_MAX_DELAY);
//	status = HAL_I2C_Master_Receive(hi2c, DevAddress, pData, Size, Timeout);
//	status = HAL_I2C_Master_Receive(&hi2c1, MPU6050_ADDR_I2C, pData, Size, Timeout);
	// Good Practice: If the status of the comms is not okay, return status
	if(status != HAL_OK) return status;

	// Next, redefine the values in the acceleration struct
	mpuStructPtr->accel_X_RAW = (int16_t)((dataBuff[0] << 8) | dataBuff[1]);
	mpuStructPtr->accel_Y_RAW = (int16_t)((dataBuff[2] << 8) | dataBuff[3]);
	mpuStructPtr->accel_Z_RAW = (int16_t)((dataBuff[4] << 8) | dataBuff[5]);

	return HAL_OK;
}

/* OLED Display (SSD1306 IC) Function Defs */
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
  /* USER CODE BEGIN 2 */
//  mpu6050_AccelStruct mpu6050StructInst;
//	// Wake MPU, Produce three acceleration measurements, then sleep MPU
//  HAL_StatusTypeDef mpuWakeStatus = mpu6050_Sleep();
//  mpuWakeStatus = mpu6050_Wake();
////  HAL_StatusTypeDef mpuWakeStatus = mpu6050_Wake();
//	if(mpuWakeStatus != HAL_OK){
//		// Execute a busy-wait loop if the MPU is not initialized properly
//		while(1){}
//	}

	// Define For-Loop to take a discrete number of measurements
//	for (uint8_t iter = 0; iter < MPU6050_PROFILER_THRESH; iter++){
//		if(mpu6050_MeasureAccel(&mpu6050StructInst) == HAL_OK){
//			printf("Accel X: %X, Accel Y: %X, Accel Z: %X \n\r", mpu6050StructInst.accel_X_RAW, mpu6050StructInst.accel_Y_RAW, mpu6050StructInst.accel_Z_RAW);
//			//			  // Todo: Double check with the above results to see what sensitivity has been selected
//			//			  // AFS_SEL = 0 ---> 0x4000 = +g
//			//			  // AFS_SEL = 1 ---> 0x2000 = +g
//		}
//		else{
//			printf("Error with measurement (!= HAL_OK) \n\r");
//			break;
//		}
//		HAL_Delay(500);
//	}
//	mpu6050_Sleep();
//	printf("MPU-6050 Measurement Over! \n\r");

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 16;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable MSI Auto calibration
  */
  HAL_RCCEx_EnableMSIPLLMode();
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x0060112F;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

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
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
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
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
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
#ifdef USE_FULL_ASSERT
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
