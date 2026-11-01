#include "lcd.h"
#include "stm32f1_spl_i2c.h"
#include "PinCfg.h"

#define WRITE_SDA (GPIO_WriteBit(I2C_PORT, LCD_SDA_PIN, Bit_SET))
#define WRITE_SCL (GPIO_WriteBit(I2C_PORT, LCD_SCL_PIN, Bit_SET))

//************************** LOW HARDWARE LAYER  ****************************************************************//
static void LCD_Delay_ms(uint32_t ms)
{
	for (uint32_t i = 0; i < 24000 * ms; i++)
		;
}
static void LCD_WriteI2C(LCD *lcdinit, uint8_t bit, uint8_t Mode)
{
	char bit_high;
	char bit_low;
	uint8_t bit_I2C[4];
	bit_high = bit & 0xF0;
	bit_low = (bit << 4) & 0xF0;
	if (lcdinit->BACKLIGHT)
	{
		bit_high |= LCD_BACKLIGHT;
		bit_low |= LCD_BACKLIGHT;
	}
	if (Mode == LCD_DATA)
	{
		bit_high |= LCD_RS;
		bit_low |= LCD_RS;
	}
	else if (Mode == LCD_COMMAND)
	{
		bit_high &= ~LCD_RS;
		bit_low &= ~LCD_RS;
	}
	bit_I2C[0] = bit_high | LCD_EN;
	LCD_Delay_ms(1);
	bit_I2C[1] = bit_high;
	bit_I2C[2] = bit_low | LCD_EN;
	LCD_Delay_ms(1);
	bit_I2C[3] = bit_low;
	SPL_I2C_MasterSend(&LCD_I2C_Init, (uint8_t *)bit_I2C, sizeof(bit_I2C), lcdinit->ADDRESS);
}
//************************** MIDDLE FUNCTION LAYER ****************************************************************//
uint8_t LCD_I2C_Init(LCD *lcdinit, uint8_t Address, uint8_t Colums, uint8_t Rows)
{
	// cau hinh gpio pin
	GPIO_InitTypeDef LcdPinInit;
	LcdPinInit.GPIO_Pin = LCD_SDA_PIN | LCD_SCL_PIN;
	LcdPinInit.GPIO_Mode = GPIO_Mode_AF_OD;
	LcdPinInit.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(I2C_PORT, &LcdPinInit);

	// cau hinh thong so i2c cho lcd
	I2C_HandleTypedef lcd_i2cInit;

	lcd_i2cInit.pI2Cx = HI2C1;
	lcd_i2cInit.I2C_Config.I2C_Ack = I2C_Ack_Enable;
	lcd_i2cInit.I2C_Config.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	lcd_i2cInit.I2C_Config.I2C_ClockSpeed = 100000;
	lcd_i2cInit.I2C_Config.I2C_DutyCycle = I2C_DutyCycle_2;
	lcd_i2cInit.I2C_Config.I2C_Mode = I2C_Mode_I2C;
	lcd_i2cInit.I2C_Config.I2C_OwnAddress1 = 0x61;

	I2C_Init(HI2C1, &lcd_i2cInit.I2C_Config);
	I2C_Cmd(HI2C1, ENABLE);

	I2C_AcknowledgeConfig(HI2C1, ENABLE);

	uint8_t status = 1;
	lcdinit->ADDRESS = Address;
	lcdinit->COLUMS = Colums;
	lcdinit->ROWS = Rows;

	lcdinit->FUNCTIONSET = LCD_FUNCTIONSET | LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS;
	lcdinit->ENTRYMODE = LCD_ENTRYMODESET | LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	lcdinit->DISPLAYCTRL = LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	lcdinit->CURSORSHIFT = LCD_CURSORSHIFT | LCD_CURSORMOVE | LCD_MOVERIGHT;
	lcdinit->BACKLIGHT = LCD_BACKLIGHT;

	LCD_Delay_ms(50);
	LCD_WriteI2C(lcdinit, 0x33, LCD_COMMAND);
	//	LCD_Delay_ms(5);
	LCD_WriteI2C(lcdinit, 0x33, LCD_COMMAND);
	LCD_Delay_ms(5);
	LCD_WriteI2C(lcdinit, 0x32, LCD_COMMAND);
	LCD_Delay_ms(5);
	LCD_WriteI2C(lcdinit, 0x20, LCD_COMMAND);
	LCD_Delay_ms(5);

	LCD_WriteI2C(lcdinit, lcdinit->ENTRYMODE, LCD_COMMAND);
	LCD_WriteI2C(lcdinit, lcdinit->DISPLAYCTRL, LCD_COMMAND);
	LCD_WriteI2C(lcdinit, lcdinit->CURSORSHIFT, LCD_COMMAND);
	LCD_WriteI2C(lcdinit, lcdinit->FUNCTIONSET, LCD_COMMAND);

	LCD_WriteI2C(lcdinit, LCD_CLEARDISPLAY, LCD_COMMAND);
	LCD_WriteI2C(lcdinit, LCD_RETURNHOME, LCD_COMMAND);
	status = 0;
	return status;
}
void LCD_I2C_SetCursor(LCD *LCD, uint8_t Xpos, uint8_t Ypos)
{
	uint8_t DRAM_ADDRESS = 0x00;
	if (Xpos >= LCD->COLUMS)
	{
		Xpos = LCD->COLUMS - 1;
	}
	if (Ypos >= LCD->ROWS)
	{
		Ypos = LCD->ROWS - 1;
	}
	if (Ypos == 0)
	{
		DRAM_ADDRESS = 0x00 + Xpos;
	}
	else if (Ypos == 1)
	{
		DRAM_ADDRESS = 0x40 + Xpos;
	}
	else if (Ypos == 2)
	{
		DRAM_ADDRESS = 0x14 + Xpos;
	}
	else if (Ypos == 3)
	{
		DRAM_ADDRESS = 0x54 + Xpos;
	}
	LCD_WriteI2C(LCD, LCD_SETDDRAMADDR | DRAM_ADDRESS, LCD_COMMAND);
}
void LCD_I2C_WriteChar(LCD *lcdinit, char character)
{
	LCD_WriteI2C(lcdinit, character, LCD_DATA);
}
void LCD_I2C_WriteString(LCD *lcdinit, char *String)
{
	while (*String)
		LCD_I2C_WriteChar(lcdinit, *String++);
}
void LCD_I2C_Clear(LCD *lcdinit)
{
	LCD_WriteI2C(lcdinit, LCD_CLEARDISPLAY, LCD_COMMAND);
	LCD_Delay_ms(5);
}
void LCD_I2C_ReturnHome(LCD *lcdinit)
{
	LCD_WriteI2C(lcdinit, LCD_RETURNHOME, LCD_COMMAND);
	LCD_Delay_ms(5);
}
// CURSUR CONTROL FUNCTION
void LCD_I2C_CursorOn(LCD *lcdinit)
{
	lcdinit->DISPLAYCTRL |= LCD_CURSORON;
	LCD_WriteI2C(lcdinit, lcdinit->DISPLAYCTRL, LCD_COMMAND);
}
void LCD_I2C_CursorOff(LCD *lcdinit)
{
	lcdinit->DISPLAYCTRL &= ~LCD_CURSORON;
	LCD_WriteI2C(lcdinit, lcdinit->DISPLAYCTRL, LCD_COMMAND);
}
void LCD_I2C_BlinkOn(LCD *lcdinit)
{
	lcdinit->DISPLAYCTRL |= LCD_BLINKON;
	LCD_WriteI2C(lcdinit, lcdinit->DISPLAYCTRL, LCD_COMMAND);
}
void LCD_I2C_BlinkOff(LCD *lcdinit)
{
	lcdinit->DISPLAYCTRL &= ~LCD_BLINKON;
	LCD_WriteI2C(lcdinit, lcdinit->DISPLAYCTRL, LCD_COMMAND);
}
// DISPLAY CONTROL FUNCTION
void LCD_I2C_DisplayOff(LCD *lcdinit)
{
	lcdinit->DISPLAYCTRL |= LCD_DISPLAYOFF;
	LCD_WriteI2C(lcdinit, lcdinit->DISPLAYCTRL, LCD_COMMAND);
}
void LCD_I2C_DisplayOn(LCD *lcdinit)
{
	lcdinit->DISPLAYCTRL |= LCD_DISPLAYON;
	LCD_WriteI2C(lcdinit, lcdinit->DISPLAYCTRL, LCD_COMMAND);
}
void LCD_I2C_DisplayMove(LCD *lcdinit)
{
	lcdinit->DISPLAYCTRL |= LCD_DISPLAYMOVE;
	LCD_WriteI2C(lcdinit, lcdinit->DISPLAYCTRL, LCD_COMMAND);
}
void LCD_I2C_CursorMove(LCD *lcdinit)
{
	lcdinit->DISPLAYCTRL |= LCD_CURSORMOVE;
	LCD_WriteI2C(lcdinit, lcdinit->DISPLAYCTRL, LCD_COMMAND);
}
// CURSOR SHIFT FUNCTION
void LCD_I2C_MoveRight(LCD *lcdinit)
{
	lcdinit->CURSORSHIFT |= LCD_MOVERIGHT;
	LCD_WriteI2C(lcdinit, lcdinit->CURSORSHIFT, LCD_COMMAND);
}
void LCD_I2C_MoveLeft(LCD *lcdinit)
{
	lcdinit->CURSORSHIFT |= LCD_MOVELEFT;
	LCD_WriteI2C(lcdinit, lcdinit->CURSORSHIFT, LCD_COMMAND);
}
// ENTRY MODE FUNCTION
void LCD_I2C_ShiftIncrement(LCD *lcdinit)
{
	lcdinit->ENTRYMODE |= LCD_ENTRYSHIFTINCREMENT;
	LCD_WriteI2C(lcdinit, lcdinit->ENTRYMODE, LCD_COMMAND);
}
void LCD_I2C_ShiftDecrement(LCD *lcdinit)
{
	lcdinit->ENTRYMODE |= LCD_ENTRYSHIFTDECREMENT;
	LCD_WriteI2C(lcdinit, lcdinit->ENTRYMODE, LCD_COMMAND);
}
