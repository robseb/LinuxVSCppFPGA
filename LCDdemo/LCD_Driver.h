/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
//[] END OF FILE
#ifndef _NT7534_DRIVER_H_
#define _NT7534_DRIVER_H_


#include <stdbool.h>
#include "hwlib.h"					// Intel hwlib 

#define SPI_GRAPHIC_LCD


void LCDDrv_Display(bool bOn);
void LCDDrv_SetStartLine(uint8_t StartLine);
void LCDDrv_SetPageAddr(uint8_t PageAddr);
void LCDDrv_SetColAddr(uint8_t ColAddr);
void LCDDrv_WriteData(uint8_t Data);
void LCDDrv_WriteMultiData(uint8_t * Data, uint16_t num);

void LCDDrv_SetADC(bool bNormal);
void LCDDrv_SetReverse(bool bNormal);
void LCDDrv_SetBias(bool bEntireOn);
void LCDDrv_SetBias(bool bDefault);
void LCDDrv_ReadModifyWrite_Start(void);
void LCDDrv_ReadModifyWrite_End(void);
void LCDDrv_Reset(void);
void LCDDrv_SetOsc(bool bDefault);
void LCDDrv_SetPowerControl(uint8_t PowerMask);
void LCDDrv_SetResistorRatio(uint8_t Value);
void LCDDrv_SetOuputResistorRatio(uint8_t Value);
void LCDDrv_SetOuputStatusSelect(bool bNormal);

void LCD_FrameCopy(uint8_t* Data);

#endif //_NT7534_DRIVER_H_