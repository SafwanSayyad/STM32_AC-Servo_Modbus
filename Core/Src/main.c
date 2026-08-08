/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  * Author			: Safwan Sayyad
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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "modbus_rtu.h"
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define PATH_BASE       0x6200
#define PATH_STRIDE     8
#define PULSES_PER_REV  10000
#define TARGET_RPM      1000
#define ACCEL_TIME      150
#define DECEL_TIME      150
#define PAUSE_MS        500

#define REG_SIMULATED_IO 0x0035
#define REG_PR_TRIGGER   0x6002
#define TRIGGER_PR0      0x0010
#define EMERGENCY_STOP   0x0040

#define TYPE_POSITIONING   1
#define RELATIVE_COMMAND   (1 << 6)
#define JUMP_ENABLE         (1 << 14)


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
void test_read_mode(void);
void test_velocity_mode(void);
void test_pr_16_paths(void);
void test_torque_mode(void);
void test_setup_velocity_mode(void);
void test_setup_pr_mode(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int __io_putchar(int ch)
{
    HAL_UART_Transmit(&huart2, (uint8_t*)&ch, 1, HAL_MAX_DELAY);
    return ch;
}

volatile int stop_requested = 0;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_13) {
        stop_requested = 1;
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
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  /*
   * HOW TO SWITCH MODES:
   * 1. Uncomment ONE test_setup_XXX_mode() call below, comment rest out
   * 2. Build, flash, run
   * 3. POWER-CYCLE THE DRIVE (not the Nucleo)
   * 4. Comment the setup function back out
   * 5. Uncomment the actual test you want (e.g. test_pr_16_paths())
   * 6. Build, flash, run
   *
   * Only ONE line should be uncommented at a time.
   */


  test_read_mode();

  //test_setup_velocity_mode();
  //test_velocity_mode();

  //test_setup_pr_mode();
  //test_pr_16_paths();

  //test_setup_torque_mode();
  //test_torque_mode();


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
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
  huart1.Init.StopBits = UART_STOPBITS_2;
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
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void test_read_mode(void)
{
    Modbus_Init(&huart1);
    uint16_t mode = 0;
    Modbus_ReadRegister(0x0003, &mode);
    printf("Current P00.01 = %d\r\n", mode);
}

void test_velocity_mode(void)
{
    Modbus_Init(&huart1);

    Modbus_WriteRegister(0x0319, 150);   /* P03.12, acceleration time */
    HAL_Delay(100);
    Modbus_WriteRegister(0x031B, 150);   /* P03.13, deceleration time */
    HAL_Delay(100);

    Modbus_WriteRegister(0x0309, 100);   /* P03.04, target rpm */
    HAL_Delay(200);

    Modbus_WriteRegister(0x0035, (1 << 4));   /* servo on */
    printf("Servo enabled, running at 100 rpm...\r\n");
    HAL_Delay(5000);

    /* Ramp speed down to 0 BEFORE disabling - avoids the dynamic brake
     * hard-stop, lets P03.13's deceleration profile actually apply */
    Modbus_WriteRegister(0x0309, 0);
    printf("Ramping down to 0 rpm...\r\n");
    HAL_Delay(2000);   /* give it time to actually decelerate and settle */

    Modbus_WriteRegister(0x0035, 0x0000);   /* servo off, now safe/gentle */
    printf("Servo disabled.\r\n");
}

void test_torque_mode(void)
{
    Modbus_Init(&huart1);

    /* Torque source = use P03.22 directly over RS485 (not analog input) */
    Modbus_WriteRegister(0x0323, 2);   /* P03.17 */
    HAL_Delay(100);

    /* Safety clamp: even with nothing resisting the shaft, it can't spin
     * faster than this. Keep LOW while hand-testing. */
    Modbus_WriteRegister(0x032B, 30);   /* P03.21, velocity limit in torque mode */
    HAL_Delay(100);

    /* Start at 0 torque before enabling */
    Modbus_WriteRegister(0x032D, 0);   /* P03.22 */
    HAL_Delay(100);

    Modbus_WriteRegister(0x0035, (1 << 4));   /* servo on */
    HAL_Delay(300);
    printf("Servo enabled. Velocity limit = 30 rpm (safety clamp).\r\n\r\n");

    uint16_t torque_steps[3] = {5, 10, 20};
    for (int i = 0; i < 3; i++) {
        printf("Setting torque = %d%%, holding for 6s...\r\n", torque_steps[i]);
        printf("  -> Try holding/twisting the shaft by hand now.\r\n");
        Modbus_WriteRegister(0x032D, torque_steps[i]);
        HAL_Delay(6000);
    }

    printf("\r\nTest complete. Setting torque back to 0.\r\n");
    Modbus_WriteRegister(0x032D, 0);
    HAL_Delay(500);

    Modbus_WriteRegister(0x0035, 0x0000);   /* servo off */
    printf("Servo disabled.\r\n");
}

void test_setup_velocity_mode(void)
{
    Modbus_Init(&huart1);

    uint16_t current_mode = 0;
    Modbus_ReadRegister(0x0003, &current_mode);
    printf("Current P00.01 = %d\r\n", current_mode);
    HAL_Delay(200);

    Modbus_WriteRegister(0x0003, 1);   /* P00.01 = 1, Velocity mode */
    printf("P00.01 written = 1 (Velocity mode)\r\n");
    HAL_Delay(200);

    Modbus_WriteRegister(0x0033, 0x2211);   /* P00.25, save to EEPROM */
    printf("Saved to EEPROM. POWER-CYCLE THE DRIVE NOW.\r\n");
}

void test_setup_torque_mode(void)
{
    Modbus_Init(&huart1);

    uint16_t current_mode = 0;
    Modbus_ReadRegister(0x0003, &current_mode);
    printf("Current P00.01 = %d\r\n", current_mode);
    HAL_Delay(200);

    Modbus_WriteRegister(0x0003, 2);   /* P00.01 = 2, Torque mode */
    printf("P00.01 written = 2 (Torque mode)\r\n");
    HAL_Delay(200);

    Modbus_WriteRegister(0x0033, 0x2211);   /* P00.25, save to EEPROM */
    printf("Saved to EEPROM. POWER-CYCLE THE DRIVE NOW.\r\n");
}

void test_setup_pr_mode(void)
{
    Modbus_Init(&huart1);

    Modbus_WriteRegister(0x0003, 6);   /* P00.01 = 6, PR mode */
    printf("P00.01 written = 6 (PR mode)\r\n");
    HAL_Delay(200);

    Modbus_WriteRegister(0x0033, 0x2211);   /* P00.25, save to EEPROM */
    printf("Saved to EEPROM. POWER-CYCLE THE DRIVE NOW.\r\n");
}

static int32_t degrees_to_pulses(int32_t deg)
{
    return (deg * PULSES_PER_REV) / 360;
}

static uint16_t path_addr(int path_num, int offset)
{
    return PATH_BASE + (path_num * PATH_STRIDE) + offset;
}


void test_pr_16_paths(void)
{
    Modbus_Init(&huart1);
    stop_requested = 0;

    int32_t sequence[16] = {
        360, -360, 180, -180,
         90,  -90,  45,  -45,
        270, -270, 120, -120,
         60,  -60,  30,  -30,
    };

    int config_failed = 0;

    printf("Configuring 16 paths (looping forever until button press)...\r\n");
    for (int i = 0; i < 16; i++) {
        int next_path = (i + 1) % 16;   /* PR15 jumps back to PR0 - infinite loop */

        int32_t pulses = degrees_to_pulses(sequence[i]);
        uint16_t high = (uint16_t)((pulses >> 16) & 0xFFFF);
        uint16_t low  = (uint16_t)(pulses & 0xFFFF);
        uint16_t mode_value = TYPE_POSITIONING | RELATIVE_COMMAND |
                               JUMP_ENABLE | (next_path << 8);

        ModbusStatus r1 = Modbus_WriteRegister_Retry(path_addr(i, 0), mode_value);
        ModbusStatus r2 = Modbus_WriteRegister_Retry(path_addr(i, 1), high);
        ModbusStatus r3 = Modbus_WriteRegister_Retry(path_addr(i, 2), low);
        ModbusStatus r4 = Modbus_WriteRegister_Retry(path_addr(i, 3), (uint16_t)TARGET_RPM);
        ModbusStatus r5 = Modbus_WriteRegister_Retry(path_addr(i, 4), ACCEL_TIME);
        ModbusStatus r6 = Modbus_WriteRegister_Retry(path_addr(i, 5), DECEL_TIME);
        ModbusStatus r7 = Modbus_WriteRegister_Retry(path_addr(i, 6), PAUSE_MS);

        if (r1 || r2 || r3 || r4 || r5 || r6 || r7) {
            printf("PR%d: CONFIG FAILED, aborting before any motion.\r\n", i);
            config_failed = 1;
            goto cleanup;
        }
        printf("PR%d configured OK\r\n", i);
    }

    if (Modbus_WriteRegister_Retry(REG_SIMULATED_IO, (1 << 4)) != MODBUS_OK) {
        printf("FAILED to enable servo, aborting.\r\n");
        config_failed = 1;
        goto cleanup;
    }
    HAL_Delay(200);

    if (Modbus_WriteRegister_Retry(REG_PR_TRIGGER, TRIGGER_PR0) != MODBUS_OK) {
        printf("FAILED to trigger PR0, aborting.\r\n");
        config_failed = 1;
        goto cleanup;
    }

    printf("Triggered PR0. Running forever - press the USER button to stop.\r\n");

    while (!stop_requested) {
        HAL_Delay(50);   /* main loop just idles, waiting for the interrupt flag */
    }

    printf("Button pressed. Stopping.\r\n");

cleanup:
    Modbus_WriteRegister_Retry(REG_PR_TRIGGER, EMERGENCY_STOP);
    HAL_Delay(200);
    Modbus_WriteRegister_Retry(REG_SIMULATED_IO, 0x0000);

    if (config_failed) {
        printf("Stopped due to a failure above. Servo disabled.\r\n");
    } else {
        printf("Servo disabled.\r\n");
    }
}

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
