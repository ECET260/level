#include "ts.h"
#include "main.h"

extern ADC_HandleTypeDef hadc1;


Pen_Holder Pen_Point;

unsigned char flag=0;
uint32_t x=0;


void GUI_TOUCH_X_ActivateX(void)
{

}

void GUI_TOUCH_X_ActivateY(void)
{

}

// Y1_Pin LCD_CS_Pin //A3 	need two analog inputs
// X1_Pin LCD_RS_Pin //A2
// Y2_Pin LCD_D1_Pin //9 730ohms
// X2_Pin LCD_D0_Pin //8 280ohms

//#define YP A3  // must be an analog pin, use "An" notation!
//#define XM A2  // must be an analog pin, use "An" notation!
//#define YM 9   // can be a digital pin
//#define XP 8   // can be a digital pin

//#define YP A1  //must be an analog pin, use "An" notation!
//#define XM A2  //must be an analog pin, use "An" notation!
//#define YM 7   //can be a digital pin 647 ohms
//#define XP 6   //can be a digital pin 322 ohms

int GUI_TOUCH_X_MeasureY(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	ADC_ChannelConfTypeDef sConfig = {0};
	int X = 0;

	//setup for touch

	//X

	  /** Configure Regular Channel */
	  sConfig.Channel = ADC_CHANNEL_Y1;
	  sConfig.Rank = ADC_REGULAR_RANK_1;
	  sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
	  sConfig.SingleDiff = ADC_SINGLE_ENDED;
	  sConfig.OffsetNumber = ADC_OFFSET_NONE;
	  sConfig.Offset = 0;
	  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }


	  GPIO_InitStruct.Pin = Y1_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  HAL_GPIO_Init(Y1_PORT, &GPIO_InitStruct);

	  //Configure GPIO pins : Y2 input with pullups
	  GPIO_InitStruct.Pin = Y2_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	  GPIO_InitStruct.Pull = GPIO_PULLUP;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(Y2_PORT, &GPIO_InitStruct);

	  //Configure GPIO pins : X1_Pin output
	  GPIO_InitStruct.Pin = X1_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(X1_PORT, &GPIO_InitStruct);

	  //Configure GPIO pins : X2_Pin output
	  GPIO_InitStruct.Pin = X2_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(X2_PORT, &GPIO_InitStruct);

	  HAL_GPIO_WritePin(X1_PORT,X1_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(X2_PORT,X2_Pin, GPIO_PIN_SET);

	  HAL_Delay(1);

	  HAL_ADC_Start(&hadc1);
	  HAL_ADC_PollForConversion(&hadc1, 200);
	  X = HAL_ADC_GetValue(&hadc1);


	  //Configure GPIO pins : Y1_Pin output
	  GPIO_InitStruct.Pin = Y1_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(Y1_PORT, &GPIO_InitStruct);


	  //Configure GPIO pins : Y2 output
	  GPIO_InitStruct.Pin = Y2_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(Y2_PORT, &GPIO_InitStruct);

	  return X;

}


int GUI_TOUCH_X_MeasureX(void)
{
	  //Y
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	ADC_ChannelConfTypeDef sConfig = {0};

	  int Y=0;

	  /** Configure Regular Channel */
	  sConfig.Channel = ADC_CHANNEL_X1;
	  sConfig.Rank = ADC_REGULAR_RANK_1;
	  sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
	  sConfig.SingleDiff = ADC_SINGLE_ENDED;
	  sConfig.OffsetNumber = ADC_OFFSET_NONE;
	  sConfig.Offset = 0;
	  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }



	  GPIO_InitStruct.Pin = X1_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  HAL_GPIO_Init(X1_PORT, &GPIO_InitStruct);


	  //Configure GPIO pins : X2 input with pullups
	  GPIO_InitStruct.Pin = X2_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	  GPIO_InitStruct.Pull = GPIO_PULLUP;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(X2_PORT, &GPIO_InitStruct);

	  HAL_GPIO_WritePin(Y1_PORT,Y1_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(Y2_PORT,Y2_Pin, GPIO_PIN_SET);

	  HAL_Delay(1);

	  HAL_ADC_Start(&hadc1);
	  HAL_ADC_PollForConversion(&hadc1, 200);
	  Y = HAL_ADC_GetValue(&hadc1);


	  //Configure GPIO pins : X1_Pin output
	  GPIO_InitStruct.Pin = X1_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(X1_PORT, &GPIO_InitStruct);


	  //Configure GPIO pins : X2 output
	  GPIO_InitStruct.Pin = X2_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(X2_PORT, &GPIO_InitStruct);

	  return Y;
}


void GUI_TOUCH_X_MeasureXY(uint32_t *sumx, uint32_t *sumy)
{
	  uint32_t i;
	  uint32_t x=0;
	  uint32_t y=0;

	  for(i=0;i<8;i++)
	  {
	   	TPReadXY(&x, &y);
	   	(*sumx)+=x;
	   	(*sumy)+=y;
	  }

	  *sumx = (*sumx)>>3;
	  *sumy = (*sumy)>>3;
}





// Y1_Pin LCD_CS_Pin //A3 	need two analog inputs
// X1_Pin LCD_RS_Pin //A2
// Y2_Pin LCD_D1_Pin //9
// X2_Pin LCD_D0_Pin //8


void TPReadXY(uint32_t *X, uint32_t *Y)
{

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	ADC_ChannelConfTypeDef sConfig = {0};

	//setup for touch

	//X

	  /** Configure Regular Channel */
	  sConfig.Channel = ADC_CHANNEL_Y1;
	  sConfig.Rank = ADC_REGULAR_RANK_1;
	  sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
	  sConfig.SingleDiff = ADC_SINGLE_ENDED;
	  sConfig.OffsetNumber = ADC_OFFSET_NONE;
	  sConfig.Offset = 0;
	  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }


	  GPIO_InitStruct.Pin = Y1_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  HAL_GPIO_Init(Y1_PORT, &GPIO_InitStruct);

	  //Configure GPIO pins : Y2 input with pullups
	  GPIO_InitStruct.Pin = Y2_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	  GPIO_InitStruct.Pull = GPIO_PULLUP;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(Y2_PORT, &GPIO_InitStruct);

	  //Configure GPIO pins : X1_Pin output
	  GPIO_InitStruct.Pin = X1_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(X1_PORT, &GPIO_InitStruct);

	  //Configure GPIO pins : X2_Pin output
	  GPIO_InitStruct.Pin = X2_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(X2_PORT, &GPIO_InitStruct);

	  HAL_GPIO_WritePin(X1_PORT,X1_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(X2_PORT,X2_Pin, GPIO_PIN_SET);

	  HAL_Delay(1);

	  HAL_ADC_Start(&hadc1);
//	  HAL_ADC_PollForConversion(&hadc1, 200);
	  *X = HAL_ADC_GetValue(&hadc1);


	  //Configure GPIO pins : Y1_Pin output
	  GPIO_InitStruct.Pin = Y1_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(Y1_PORT, &GPIO_InitStruct);


	  //Configure GPIO pins : Y2 output
	  GPIO_InitStruct.Pin = Y2_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(Y2_PORT, &GPIO_InitStruct);


	  //Y

	  /** Configure Regular Channel */
	  sConfig.Channel = ADC_CHANNEL_X1;
	  sConfig.Rank = ADC_REGULAR_RANK_1;
	  sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
	  sConfig.SingleDiff = ADC_SINGLE_ENDED;
	  sConfig.OffsetNumber = ADC_OFFSET_NONE;
	  sConfig.Offset = 0;
	  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }



	  GPIO_InitStruct.Pin = X1_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  HAL_GPIO_Init(X1_PORT, &GPIO_InitStruct);


	  //Configure GPIO pins : X2 input with pullups
	  GPIO_InitStruct.Pin = X2_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	  GPIO_InitStruct.Pull = GPIO_PULLUP;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(X2_PORT, &GPIO_InitStruct);

	  HAL_GPIO_WritePin(Y1_PORT,Y1_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(Y2_PORT,Y2_Pin, GPIO_PIN_SET);

	  HAL_Delay(1);

	  HAL_ADC_Start(&hadc1);
//	  HAL_ADC_PollForConversion(&hadc1, 200);
	  *Y = HAL_ADC_GetValue(&hadc1);


	  //Configure GPIO pins : X1_Pin output
	  GPIO_InitStruct.Pin = X1_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(X1_PORT, &GPIO_InitStruct);


	  //Configure GPIO pins : X2 output
	  GPIO_InitStruct.Pin = X2_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(X2_PORT, &GPIO_InitStruct);


}


#define TSLEFT 3520
#define TSRIGHT 450
#define TSTOP 3515
#define TSBOTTOM 580

//hard code these so not to recalculate every time
//#define XDIVISOR (TSRIGHT-TSLEFT)/320
//#define YDIVISOR (TSBOTTOM-TSTOP)/240


#define XDIVISOR 9.59	//(TSRIGHT-TSLEFT)/320   	(3520-450)/320=9.59
#define YDIVISOR 12.23	//(TSBOTTOM-TSTOP)/240		(3515-580)/240=12.23


void Convert_Pos(void)
{

	uint32_t potx = 0;
	uint32_t poty = 0;

	GUI_TOUCH_X_MeasureXY(&potx, &poty);

	Pen_Point.X=potx;
	Pen_Point.Y=poty;


	Pen_Point.X0=(int)320-((Pen_Point.Y-TSRIGHT)/XDIVISOR); //widescreen xy, 00 upper left
	Pen_Point.Y0=(int)240-((Pen_Point.X-TSBOTTOM)/YDIVISOR);

	pstate.x = Pen_Point.X0;
	pstate.y = Pen_Point.Y0;

	if(pstate.x || pstate.y)
		pstate.Pressed = 1;		//pressed active lo   pfGetPENIRQ() works for button
	else
		pstate.Pressed = 0;

	GUI_PID_StoreState(&pstate);



}

