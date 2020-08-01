// ============================================================================
// Copyright (c) 2013 by Terasic Technologies Inc.
// ============================================================================
//
// Permission:
//
//   Terasic grants permission to use and modify this code for use
//   in synthesis for all Terasic Development Boards and Altera Development 
//   Kits made by Terasic.  Other use of this code, including the selling 
//   ,duplication, or modification of any portion is strictly prohibited.
//
// Disclaimer:
//
//   This VHDL/Verilog or C/C++ source code is intended as a design reference
//   which illustrates how these types of functions can be implemented.
//   It is the user's responsibility to verify their design for
//   consistency and functionality through the use of formal
//   verification methods.  Terasic provides no warranty regarding the use 
//   or functionality of this code.
//
// ============================================================================
//           
//  Terasic Technologies Inc
//  9F., No.176, Sec.2, Gongdao 5th Rd, East Dist, Hsinchu City, 30070. Taiwan
//  
//  
//                     web: http://www.terasic.com/  
//                     email: support@terasic.com
//
// ============================================================================

#include "LCDM.h"
#include "LCD_Driver.h"





void LCDDrv_Display(bool bOn){
    LCDHW_Write8(0,bOn?0xAF:0xAE); 
}

// Specifies line address (refer to Figure 6) to determine the initial display line
// When this command changes the line address, smooth scrolling or a page change takes place
void LCDDrv_SetStartLine(uint8_t StartLine){
    LCDHW_Write8(0,0x40 | (StartLine & 0x3F));
}


void LCDDrv_SetPageAddr(uint8_t PageAddr){
    LCDHW_Write8(0,0xB0 | (PageAddr & 0x0F));
}


void LCDDrv_SetColAddr(uint8_t ColAddr){
    LCDHW_Write8(0,0x00 | (ColAddr & 0x0F)); // low 4 bits
    LCDHW_Write8(0,0x10 | ((ColAddr >> 4) & 0x0F)); // high 4-bits
    

}



void LCDDrv_WriteData(uint8_t Data){
    LCDHW_Write8(1, Data);
}

void LCDDrv_WriteMultiData(uint8_t * Data, uint16_t num){
    int i;
    for(i=0;i<num;i++)
        LCDHW_Write8(1, *(Data+i));
}




void LCDDrv_SetADC(bool bNormal){
    LCDHW_Write8(0, bNormal?0xA0:0xA1);
}

void LCDDrv_SetReverse(bool bNormal){
    LCDHW_Write8(0, bNormal?0xA6:0xA7);
}

void LCDDrv_EntireOn(bool bEntireOn){
    LCDHW_Write8(0, bEntireOn?0xA5:0xA4);
}

void LCDDrv_SetBias(bool bDefault){
    LCDHW_Write8(0, bDefault?0xA2:0xA3);
}

// Once Read-Modify-Write is issued, column address is not incremental by Read Display Data command 
// but incremental by Write Display Data command only.
// It continues until End command is issued.
void LCDDrv_ReadModifyWrite_Start(void){
    LCDHW_Write8(0, 0xE0);
}

void LCDDrv_ReadModifyWrite_End(void){
    LCDHW_Write8(0, 0xEE);
}

void LCDDrv_Reset(void){
    LCDHW_Write8(0, 0xE2);
}


void LCDDrv_SetOsc(bool bDefault){
    LCDHW_Write8(0, bDefault?0xE4:0xE5);
}

void LCDDrv_SetPowerControl(uint8_t PowerMask){
    LCDHW_Write8(0, 0x28 | (PowerMask & 0x7));
}

void LCDDrv_SetResistorRatio(uint8_t Value){
    LCDHW_Write8(0, 0x20 | (Value & 0x7));
}


void LCDDrv_SetElectricVolume(uint8_t Value){
    // write two bytes
    LCDHW_Write8(0, 0x81);
    LCDHW_Write8(0, Value & 0x3F);
}

void LCDDrv_SetOuputStatusSelect(bool bNormal){
    LCDHW_Write8(0, bNormal?0xC0:0xC8);
}

void LCD_SetStartAddr(uint8_t x, uint8_t y) {
    LCDDrv_SetPageAddr(y / 8);
    LCDDrv_SetColAddr(x);
}


void LCD_Clear(void) {
    int Page, i;
    for (Page = 0; Page < 8; Page++) {
        LCDDrv_SetPageAddr(Page);
        LCDDrv_SetColAddr(0);
        for (i = 0; i < 132; i++) {
            LCDDrv_WriteData(0x00);
        }
    }
}


void LCD_FrameCopy(uint8_t* Data) {
    int Page;

    uint8_t* pPageData = Data;
    for (Page = 0; Page < 8; Page++) {
        LCD_SetStartAddr(0, Page * 8);
        LCDDrv_WriteMultiData(pPageData, 128);
        pPageData += 128;
    }
}