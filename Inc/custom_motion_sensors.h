/**
  ******************************************************************************
  * @file    custom_motion_sensors.h
  * @author  MEMS Application Team
  * @version V6.0.0
  * @date    16-January-2019
  * @brief   This file contains definitions for the BSP Motion Sensors interface for custom boards
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2019 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CUSTOM_MOTION_SENSORS_H__
#define __CUSTOM_MOTION_SENSORS_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include "custom_mems_conf.h"
#include "motion_sensor.h"

#ifndef USE_CUSTOM_MOTION_SENSOR_LSM6DSL_0
#define USE_CUSTOM_MOTION_SENSOR_LSM6DSL_0          1
#endif

#if (USE_CUSTOM_MOTION_SENSOR_LSM6DSL_0 == 1)
#include "lsm6dsl.h"
#endif

#if (USE_CUSTOM_MOTION_SENSOR_LSM6DSL_0 == 1)
#define CUSTOM_LSM6DSL_0 (0)
#endif

typedef struct
{
  int32_t x;
  int32_t y;
  int32_t z;
} CUSTOM_MOTION_SENSOR_Axes_t;

typedef struct
{
  int16_t x;
  int16_t y;
  int16_t z;
} CUSTOM_MOTION_SENSOR_AxesRaw_t;

/* Motion Sensor instance Info */
typedef struct
{
  uint8_t  Acc;
  uint8_t  Gyro;
  uint8_t  Magneto;
  uint8_t  LowPower;
  uint32_t GyroMaxFS;
  uint32_t AccMaxFS;
  uint32_t MagMaxFS;
  float    GyroMaxOdr;
  float    AccMaxOdr;
  float    MagMaxOdr;
} CUSTOM_MOTION_SENSOR_Capabilities_t;

typedef struct
{
  uint32_t Functions;
} CUSTOM_MOTION_SENSOR_Ctx_t;

#ifndef MOTION_GYRO
#define MOTION_GYRO             1U
#endif
#ifndef MOTION_ACCELERO
#define MOTION_ACCELERO         2U
#endif
#ifndef MOTION_MAGNETO
#define MOTION_MAGNETO          4U
#endif

#define CUSTOM_MOTION_FUNCTIONS_NBR    3U
#define CUSTOM_MOTION_INSTANCES_NBR    (USE_CUSTOM_MOTION_SENSOR_LSM6DSL_0)

#if (CUSTOM_MOTION_INSTANCES_NBR == 0)
#error "No motion sensor instance has been selected"
#endif

int32_t CUSTOM_MOTION_SENSOR_Init(uint32_t Instance, uint32_t Functions);
int32_t CUSTOM_MOTION_SENSOR_DeInit(uint32_t Instance);
int32_t CUSTOM_MOTION_SENSOR_GetCapabilities(uint32_t Instance, CUSTOM_MOTION_SENSOR_Capabilities_t *Capabilities);
int32_t CUSTOM_MOTION_SENSOR_ReadID(uint32_t Instance, uint8_t *Id);
int32_t CUSTOM_MOTION_SENSOR_Enable(uint32_t Instance, uint32_t Function);
int32_t CUSTOM_MOTION_SENSOR_Disable(uint32_t Instance, uint32_t Function);
int32_t CUSTOM_MOTION_SENSOR_GetAxes(uint32_t Instance, uint32_t Function, CUSTOM_MOTION_SENSOR_Axes_t *Axes);
int32_t CUSTOM_MOTION_SENSOR_GetAxesRaw(uint32_t Instance, uint32_t Function, CUSTOM_MOTION_SENSOR_AxesRaw_t *Axes);
int32_t CUSTOM_MOTION_SENSOR_GetSensitivity(uint32_t Instance, uint32_t Function, float *Sensitivity);
int32_t CUSTOM_MOTION_SENSOR_GetOutputDataRate(uint32_t Instance, uint32_t Function, float *Odr);
int32_t CUSTOM_MOTION_SENSOR_SetOutputDataRate(uint32_t Instance, uint32_t Function, float Odr);
int32_t CUSTOM_MOTION_SENSOR_GetFullScale(uint32_t Instance, uint32_t Function, int32_t *Fullscale);
int32_t CUSTOM_MOTION_SENSOR_SetFullScale(uint32_t Instance, uint32_t Function, int32_t Fullscale);

#ifdef __cplusplus
}
#endif

#endif /* __CUSTOM_MOTION_SENSORS_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
