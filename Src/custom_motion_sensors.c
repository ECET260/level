/**
 ******************************************************************************
 * @file    custom_motion_sensors.c
 * @author  MEMS Application Team
 * @version V6.0.0
 * @date    16-January-2019
 * @brief   This file provides BSP Motion Sensors interface for custom boards
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

/* Includes ------------------------------------------------------------------*/
#include "custom_motion_sensors.h"

extern void *MotionCompObj[CUSTOM_MOTION_INSTANCES_NBR]; /* This "redundant" line is here to fulfil MISRA C-2012 rule 8.4 */
void *MotionCompObj[CUSTOM_MOTION_INSTANCES_NBR];

/* We define a jump table in order to get the correct index from the desired function. */
/* This table should have a size equal to the maximum value of a function plus 1.      */
static uint32_t FunctionIndex[5] = {0,0,1,1,2};
static MOTION_SENSOR_FuncDrv_t *MotionFuncDrv[CUSTOM_MOTION_INSTANCES_NBR][CUSTOM_MOTION_FUNCTIONS_NBR];
static MOTION_SENSOR_CommonDrv_t *MotionDrv[CUSTOM_MOTION_INSTANCES_NBR];
static CUSTOM_MOTION_SENSOR_Ctx_t MotionCtx[CUSTOM_MOTION_INSTANCES_NBR];

#if (USE_CUSTOM_MOTION_SENSOR_LSM6DSL_0 == 1)
static int32_t LSM6DSL_0_Probe(uint32_t Functions);
#endif

/**
  * @brief  Initializes the motion sensors
  * @param  Instance Motion sensor instance
  * @param  Functions Motion sensor functions. Could be :
  *         - MOTION_GYRO
  *         - MOTION_ACCELERO
  *         - MOTION_MAGNETO
  * @retval BSP status
  */
int32_t CUSTOM_MOTION_SENSOR_Init(uint32_t Instance, uint32_t Functions)
{
  int32_t ret = BSP_ERROR_NONE;
  uint32_t function = MOTION_GYRO;
  uint32_t i;
  uint32_t component_functions = 0;
  CUSTOM_MOTION_SENSOR_Capabilities_t cap;

  switch (Instance)
  {
#if (USE_CUSTOM_MOTION_SENSOR_LSM6DSL_0 == 1)
    case CUSTOM_LSM6DSL_0:
      if (LSM6DSL_0_Probe(Functions) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_NO_INIT;
      }
      if (MotionDrv[Instance]->GetCapabilities(MotionCompObj[Instance], (void *)&cap) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_UNKNOWN_COMPONENT;
      }
      if (cap.Acc == 1U)
      {
        component_functions |= MOTION_ACCELERO;
      }
      if (cap.Gyro == 1U)
      {
        component_functions |= MOTION_GYRO;
      }
      if (cap.Magneto == 1U)
      {
        component_functions |= MOTION_MAGNETO;
      }
      break;
#endif
    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  if (ret != BSP_ERROR_NONE)
  {
    return ret;
  }

  for (i = 0; i < CUSTOM_MOTION_FUNCTIONS_NBR; i++)
  {
    if (((Functions & function) == function) && ((component_functions & function) == function))
    {
      if (MotionFuncDrv[Instance][FunctionIndex[function]]->Enable(MotionCompObj[Instance]) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_COMPONENT_FAILURE;
      }
    }
    function = function << 1;
  }

  return ret;
}

/**
 * @brief  Deinitialize Motion sensor
 * @param  Instance Motion sensor instance
 * @retval BSP status
 */
int32_t CUSTOM_MOTION_SENSOR_DeInit(uint32_t Instance)
{
  int32_t ret;

  if (Instance >= CUSTOM_MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if (MotionDrv[Instance]->DeInit(MotionCompObj[Instance]) != BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
 * @brief  Get motion sensor instance capabilities
 * @param  Instance Motion sensor instance
 * @param  Capabilities pointer to motion sensor capabilities
 * @retval BSP status
 */
int32_t CUSTOM_MOTION_SENSOR_GetCapabilities(uint32_t Instance, CUSTOM_MOTION_SENSOR_Capabilities_t *Capabilities)
{
  int32_t ret;

  if (Instance >= CUSTOM_MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if (MotionDrv[Instance]->GetCapabilities(MotionCompObj[Instance], Capabilities) != BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
 * @brief  Get WHOAMI value
 * @param  Instance Motion sensor instance
 * @param  Id WHOAMI value
 * @retval BSP status
 */
int32_t CUSTOM_MOTION_SENSOR_ReadID(uint32_t Instance, uint8_t *Id)
{
  int32_t ret;

  if (Instance >= CUSTOM_MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if (MotionDrv[Instance]->ReadID(MotionCompObj[Instance], Id) != BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
 * @brief  Enable Motion sensor
 * @param  Instance Motion sensor instance
 * @param  Function Motion sensor function. Could be :
 *         - MOTION_GYRO
 *         - MOTION_ACCELERO
 *         - MOTION_MAGNETO
 * @retval BSP status
 */
int32_t CUSTOM_MOTION_SENSOR_Enable(uint32_t Instance, uint32_t Function)
{
  int32_t ret;

  if (Instance >= CUSTOM_MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((MotionCtx[Instance].Functions & Function) == Function)
    {
      if (MotionFuncDrv[Instance][FunctionIndex[Function]]->Enable(MotionCompObj[Instance]) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }

  return ret;
}

/**
 * @brief  Disable Motion sensor
 * @param  Instance Motion sensor instance
 * @param  Function Motion sensor function. Could be :
 *         - MOTION_GYRO
 *         - MOTION_ACCELERO
 *         - MOTION_MAGNETO
 * @retval BSP status
 */
int32_t CUSTOM_MOTION_SENSOR_Disable(uint32_t Instance, uint32_t Function)
{
  int32_t ret;

  if (Instance >= CUSTOM_MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((MotionCtx[Instance].Functions & Function) == Function)
    {
      if (MotionFuncDrv[Instance][FunctionIndex[Function]]->Disable(MotionCompObj[Instance]) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }

  return ret;
}

/**
 * @brief  Get motion sensor axes data
 * @param  Instance Motion sensor instance
 * @param  Function Motion sensor function. Could be :
 *         - MOTION_GYRO
 *         - MOTION_ACCELERO
 *         - MOTION_MAGNETO
 * @param  Axes pointer to axes data structure
 * @retval BSP status
 */
int32_t CUSTOM_MOTION_SENSOR_GetAxes(uint32_t Instance, uint32_t Function, CUSTOM_MOTION_SENSOR_Axes_t *Axes)
{
  int32_t ret;

  if (Instance >= CUSTOM_MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((MotionCtx[Instance].Functions & Function) == Function)
    {
      if (MotionFuncDrv[Instance][FunctionIndex[Function]]->GetAxes(MotionCompObj[Instance], Axes) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }

  return ret;
}

/**
 * @brief  Get motion sensor axes raw data
 * @param  Instance Motion sensor instance
 * @param  Function Motion sensor function. Could be :
 *         - MOTION_GYRO
 *         - MOTION_ACCELERO
 *         - MOTION_MAGNETO
 * @param  Axes pointer to axes raw data structure
 * @retval BSP status
 */
int32_t CUSTOM_MOTION_SENSOR_GetAxesRaw(uint32_t Instance, uint32_t Function, CUSTOM_MOTION_SENSOR_AxesRaw_t *Axes)
{
  int32_t ret;

  if (Instance >= CUSTOM_MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((MotionCtx[Instance].Functions & Function) == Function)
    {
      if (MotionFuncDrv[Instance][FunctionIndex[Function]]->GetAxesRaw(MotionCompObj[Instance], Axes) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }

  return ret;
}

/**
 * @brief  Get motion sensor sensitivity
 * @param  Instance Motion sensor instance
 * @param  Function Motion sensor function. Could be :
 *         - MOTION_GYRO
 *         - MOTION_ACCELERO
 *         - MOTION_MAGNETO
 * @param  Sensitivity pointer to sensitivity read value
 * @retval BSP status
 */
int32_t CUSTOM_MOTION_SENSOR_GetSensitivity(uint32_t Instance, uint32_t Function, float *Sensitivity)
{
  int32_t ret;

  if (Instance >= CUSTOM_MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((MotionCtx[Instance].Functions & Function) == Function)
    {
      if (MotionFuncDrv[Instance][FunctionIndex[Function]]->GetSensitivity(MotionCompObj[Instance],
          Sensitivity) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }

  return ret;
}

/**
 * @brief  Get motion sensor Output Data Rate
 * @param  Instance Motion sensor instance
 * @param  Function Motion sensor function. Could be :
 *         - MOTION_GYRO
 *         - MOTION_ACCELERO
 *         - MOTION_MAGNETO
 * @param  Odr pointer to Output Data Rate read value
 * @retval BSP status
 */
int32_t CUSTOM_MOTION_SENSOR_GetOutputDataRate(uint32_t Instance, uint32_t Function, float *Odr)
{
  int32_t ret;

  if (Instance >= CUSTOM_MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((MotionCtx[Instance].Functions & Function) == Function)
    {
      if (MotionFuncDrv[Instance][FunctionIndex[Function]]->GetOutputDataRate(MotionCompObj[Instance], Odr) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }

  return ret;
}

/**
 * @brief  Get motion sensor Full Scale
 * @param  Instance Motion sensor instance
 * @param  Function Motion sensor function. Could be :
 *         - MOTION_GYRO
 *         - MOTION_ACCELERO
 *         - MOTION_MAGNETO
 * @param  Fullscale pointer to Fullscale read value
 * @retval BSP status
 */
int32_t CUSTOM_MOTION_SENSOR_GetFullScale(uint32_t Instance, uint32_t Function, int32_t *Fullscale)
{
  int32_t ret;

  if (Instance >= CUSTOM_MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((MotionCtx[Instance].Functions & Function) == Function)
    {
      if (MotionFuncDrv[Instance][FunctionIndex[Function]]->GetFullScale(MotionCompObj[Instance], Fullscale) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }

  return ret;
}

/**
 * @brief  Set motion sensor Output Data Rate
 * @param  Instance Motion sensor instance
 * @param  Function Motion sensor function. Could be :
 *         - MOTION_GYRO
 *         - MOTION_ACCELERO
 *         - MOTION_MAGNETO
 * @param  Odr Output Data Rate value to be set
 * @retval BSP status
 */
int32_t CUSTOM_MOTION_SENSOR_SetOutputDataRate(uint32_t Instance, uint32_t Function, float Odr)
{
  int32_t ret;

  if (Instance >= CUSTOM_MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((MotionCtx[Instance].Functions & Function) == Function)
    {
      if (MotionFuncDrv[Instance][FunctionIndex[Function]]->SetOutputDataRate(MotionCompObj[Instance], Odr) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }

  return ret;
}

/**
 * @brief  Set motion sensor Full Scale
 * @param  Instance Motion sensor instance
 * @param  Function Motion sensor function. Could be :
 *         - MOTION_GYRO
 *         - MOTION_ACCELERO
 *         - MOTION_MAGNETO
 * @param  Fullscale Fullscale value to be set
 * @retval BSP status
 */
int32_t CUSTOM_MOTION_SENSOR_SetFullScale(uint32_t Instance, uint32_t Function, int32_t Fullscale)
{
  int32_t ret;

  if (Instance >= CUSTOM_MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((MotionCtx[Instance].Functions & Function) == Function)
    {
      if (MotionFuncDrv[Instance][FunctionIndex[Function]]->SetFullScale(MotionCompObj[Instance], Fullscale) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }

  return ret;
}

#if (USE_CUSTOM_MOTION_SENSOR_LSM6DSL_0  == 1)
/**
 * @brief  Register Bus IOs for LSM6DSL instance
 * @param  Functions Motion sensor functions. Could be :
 *         - MOTION_GYRO and/or MOTION_ACCELERO
 * @retval BSP status
 */
static int32_t LSM6DSL_0_Probe(uint32_t Functions)
{
  LSM6DSL_IO_t            io_ctx;
  uint8_t                 id;
  static LSM6DSL_Object_t lsm6dsl_obj_0;
  LSM6DSL_Capabilities_t  cap;
  int32_t                 ret = BSP_ERROR_NONE;

  /* Configure the accelero driver */
  io_ctx.BusType     = LSM6DSL_I2C_BUS; /* I2C */
  io_ctx.Address     = LSM6DSL_I2C_ADD_L; /* SA0 = GND */
  io_ctx.Init        = CUSTOM_LSM6DSL_0_I2C_Init;
  io_ctx.DeInit      = CUSTOM_LSM6DSL_0_I2C_DeInit;
  io_ctx.ReadReg     = CUSTOM_LSM6DSL_0_I2C_ReadReg;
  io_ctx.WriteReg    = CUSTOM_LSM6DSL_0_I2C_WriteReg;
  io_ctx.GetTick     = BSP_GetTick;

  if (LSM6DSL_RegisterBusIO(&lsm6dsl_obj_0, &io_ctx) != LSM6DSL_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if (LSM6DSL_ReadID(&lsm6dsl_obj_0, &id) != LSM6DSL_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if (id != LSM6DSL_ID)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else
  {
    (void)LSM6DSL_GetCapabilities(&lsm6dsl_obj_0, &cap);
    MotionCtx[CUSTOM_LSM6DSL_0].Functions = ((uint32_t)cap.Gyro) | ((uint32_t)cap.Acc << 1) | ((uint32_t)cap.Magneto << 2);

    MotionCompObj[CUSTOM_LSM6DSL_0] = &lsm6dsl_obj_0;
    /* The second cast (void *) is added to bypass Misra R11.3 rule */
    MotionDrv[CUSTOM_LSM6DSL_0] = (MOTION_SENSOR_CommonDrv_t *)(void *)&LSM6DSL_COMMON_Driver;

    if ((ret == BSP_ERROR_NONE) && ((Functions & MOTION_GYRO) == MOTION_GYRO) && (cap.Gyro == 1U))
    {
      /* The second cast (void *) is added to bypass Misra R11.3 rule */
      MotionFuncDrv[CUSTOM_LSM6DSL_0][FunctionIndex[MOTION_GYRO]] = (MOTION_SENSOR_FuncDrv_t *)(void *)&LSM6DSL_GYRO_Driver;

      if (MotionDrv[CUSTOM_LSM6DSL_0]->Init(MotionCompObj[CUSTOM_LSM6DSL_0]) != LSM6DSL_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    if ((ret == BSP_ERROR_NONE) && ((Functions & MOTION_ACCELERO) == MOTION_ACCELERO) && (cap.Acc == 1U))
    {
      /* The second cast (void *) is added to bypass Misra R11.3 rule */
      MotionFuncDrv[CUSTOM_LSM6DSL_0][FunctionIndex[MOTION_ACCELERO]] = (MOTION_SENSOR_FuncDrv_t *)(void *)&LSM6DSL_ACC_Driver;

      if (MotionDrv[CUSTOM_LSM6DSL_0]->Init(MotionCompObj[CUSTOM_LSM6DSL_0]) != LSM6DSL_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    if ((ret == BSP_ERROR_NONE) && ((Functions & MOTION_MAGNETO) == MOTION_MAGNETO))
    {
      /* Return an error if the application try to initialize a function not supported by the component */
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return ret;
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
