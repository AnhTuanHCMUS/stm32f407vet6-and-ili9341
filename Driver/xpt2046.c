
#include "xpt2046.h"

#define T_IRQ XPT2046_ReadIRQ()
#define T_CS_ON XPT2046_SetCS()
#define T_CS_OFF XPT2046_ResetCS()



extern SPI_HandleTypeDef hspi2;
extern void Error_Handler(void);
static uint8_t XPT2046_initilazed = 0;

static void XPT2046_Write_Byte(uint8_t num)
{
	hspi2.Instance->CR2 &= ~(SPI_DATASIZE_16BIT); // Set 8 bit mode
	__HAL_SPI_ENABLE(&hspi2);
	if (HAL_SPI_Transmit(&hspi2, &num, 1, 1000) != HAL_OK) {
		Error_Handler();
	}
	__HAL_SPI_DISABLE(&hspi2);
}
void XPT2046_Init(void)
{
	XPT2046_initilazed = 1;
}

void XPT2046_SetCS(void)
{
	HAL_GPIO_WritePin(T_CS_GPIO_Port, T_CS_Pin, GPIO_PIN_SET);
}

void XPT2046_ResetCS(void)
{
	HAL_GPIO_WritePin(T_CS_GPIO_Port, T_CS_Pin, GPIO_PIN_RESET);
}

uint16_t XPT2046_Read_AD(uint8_t CMD)
{
	uint8_t num[2];
	uint16_t ret;

	T_CS_OFF;
	XPT2046_Write_Byte(CMD);
	HAL_Delay(6);

	hspi2.Instance->CR2 |= SPI_DATASIZE_16BIT; // Set 16 bit mode
	__HAL_SPI_ENABLE(&hspi2);
	if (HAL_SPI_Receive(&hspi2, num, 2, 1000) != HAL_OK) {
		Error_Handler();
	}
	__HAL_SPI_DISABLE(&hspi2);
	T_CS_ON;
	ret = (num[0] << 8) | num[1];
	return ret;
}


void XPT2046_Update(uint16_t *x, uint16_t *y, uint16_t *z)
{
	int32_t datax = 0;
	int32_t datay = 0;

	static uint32_t ptime = 0;

	if (XPT2046_initilazed == 0) {
		return;
	}

	if (HAL_GetTick() - ptime < MSEC_THRESHOLD) {
		return;
	}

	int16_t z1 = XPT2046_Read_AD(0xB1); // z1
	int32_t intz = z1 + 4095;
	int16_t z2 = XPT2046_Read_AD(0xC1); // z2
	intz -= z2;
	if (intz >= Z_THRESHOLD)
	{
		XPT2046_Read_AD(0x91);  // dummy 1st X measure
		int dem = 20;
		while (dem > 0 )
		{
			datax += (uint32_t)XPT2046_Read_AD(0x91);
			datay += (uint32_t)XPT2046_Read_AD(0xD1);
			dem--;
		}
	}
	else
	{
		datax = datay =  0;
		intz = 0;
	}
	ptime = HAL_GetTick();
	int16_t intx = datax / 20;
	int16_t inty = datay / 20;
	*x = intx;
	*y = inty;
	*z = intz;

}

void  ConvXPTtoILI(uint16_t *x, uint16_t *y)
{


    uint16_t tx,ty;
	tx = (uint16_t)(((uint32_t)*x - XPT_XMIN) * GUI_WIDTH / XPT_WIDTH);
	tx = (tx < 0) ? 0 : tx;
	tx = (tx >= GUI_WIDTH) ? 0 : tx;

	ty = (uint16_t)(((uint32_t)*y - XPT_YMIN) * GUI_HEIGHT / XPT_HEIGHT);
	ty = (ty < 0) ? 0 : ty;
	ty = (ty >= GUI_HEIGHT) ? 0 : ty;

	*x = tx;
	*y = ty;

}
bool XPT2046_TouchPressed()
{
    return HAL_GPIO_ReadPin(T_PEN_GPIO_Port, T_PEN_Pin) == GPIO_PIN_RESET;
}
void XPT2046_UnTouch()
{
	HAL_GPIO_WritePin(T_PEN_GPIO_Port, T_PEN_Pin, GPIO_PIN_SET);
}
