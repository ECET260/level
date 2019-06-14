/**
  ******************************************************************************
  * File Name          :  stmicroelectronics_x-cube-mems1_6_2_0.c
  * Description        : This file provides code for the configuration
  *                      of the STMicroelectronics.X-CUBE-MEMS1.6.2.0 instances.
  ******************************************************************************
  *
  * COPYRIGHT 2019 STMicroelectronics
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  ******************************************************************************
  */

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "app_x-cube-mems1.h"
#include "main.h"
#include <stdio.h>

#include "custom_motion_sensors.h"
#include "lsm6dsl_settings.h"
#include "b_l475e_iot01a.h"
#include "math.h"

#include "GUI.h"

CUSTOM_MOTION_SENSOR_Axes_t acceleration;

/* Private typedef -----------------------------------------------------------*/
typedef struct displayFloatToInt_s {
  int8_t sign; /* 0 means positive, 1 means negative*/
  uint32_t  out_int;
  uint32_t  out_dec;
} displayFloatToInt_t;

/* Private define ------------------------------------------------------------*/
#define MAX_BUF_SIZE 256  

/* Private variables ---------------------------------------------------------*/
static volatile uint8_t PushButtonDetected = 0;
static uint8_t verbose = 1;  /* Verbose output to UART terminal ON/OFF. */
static CUSTOM_MOTION_SENSOR_Capabilities_t MotionCapabilities[CUSTOM_MOTION_INSTANCES_NBR];
static char dataOut[MAX_BUF_SIZE];
static int32_t PushButtonState = GPIO_PIN_RESET;

/* Private function prototypes -----------------------------------------------*/
static void floatToInt(float in, displayFloatToInt_t *out_value, int32_t dec_prec);
static void Accelero_Sensor_Handler(uint32_t Instance);
static void Gyro_Sensor_Handler(uint32_t Instance);
static void Magneto_Sensor_Handler(uint32_t Instance);
static void MX_DataLogTerminal_Init(void);
static void MX_DataLogTerminal_Process(void);

void MX_MEMS_Init(void)
{
  /* USER CODE BEGIN SV */ 

  /* USER CODE END SV */

  /* USER CODE BEGIN MEMS_Init_PreTreatment */
  
  /* USER CODE END MEMS_Init_PreTreatment */

  /* Initialize the peripherals and the MEMS components */

  MX_DataLogTerminal_Init();

  /* USER CODE BEGIN MEMS_Init_PostTreatment */
  
  /* USER CODE END MEMS_Init_PostTreatment */
}
/*
 * LM background task
 */
void MX_MEMS_Process(void)
{
  /* USER CODE BEGIN MEMS_Process_PreTreatment */
  
  /* USER CODE END MEMS_Process_PreTreatment */

  MX_DataLogTerminal_Process();

  /* USER CODE BEGIN MEMS_Process_PostTreatment */
  
  /* USER CODE END MEMS_Process_PostTreatment */
}

/**
  * @brief  Initialize the DataLogTerminal application
  * @retval None
  */
void MX_DataLogTerminal_Init(void)
{
  displayFloatToInt_t out_value_odr;
  int i;

  /* Initialize LED */
  BSP_LED_Init(LED2);

  /* Initialize button */
  BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_EXTI);

  /* Check what is the Push Button State when the button is not pressed. It can change across families */
  PushButtonState = (BSP_PB_GetState(BUTTON_KEY)) ?  0 : 1;

  /* Initialize Virtual COM Port */
  BSP_COM_Init(COM1);

  CUSTOM_MOTION_SENSOR_Init(CUSTOM_LSM6DSL_0, MOTION_ACCELERO | MOTION_GYRO);

  CUSTOM_MOTION_SENSOR_SetOutputDataRate(CUSTOM_LSM6DSL_0, MOTION_ACCELERO, LSM6DSL_ACC_ODR);

  CUSTOM_MOTION_SENSOR_SetFullScale(CUSTOM_LSM6DSL_0, MOTION_ACCELERO, LSM6DSL_ACC_FS);

  CUSTOM_MOTION_SENSOR_SetOutputDataRate(CUSTOM_LSM6DSL_0, MOTION_GYRO, LSM6DSL_GYRO_ODR);

  CUSTOM_MOTION_SENSOR_SetFullScale(CUSTOM_LSM6DSL_0, MOTION_GYRO, LSM6DSL_GYRO_FS);

  for(i = 0; i < CUSTOM_MOTION_INSTANCES_NBR; i++)
  {
    CUSTOM_MOTION_SENSOR_GetCapabilities(i, &MotionCapabilities[i]);
    snprintf(dataOut, MAX_BUF_SIZE,
             "\r\nMotion Sensor Instance %d capabilities: \r\n ACCELEROMETER: %d\r\n GYROSCOPE: %d\r\n MAGNETOMETER: %d\r\n LOW POWER: %d\r\n",
             i, MotionCapabilities[i].Acc, MotionCapabilities[i].Gyro, MotionCapabilities[i].Magneto, MotionCapabilities[i].LowPower);
    printf("%s", dataOut);
    floatToInt(MotionCapabilities[i].AccMaxOdr, &out_value_odr, 3);
    snprintf(dataOut, MAX_BUF_SIZE, " MAX ACC ODR: %d.%03d Hz, MAX ACC FS: %d\r\n", (int)out_value_odr.out_int,
             (int)out_value_odr.out_dec, (int)MotionCapabilities[i].AccMaxFS);
    printf("%s", dataOut);
    floatToInt(MotionCapabilities[i].GyroMaxOdr, &out_value_odr, 3);
    snprintf(dataOut, MAX_BUF_SIZE, " MAX GYRO ODR: %d.%03d Hz, MAX GYRO FS: %d\r\n", (int)out_value_odr.out_int,
             (int)out_value_odr.out_dec, (int)MotionCapabilities[i].GyroMaxFS);
    printf("%s", dataOut);
    floatToInt(MotionCapabilities[i].MagMaxOdr, &out_value_odr, 3);
    snprintf(dataOut, MAX_BUF_SIZE, " MAX MAG ODR: %d.%03d Hz, MAX MAG FS: %d\r\n", (int)out_value_odr.out_int,
             (int)out_value_odr.out_dec, (int)MotionCapabilities[i].MagMaxFS);
    printf("%s", dataOut);
  }

}

/**
  * @brief  BSP Push Button callback
  * @param  Button Specifies the pin connected EXTI line
  * @retval None.
  */
void BSP_PB_Callback(Button_TypeDef Button)
{
  PushButtonDetected = 1;
}

/**
  * @brief  Process of the DataLogTerminal application
  * @retval None
  */
void MX_DataLogTerminal_Process(void)
{
  int i;

  if (PushButtonDetected != 0U)
  {
    /* Debouncing */
    HAL_Delay(50);

    /* Wait until the button is released */
    while ((BSP_PB_GetState( BUTTON_KEY ) == PushButtonState));

    /* Debouncing */
    HAL_Delay(50);

    /* Reset Interrupt flag */
    PushButtonDetected = 0;

    /* Do nothing */
  }

  for(i = 0; i < CUSTOM_MOTION_INSTANCES_NBR; i++)
  {
    if(MotionCapabilities[i].Acc)
    {
      Accelero_Sensor_Handler(i);
    }
    if(MotionCapabilities[i].Gyro)
    {
      Gyro_Sensor_Handler(i);
    }
    if(MotionCapabilities[i].Magneto)
    {
      Magneto_Sensor_Handler(i);
    }
  }

//mel  HAL_Delay( 1000 ); //spped up refresh
  HAL_Delay( 50 );
}

/**
  * @brief  Splits a float into two integer values.
  * @param  in the float value as input
  * @param  out_value the pointer to the output integer structure
  * @param  dec_prec the decimal precision to be used
  * @retval None
  */
static void floatToInt(float in, displayFloatToInt_t *out_value, int32_t dec_prec)
{
  if(in >= 0.0f)
  {
    out_value->sign = 0;
  }else
  {
    out_value->sign = 1;
    in = -in;
  }

  in = in + (0.5f / pow(10, dec_prec));
  out_value->out_int = (int32_t)in;
  in = in - (float)(out_value->out_int);
  out_value->out_dec = (int32_t)trunc(in * pow(10, dec_prec));
}

/**
  * @brief  Handles the accelerometer axes data getting/sending
  * @param  Instance the device instance
  * @retval None
  */
static void Accelero_Sensor_Handler(uint32_t Instance)
{
  float odr;
  int32_t fullScale;
//  CUSTOM_MOTION_SENSOR_Axes_t acceleration;
  displayFloatToInt_t out_value;
  uint8_t whoami;

  if (CUSTOM_MOTION_SENSOR_GetAxes(Instance, MOTION_ACCELERO, &acceleration))
  {
    snprintf(dataOut, MAX_BUF_SIZE, "\r\nACC[%d]: Error\r\n", (int)Instance);
  }
  else
  {
    snprintf(dataOut, MAX_BUF_SIZE, "\r\nACC_X[%d]: %d, ACC_Y[%d]: %d, ACC_Z[%d]: %d\r\n", (int)Instance,
             (int)acceleration.x, (int)Instance, (int)acceleration.y, (int)Instance, (int)acceleration.z);
  }

  printf("%s", dataOut);

  if (verbose == 1)
  {
    if (CUSTOM_MOTION_SENSOR_ReadID(Instance, &whoami))
    {
      snprintf(dataOut, MAX_BUF_SIZE, "WHOAMI[%d]: Error\r\n", (int)Instance);
    }
    else
    {
      snprintf(dataOut, MAX_BUF_SIZE, "WHOAMI[%d]: 0x%x\r\n", (int)Instance, (int)whoami);
    }

    printf("%s", dataOut);

    if (CUSTOM_MOTION_SENSOR_GetOutputDataRate(Instance, MOTION_ACCELERO, &odr))
    {
      snprintf(dataOut, MAX_BUF_SIZE, "ODR[%d]: ERROR\r\n", (int)Instance);
    }
    else
    {
      floatToInt(odr, &out_value, 3);
      snprintf(dataOut, MAX_BUF_SIZE, "ODR[%d]: %d.%03d Hz\r\n", (int)Instance, (int)out_value.out_int,
               (int)out_value.out_dec);
    }

    printf("%s", dataOut);

    if (CUSTOM_MOTION_SENSOR_GetFullScale(Instance, MOTION_ACCELERO, &fullScale))
    {
      snprintf(dataOut, MAX_BUF_SIZE, "FS[%d]: ERROR\r\n", (int)Instance);
    }
    else
    {
      snprintf(dataOut, MAX_BUF_SIZE, "FS[%d]: %d g\r\n", (int)Instance, (int)fullScale);
    }

    printf("%s", dataOut);

    GUI_SetFont(&GUI_Font13HB_1);

//   GUI_DispStringAt("           ", 20, 130);
//    GUI_DispDecAt(acceleration.x, 20, 130, 4 );
//
//    GUI_DispStringAt("           ", 20, 170);
//    GUI_DispDecAt(acceleration.y, 20, 165, 4 );
//
//    GUI_DispStringAt("           ", 20, 210);
//    GUI_DispDecAt(acceleration.z, 20, 200, 4 );

    snprintf(dataOut, MAX_BUF_SIZE, "X: % 4ld  ", acceleration.x);
    GUI_DispStringAt(dataOut,30,150);

    snprintf(dataOut, MAX_BUF_SIZE, "Y: % 4ld  ", acceleration.y);
    GUI_DispStringAt(dataOut,30,180);

    snprintf(dataOut, MAX_BUF_SIZE, "Z: % 4ld  ", acceleration.z);
    GUI_DispStringAt(dataOut,30,210);

//    snprintf(dataOut, MAX_BUF_SIZE, "X: %- 04d    Y: %- 04d    Z: %- 04d  ", acceleration.x, acceleration.y, acceleration.z);
//    GUI_DispStringAt(dataOut,10,200);


  }
}

/**
  * @brief  Handles the gyroscope axes data getting/sending
  * @param  Instance the device instance
  * @retval None
  */
static void Gyro_Sensor_Handler(uint32_t Instance)
{
  float odr;
  int32_t fullScale;
  CUSTOM_MOTION_SENSOR_Axes_t angular_velocity;
  displayFloatToInt_t out_value;
  uint8_t whoami;

  if (CUSTOM_MOTION_SENSOR_GetAxes(Instance, MOTION_GYRO, &angular_velocity))
  {
    snprintf(dataOut, MAX_BUF_SIZE, "\r\nGYR[%d]: Error\r\n", (int)Instance);
  }
  else
  {
    snprintf(dataOut, MAX_BUF_SIZE, "\r\nGYR_X[%d]: %d, GYR_Y[%d]: %d, GYR_Z[%d]: %d\r\n", (int)Instance,
             (int)angular_velocity.x, (int)Instance, (int)angular_velocity.y, (int)Instance, (int)angular_velocity.z);
  }

  printf("%s", dataOut);

  if (verbose == 1)
  {
    if (CUSTOM_MOTION_SENSOR_ReadID(Instance, &whoami))
    {
      snprintf(dataOut, MAX_BUF_SIZE, "WHOAMI[%d]: Error\r\n", (int)Instance);
    }
    else
    {
      snprintf(dataOut, MAX_BUF_SIZE, "WHOAMI[%d]: 0x%x\r\n", (int)Instance, (int)whoami);
    }

    printf("%s", dataOut);

    if (CUSTOM_MOTION_SENSOR_GetOutputDataRate(Instance, MOTION_GYRO, &odr))
    {
      snprintf(dataOut, MAX_BUF_SIZE, "ODR[%d]: ERROR\r\n", (int)Instance);
    }
    else
    {
      floatToInt(odr, &out_value, 3);
      snprintf(dataOut, MAX_BUF_SIZE, "ODR[%d]: %d.%03d Hz\r\n", (int)Instance, (int)out_value.out_int,
               (int)out_value.out_dec);
    }

    printf("%s", dataOut);

    if (CUSTOM_MOTION_SENSOR_GetFullScale(Instance, MOTION_GYRO, &fullScale))
    {
      snprintf(dataOut, MAX_BUF_SIZE, "FS[%d]: ERROR\r\n", (int)Instance);
    }
    else
    {
      snprintf(dataOut, MAX_BUF_SIZE, "FS[%d]: %d dps\r\n", (int)Instance, (int)fullScale);
    }

    printf("%s", dataOut);
  }
}

/**
  * @brief  Handles the magneto axes data getting/sending
  * @param  Instance the device instance
  * @retval None
  */
static void Magneto_Sensor_Handler(uint32_t Instance)
{
  float odr;
  int32_t fullScale;
  CUSTOM_MOTION_SENSOR_Axes_t magnetic_field;
  displayFloatToInt_t out_value;
  uint8_t whoami;

  if (CUSTOM_MOTION_SENSOR_GetAxes(Instance, MOTION_MAGNETO, &magnetic_field))
  {
    snprintf(dataOut, MAX_BUF_SIZE, "\r\nMAG[%d]: Error\r\n", (int)Instance);
  }
  else
  {
    snprintf(dataOut, MAX_BUF_SIZE, "\r\nMAG_X[%d]: %d, MAG_Y[%d]: %d, MAG_Z[%d]: %d\r\n", (int)Instance,
             (int)magnetic_field.x, (int)Instance, (int)magnetic_field.y, (int)Instance, (int)magnetic_field.z);
  }

  printf("%s", dataOut);

  if (verbose == 1)
  {
    if (CUSTOM_MOTION_SENSOR_ReadID(Instance, &whoami))
    {
      snprintf(dataOut, MAX_BUF_SIZE, "WHOAMI[%d]: Error\r\n", (int)Instance);
    }
    else
    {
      snprintf(dataOut, MAX_BUF_SIZE, "WHOAMI[%d]: 0x%x\r\n", (int)Instance, (int)whoami);
    }

    printf("%s", dataOut);

    if (CUSTOM_MOTION_SENSOR_GetOutputDataRate(Instance, MOTION_MAGNETO, &odr))
    {
      snprintf(dataOut, MAX_BUF_SIZE, "ODR[%d]: ERROR\r\n", (int)Instance);
    }
    else
    {
      floatToInt(odr, &out_value, 3);
      snprintf(dataOut, MAX_BUF_SIZE, "ODR[%d]: %d.%03d Hz\r\n", (int)Instance, (int)out_value.out_int,
               (int)out_value.out_dec);
    }

    printf("%s", dataOut);

    if (CUSTOM_MOTION_SENSOR_GetFullScale(Instance, MOTION_MAGNETO, &fullScale))
    {
      snprintf(dataOut, MAX_BUF_SIZE, "FS[%d]: ERROR\r\n", (int)Instance);
    }
    else
    {
      snprintf(dataOut, MAX_BUF_SIZE, "FS[%d]: %d gauss\r\n", (int)Instance, (int)fullScale);
    }

    printf("%s", dataOut);
  }
}

#ifdef __cplusplus
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
