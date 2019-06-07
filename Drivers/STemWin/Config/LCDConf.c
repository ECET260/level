/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2017  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.44 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The  software has  been licensed  to STMicroelectronics International
N.V. a Dutch company with a Swiss branch and its headquarters in Plan-
les-Ouates, Geneva, 39 Chemin du Champ des Filles, Switzerland for the
purposes of creating libraries for ARM Cortex-M-based 32-bit microcon_
troller products commercialized by Licensee only, sublicensed and dis_
tributed under the terms and conditions of the End User License Agree_
ment supplied by STMicroelectronics International N.V.
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : LCDConf_FlexColor_Template.c
Purpose     : Display controller configuration (single layer)
---------------------------END-OF-HEADER------------------------------
*/

/**
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics. 
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license SLA0044,
  * the "License"; You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *                      http://www.st.com/SLA0044
  *
  ******************************************************************************
  */

#include "GUI.h"
#include "GUIDRV_FlexColor.h"
#include "main.h"
#include "LCDConf.h"
#include "GUITDRV_ADS7846.h"

GUITDRV_ADS7846_CONFIG pConfig;

/*********************************************************************
*
*       Layer configuration (to be modified)
*
**********************************************************************
*/

//
// Physical display size
//
#define XSIZE_PHYS  240 // To be adapted to x-screen size
#define YSIZE_PHYS  320 // To be adapted to y-screen size

/* Color conversion */
#define COLOR_CONVERSION    GUICC_565

/* Display driver */
//#define DISPLAY_DRIVER GUIDRV_COMPACT_COLOR_16 //EmWin
#define DISPLAY_DRIVER GUIDRV_FLEXCOLOR			 //STemWin

// Buffers / VScreens
//
#define NUM_BUFFERS   1
#define NUM_VSCREENS  1

//
// Display orientation
//

#define DISPLAY_ORIENTATION  GUI_SWAP_XY | GUI_MIRROR_Y

//
// Touch screen
//
#define USE_TOUCH   1
//
// 	Touch screen calibration
//	calibrate screen

#define TOUCH_X_MIN 450
#define TOUCH_X_MAX 3520
#define TOUCH_Y_MIN 580
#define TOUCH_Y_MAX 3515


/*********************************************************************
*
*       Configuration checking
*
**********************************************************************
*/
#ifndef   VXSIZE_PHYS
  #define VXSIZE_PHYS XSIZE_PHYS
#endif
#ifndef   VYSIZE_PHYS
  #define VYSIZE_PHYS YSIZE_PHYS
#endif



#ifndef   XSIZE_PHYS
  #error Physical X size of display is not defined!
#endif
#ifndef   YSIZE_PHYS
  #error Physical Y size of display is not defined!
#endif
#ifndef   GUICC_565
  #error Color conversion not defined!
#endif
#ifndef   GUIDRV_FLEXCOLOR
  #error No display driver defined!
#endif

#ifndef   NUM_VSCREENS
  #define NUM_VSCREENS 1
#else
  #if (NUM_VSCREENS <= 0)
    #error At least one screeen needs to be defined!
  #endif
#endif
#if (NUM_VSCREENS > 1) && (NUM_BUFFERS > 1)
  #error Virtual screens and multiple buffers are not allowed!
#endif

#ifndef   LCD_SWAP_XY
  #define LCD_SWAP_XY 0
#endif
#if LCD_SWAP_XY
  #define LCD_XSIZE YSIZE_PHYS
  #define LCD_YSIZE XSIZE_PHYS
#else
  #define LCD_XSIZE XSIZE_PHYS
  #define LCD_YSIZE YSIZE_PHYS
#endif
#define LCD_VXSIZE LCD_XSIZE
#define LCD_VYSIZE LCD_YSIZE


#ifndef   DISPLAY_ORIENTATION
  #define DISPLAY_ORIENTATION  0
#endif

#if ((DISPLAY_ORIENTATION & GUI_SWAP_XY) != 0)
#define LANDSCAPE   1
#else
#define LANDSCAPE   0
#endif

#if (LANDSCAPE == 1)
#define WIDTH       YSIZE_PHYS  /* Screen Width (in pixels)         */
#define HEIGHT      XSIZE_PHYS  /* Screen Hight (in pixels)         */
#else
#define WIDTH       XSIZE_PHYS  /* Screen Width (in pixels)         */
#define HEIGHT      YSIZE_PHYS  /* Screen Hight (in pixels)         */
#endif

#if ((DISPLAY_ORIENTATION & GUI_SWAP_XY) != 0)
  #if ((DISPLAY_ORIENTATION & GUI_MIRROR_X) != 0)
    #define TOUCH_TOP    TOUCH_X_MAX
    #define TOUCH_BOTTOM TOUCH_X_MIN
  #else
    #define TOUCH_TOP    TOUCH_X_MIN
    #define TOUCH_BOTTOM TOUCH_X_MAX
  #endif
  #if ((DISPLAY_ORIENTATION & GUI_MIRROR_Y) != 0)
    #define TOUCH_LEFT   TOUCH_Y_MAX
    #define TOUCH_RIGHT  TOUCH_Y_MIN
  #else
    #define TOUCH_LEFT   TOUCH_Y_MIN
    #define TOUCH_RIGHT  TOUCH_Y_MAX
  #endif
#else
  #if ((DISPLAY_ORIENTATION & GUI_MIRROR_X) != 0)
    #define TOUCH_LEFT   TOUCH_X_MAX
    #define TOUCH_RIGHT  TOUCH_X_MIN
  #else
    #define TOUCH_LEFT   TOUCH_X_MIN
    #define TOUCH_RIGHT  TOUCH_X_MAX
  #endif
  #if ((DISPLAY_ORIENTATION & GUI_MIRROR_Y) != 0)
    #define TOUCH_TOP    TOUCH_Y_MAX
    #define TOUCH_BOTTOM TOUCH_Y_MIN
  #else
    #define TOUCH_TOP    TOUCH_Y_MIN
    #define TOUCH_BOTTOM TOUCH_Y_MAX
  #endif
#endif

/*********************************************************************
*
*       Local functions
*
**********************************************************************
*/
/********************************************************************
*
*       LcdWriteReg
*
* Function description:
*   Sets display register
*/
void LcdWriteReg(U8 Command) {
  // ... TBD by user
	HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_RESET);	//command mode

	HAL_GPIO_WritePin(LCD_D7_GPIO_Port, LCD_D7_Pin, (Command & 0b10000000) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_D6_GPIO_Port, LCD_D6_Pin, (Command & 0b01000000) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_D5_GPIO_Port, LCD_D5_Pin, (Command & 0b00100000) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_D4_GPIO_Port, LCD_D4_Pin, (Command & 0b00010000) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_D3_GPIO_Port, LCD_D3_Pin, (Command & 0b00001000) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_D2_GPIO_Port, LCD_D2_Pin, (Command & 0b00000100) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_D1_GPIO_Port, LCD_D1_Pin, (Command & 0b00000010) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_D0_GPIO_Port, LCD_D0_Pin, (Command & 0b00000001) ? GPIO_PIN_SET : GPIO_PIN_RESET);

	HAL_GPIO_WritePin(LCD_WR_GPIO_Port, LCD_WR_Pin, GPIO_PIN_RESET);
//	HAL_Delay(1);
	HAL_GPIO_WritePin(LCD_WR_GPIO_Port, LCD_WR_Pin, GPIO_PIN_SET);
}

/********************************************************************
*
*       LcdWriteData
*
* Function description:
*   Writes a value to a display register
*/
void LcdWriteData(U8 Data) {
  // ... TBD by user
	HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_SET);	//data mode

	HAL_GPIO_WritePin(LCD_D7_GPIO_Port, LCD_D7_Pin, ((Data & 0b10000000) ? GPIO_PIN_SET : GPIO_PIN_RESET));
	HAL_GPIO_WritePin(LCD_D6_GPIO_Port, LCD_D6_Pin, ((Data & 0b01000000) ? GPIO_PIN_SET : GPIO_PIN_RESET));
	HAL_GPIO_WritePin(LCD_D5_GPIO_Port, LCD_D5_Pin, ((Data & 0b00100000) ? GPIO_PIN_SET : GPIO_PIN_RESET));
	HAL_GPIO_WritePin(LCD_D4_GPIO_Port, LCD_D4_Pin, ((Data & 0b00010000) ? GPIO_PIN_SET : GPIO_PIN_RESET));
	HAL_GPIO_WritePin(LCD_D3_GPIO_Port, LCD_D3_Pin, ((Data & 0b00001000) ? GPIO_PIN_SET : GPIO_PIN_RESET));
	HAL_GPIO_WritePin(LCD_D2_GPIO_Port, LCD_D2_Pin, ((Data & 0b00000100) ? GPIO_PIN_SET : GPIO_PIN_RESET));
	HAL_GPIO_WritePin(LCD_D1_GPIO_Port, LCD_D1_Pin, ((Data & 0b00000010) ? GPIO_PIN_SET : GPIO_PIN_RESET));
	HAL_GPIO_WritePin(LCD_D0_GPIO_Port, LCD_D0_Pin, ((Data & 0b00000001) ? GPIO_PIN_SET : GPIO_PIN_RESET));

	HAL_GPIO_WritePin(LCD_WR_GPIO_Port, LCD_WR_Pin, GPIO_PIN_RESET);
//	HAL_Delay(1);
	HAL_GPIO_WritePin(LCD_WR_GPIO_Port, LCD_WR_Pin, GPIO_PIN_SET);


}

/********************************************************************
*
*       LcdWriteDataMultiple
*
* Function description:
*   Writes multiple values to a display register.
*/
void LcdWriteDataMultiple(U8 * pData, int NumItems) {
  while (NumItems--) {
    // ... TBD by user

		HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_SET);	//data mode

		HAL_GPIO_WritePin(LCD_D7_GPIO_Port, LCD_D7_Pin, (*pData & 0b10000000) ? GPIO_PIN_SET : GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LCD_D6_GPIO_Port, LCD_D6_Pin, (*pData & 0b01000000) ? GPIO_PIN_SET : GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LCD_D5_GPIO_Port, LCD_D5_Pin, (*pData & 0b00100000) ? GPIO_PIN_SET : GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LCD_D4_GPIO_Port, LCD_D4_Pin, (*pData & 0b00010000) ? GPIO_PIN_SET : GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LCD_D3_GPIO_Port, LCD_D3_Pin, (*pData & 0b00001000) ? GPIO_PIN_SET : GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LCD_D2_GPIO_Port, LCD_D2_Pin, (*pData & 0b00000100) ? GPIO_PIN_SET : GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LCD_D1_GPIO_Port, LCD_D1_Pin, (*pData & 0b00000010) ? GPIO_PIN_SET : GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LCD_D0_GPIO_Port, LCD_D0_Pin, (*pData & 0b00000001) ? GPIO_PIN_SET : GPIO_PIN_RESET);

		HAL_GPIO_WritePin(LCD_WR_GPIO_Port, LCD_WR_Pin, GPIO_PIN_RESET);
//		HAL_Delay(1);
		HAL_GPIO_WritePin(LCD_WR_GPIO_Port, LCD_WR_Pin, GPIO_PIN_SET);

		pData++;

  }
}

/********************************************************************
*
*       LcdReadDataMultiple
*
* Function description:
*   Reads multiple values from a display register.
*/
void LcdReadDataMultiple(U8 * pData, int NumItems) {

	  GPIO_InitTypeDef GPIO_InitStruct = {0};

	  HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_SET);	//data mode

	  //Configure GPIO pins input
	  GPIO_InitStruct.Pin = LCD_D7_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(LCD_D7_GPIO_Port, &GPIO_InitStruct);

	  GPIO_InitStruct.Pin = LCD_D6_Pin;
	  HAL_GPIO_Init(LCD_D6_GPIO_Port, &GPIO_InitStruct);

	  GPIO_InitStruct.Pin = LCD_D5_Pin;
	  HAL_GPIO_Init(LCD_D5_GPIO_Port, &GPIO_InitStruct);

	  GPIO_InitStruct.Pin = LCD_D4_Pin;
	  HAL_GPIO_Init(LCD_D4_GPIO_Port, &GPIO_InitStruct);

	  GPIO_InitStruct.Pin = LCD_D3_Pin;
	  HAL_GPIO_Init(LCD_D3_GPIO_Port, &GPIO_InitStruct);

	  GPIO_InitStruct.Pin = LCD_D2_Pin;
	  HAL_GPIO_Init(LCD_D2_GPIO_Port, &GPIO_InitStruct);

	  GPIO_InitStruct.Pin = LCD_D1_Pin;
	  HAL_GPIO_Init(LCD_D1_GPIO_Port, &GPIO_InitStruct);

	  GPIO_InitStruct.Pin = LCD_D0_Pin;
	  HAL_GPIO_Init(LCD_D0_GPIO_Port, &GPIO_InitStruct);

  while (NumItems--) {
    // ... TBD by user
	  uint8_t LCD_Data = 0;

	  HAL_GPIO_WritePin(LCD_RD_GPIO_Port, LCD_RD_Pin, GPIO_PIN_RESET); 			//enable RD

	  LCD_Data |= HAL_GPIO_ReadPin(LCD_D7_GPIO_Port, LCD_D7_Pin) << 7;
	  LCD_Data |= HAL_GPIO_ReadPin(LCD_D6_GPIO_Port, LCD_D6_Pin) << 6;
	  LCD_Data |= HAL_GPIO_ReadPin(LCD_D5_GPIO_Port, LCD_D5_Pin) << 5;
	  LCD_Data |= HAL_GPIO_ReadPin(LCD_D4_GPIO_Port, LCD_D4_Pin) << 4;
	  LCD_Data |= HAL_GPIO_ReadPin(LCD_D3_GPIO_Port, LCD_D3_Pin) << 3;
	  LCD_Data |= HAL_GPIO_ReadPin(LCD_D2_GPIO_Port, LCD_D2_Pin) << 2;
	  LCD_Data |= HAL_GPIO_ReadPin(LCD_D1_GPIO_Port, LCD_D1_Pin) << 1;
	  LCD_Data |= HAL_GPIO_ReadPin(LCD_D0_GPIO_Port, LCD_D0_Pin);

	  HAL_GPIO_WritePin(LCD_RD_GPIO_Port, LCD_RD_Pin, GPIO_PIN_SET); 			//disable RD

	  *pData++ = LCD_Data;


  }

  //Configure GPIO pins back to output
  GPIO_InitStruct.Pin = LCD_D7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LCD_D7_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LCD_D6_Pin;
  HAL_GPIO_Init(LCD_D6_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LCD_D5_Pin;
  HAL_GPIO_Init(LCD_D5_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LCD_D4_Pin;
  HAL_GPIO_Init(LCD_D4_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LCD_D3_Pin;
  HAL_GPIO_Init(LCD_D3_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LCD_D2_Pin;
  HAL_GPIO_Init(LCD_D2_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LCD_D1_Pin;
  HAL_GPIO_Init(LCD_D1_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LCD_D0_Pin;
  HAL_GPIO_Init(LCD_D0_GPIO_Port, &GPIO_InitStruct);
}

/*********************************************************************
*
*       Public functions
*
**********************************************************************
*/
/*********************************************************************
*
*       LCD_X_Config
*
* Function description:
*   Called during the initialization process in order to set up the
*   display driver configuration.
*
*/
void LCD_X_Config(void) {
  GUI_DEVICE * pDevice;
  CONFIG_FLEXCOLOR Config = {0};
  GUI_PORT_API PortAPI = {0};
  //
  // Set display driver and color conversion
  //
  pDevice = GUI_DEVICE_CreateAndLink(GUIDRV_FLEXCOLOR, COLOR_CONVERSION, 0, 0);
  //
  // Display driver configuration, required for Lin-driver
  //
  LCD_SetSizeEx (0, XSIZE_PHYS , YSIZE_PHYS);
  LCD_SetVSizeEx(0, VXSIZE_PHYS, VYSIZE_PHYS);
  //
  // Orientation
  //
  Config.Orientation = DISPLAY_ORIENTATION; //GUI_SWAP_XY | GUI_MIRROR_Y;

  //mel
  Config.NumDummyReads = 1;  //needed for ili9341 shield

  GUIDRV_FlexColor_Config(pDevice, &Config);
  //
  // Set controller and operation mode
  //
  PortAPI.pfWrite8_A0  = LcdWriteReg;
  PortAPI.pfWrite8_A1  = LcdWriteData;
  PortAPI.pfWriteM8_A1 = LcdWriteDataMultiple;
  PortAPI.pfReadM8_A1  = LcdReadDataMultiple;
  GUIDRV_FlexColor_SetFunc(pDevice, &PortAPI, GUIDRV_FLEXCOLOR_F66709, GUIDRV_FLEXCOLOR_M16C0B8);

  //
  // Display driver configuration
  //
 if (LCD_GetSwapXY()) {
    LCD_SetSizeEx (0, YSIZE_PHYS, XSIZE_PHYS);
    LCD_SetVSizeEx(0, YSIZE_PHYS * NUM_VSCREENS, XSIZE_PHYS);
  } else {
    LCD_SetSizeEx (0, XSIZE_PHYS, YSIZE_PHYS);
    LCD_SetVSizeEx(0, XSIZE_PHYS, YSIZE_PHYS * NUM_VSCREENS);
  }


#if (USE_TOUCH == 1)

#ifndef SHIELD
 	 pConfig.Orientation = GUI_SWAP_XY;
#else
	 pConfig.Orientation = GUI_SWAP_XY | GUI_MIRROR_Y;		//shield XMAX on left
#endif
	 // - Orientation: Orientation of the touch screen if not the same as the physical orientation.
	 //                 A or-combination of the defines GUI_SWAP_XY, GUI_MIRROR_X and GUI_MIRROR_Y
	 //                 can be used.
	 pConfig.xLog0 = 0; 				// - xLog0      : Logical pixel value of horizontal reference point 0. Typically 0.
	 pConfig.xLog1 = XSIZE_PHYS -1; 	// - xLog1      : Logical pixel value of horizontal reference point 1. Typically horizontal screen resolution -1.
	 pConfig.xPhys0 = TOUCH_LEFT;		// - xPhys0     : Physical AD value of horizontal reference point 0.
	 pConfig.xPhys1 = TOUCH_RIGHT;		// - xPhys1     : Physical AD value of horizontal reference point 1.
	 pConfig.yLog0 = 0;					// - yLog0      : Logical pixel value of vertical reference point 0. Typically 0.
	 pConfig.yLog1 = YSIZE_PHYS - 1;	// - yLog1      : Logical pixel value of vertical reference point 1. Typically vertical screen resolution -1.
	 pConfig.yPhys0 = TOUCH_TOP;		// - yPhys0     : Physical AD value of vertical reference point 0.
	 pConfig.xPhys1 = TOUCH_BOTTOM;		// - yPhys1     : Physical AD value of vertical reference point 1.

	    // Set orientation of touch screen
	    //
	    GUI_TOUCH_SetOrientation(pConfig.Orientation);

	    //
	    // Calibrate touch screen
	    //
	    GUI_TOUCH_Calibrate(GUI_COORD_X, 0, WIDTH  - 1, TOUCH_LEFT, TOUCH_RIGHT);
	    GUI_TOUCH_Calibrate(GUI_COORD_Y, 0, HEIGHT - 1, TOUCH_TOP,  TOUCH_BOTTOM);

#endif
}

/*********************************************************************
*
*       LCD_X_DisplayDriver
*
* Function description:
*   This function is called by the display driver for several purposes.
*   To support the according task the routine needs to be adapted to
*   the display controller. Please note that the commands marked with
*   'optional' are not cogently required and should only be adapted if
*   the display controller supports these features.
*
* Parameter:
*   LayerIndex - Index of layer to be configured
*   Cmd        - Please refer to the details in the switch statement below
*   pData      - Pointer to a LCD_X_DATA structure
*
* Return Value:
*   < -1 - Error
*     -1 - Command not handled
*      0 - Ok
*/
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData) {
  int r;
  (void) LayerIndex;
  (void) pData;
  
  switch (Cmd) {
  case LCD_X_INITCONTROLLER: {
    //
    // Called during the initialization process in order to set up the
    // display controller and put it into operation. If the display
    // controller is not initialized by any external routine this needs
    // to be adapted by the customer...
    //
    // ...
	  InitLCD_ILI9341();

    return 0;
  }
  default:
    r = -1;
  }
  return r;
}

//ILI9341 specific functions

void InitLCD_ILI9341(void) {

	GPIO_InitTypeDef GPIO_InitStruct = {0};

	//Make sure analog pins used for touch input set to digital
	  //Configure GPIO pins : LCD_RS_Pin and LCD_CS digital output
	  GPIO_InitStruct.Pin = LCD_RS_Pin | LCD_CS_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(LCD_CS_GPIO_Port, &GPIO_InitStruct);

	HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LCD_RD_GPIO_Port, LCD_RD_Pin, GPIO_PIN_SET); 			//disable RD
	HAL_GPIO_WritePin(LCD_WR_GPIO_Port, LCD_WR_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_SET);
	//HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET); 		//disable CS
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET); 		//enable CS


	/* Force reset */
	  HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET);
	  HAL_Delay(30);
	  HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET);
	  HAL_Delay(10);
	  HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET);


	/* Delay for RST response */
	HAL_Delay(200);

	/* Software reset */
	LcdWriteReg(ILI9341_RESET);
	HAL_Delay(100);

	LcdWriteReg(ILI9341_POWERA);
	LcdWriteData(0x39);
	LcdWriteData(0x2C);
	LcdWriteData(0x00);
	LcdWriteData(0x34);
	LcdWriteData(0x02);
	LcdWriteReg(ILI9341_POWERB);
	LcdWriteData(0x00);
	LcdWriteData(0xC1);
	LcdWriteData(0x30);
	LcdWriteReg(ILI9341_DTCA);
	LcdWriteData(0x85);
	LcdWriteData(0x00);
	LcdWriteData(0x78);
	LcdWriteReg(ILI9341_DTCB);
	LcdWriteData(0x00);
	LcdWriteData(0x00);
	LcdWriteReg(ILI9341_POWER_SEQ);
	LcdWriteData(0x64);
	LcdWriteData(0x03);
	LcdWriteData(0x12);
	LcdWriteData(0x81);
	LcdWriteReg(ILI9341_PRC);
	LcdWriteData(0x20);
	LcdWriteReg(ILI9341_POWER1); //4.6V
	LcdWriteData(0x23);
	LcdWriteReg(ILI9341_POWER2);
	LcdWriteData(0x10);
	LcdWriteReg(ILI9341_VCOM1);
	LcdWriteData(0x3E);		//vcomh 4.250V
	LcdWriteData(0x28);		//vcoml -1.500V
	LcdWriteReg(ILI9341_VCOM2);
	LcdWriteData(0x86);
	LcdWriteReg(ILI9341_MAC); //memory access control
	LcdWriteData(0x48);	//48
	LcdWriteReg(ILI9341_PIXEL_FORMAT);
	LcdWriteData(0x55);	//rgb  16bits/pixel mcu 16bits/pixel
	LcdWriteReg(ILI9341_FRC); //Frame Rate Cntrol
	LcdWriteData(0x00); //diva freq == fosc
	LcdWriteData(0x18);	//79Hz refresh
	LcdWriteReg(ILI9341_DFC);//display function control
	LcdWriteData(0x08);
	LcdWriteData(0xA2); //normally white, gs - g1-g320, ss - s720-s1
	LcdWriteData(0x27);
	LcdWriteReg(ILI9341_3GAMMA_EN);
	LcdWriteData(0x00);
	LcdWriteReg(ILI9341_COLUMN_ADDR);
	LcdWriteData(0x00);
	LcdWriteData(0x00);
	LcdWriteData(0x00);
	LcdWriteData(0xEF);
	LcdWriteReg(ILI9341_PAGE_ADDR);
	LcdWriteData(0x00);
	LcdWriteData(0x00);
	LcdWriteData(0x01);
	LcdWriteData(0x3F);
	LcdWriteReg(ILI9341_GAMMA);
	LcdWriteData(0x01);
	LcdWriteReg(ILI9341_PGAMMA);
	LcdWriteData(0x0F);
	LcdWriteData(0x31);
	LcdWriteData(0x2B);
	LcdWriteData(0x0C);
	LcdWriteData(0x0E);
	LcdWriteData(0x08);
	LcdWriteData(0x4E);
	LcdWriteData(0xF1);
	LcdWriteData(0x37);
	LcdWriteData(0x07);
	LcdWriteData(0x10);
	LcdWriteData(0x03);
	LcdWriteData(0x0E);
	LcdWriteData(0x09);
	LcdWriteData(0x00);
	LcdWriteReg(ILI9341_NGAMMA);
	LcdWriteData(0x00);
	LcdWriteData(0x0E);
	LcdWriteData(0x14);
	LcdWriteData(0x03);
	LcdWriteData(0x11);
	LcdWriteData(0x07);
	LcdWriteData(0x31);
	LcdWriteData(0xC1);
	LcdWriteData(0x48);
	LcdWriteData(0x08);
	LcdWriteData(0x0F);
	LcdWriteData(0x0C);
	LcdWriteData(0x31);
	LcdWriteData(0x36);
	LcdWriteData(0x0F);
	LcdWriteReg(ILI9341_SLEEP_OUT);

	HAL_Delay(100);

	LcdWriteReg(ILI9341_DISPLAY_ON);
	LcdWriteReg(ILI9341_GRAM);
}

/*************************** End of file ****************************/

