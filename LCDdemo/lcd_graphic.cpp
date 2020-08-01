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

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "hwlib.h"

#include "lcd_graphic.h"
#include "LCDM.h"
#include "LCD_Driver.h"

//////////////////////////////////////////////
// lowest level API
void DRAW_Pixel(LCD_CANVAS *pCanvas, int X, int Y, int Color){
    int nLine;
    uint8_t *pFrame, Mask;

    nLine = Y >> 3; //Y/8;
    Mask = 0x01 << (Y % 8);
    pFrame = pCanvas->pFrame + pCanvas->Width*nLine + X;
    if (Color == 0x00)
        *pFrame &= ~Mask;
    else
        *pFrame |= Mask;
}


////////////////////////////////////////////////
// high-level API for developer


// !!!! noe. this fucntion is LCD hardware depentdent
void DRAW_Refresh(LCD_CANVAS *pCanvas){
	LCD_FrameCopy(pCanvas->pFrame);
}




void DRAW_Line(LCD_CANVAS *pCanvas, int X1, int Y1, int X2, int Y2, int Color){
    int X_Start, X_End;
    int Y_Start, Y_End;
    int x,y, acc=0, inc, x_delta, y_delta;

    if (X1 == X2){
        if (Y1 <= Y2){
            Y_Start = Y1;
            Y_End = Y2;
        }else{
            Y_Start = Y2;
            Y_End = Y1;
        }
        for(y=Y_Start;y<Y_End;y++)
            DRAW_Pixel(pCanvas, X1, y, Color);
    }else if (Y1 == Y2){
        if (X1 <= X2){
            X_Start = X1;
            X_End = X2;
        }else{
            X_Start = X2;
            X_End = X1;
        }
        for(x=X_Start;x<X_End;x++){
            DRAW_Pixel(pCanvas, x, Y1, Color);
        }
    }else if (abs(X1-X2) >= abs(Y1-Y2)){
        if (X1 <= X2){
            X_Start = X1;
            Y_Start = Y1;
            X_End = X2;
            Y_End = Y2;
        }else{
            X_Start = X2;
            Y_Start = Y2;
            X_End = X1;
            Y_End = Y1;
        }

        x_delta = X_End - X_Start;
        y_delta = Y_End - Y_Start;
        inc = (y_delta >= 0)?1:-1;
        y_delta = abs(y_delta);
        acc = x_delta/2;
        y = Y_Start;

        for(x=X_Start;x<X_End;x++){
            DRAW_Pixel(pCanvas, x, y, Color);
            acc +=  y_delta;
            if (acc >= x_delta){
                y += inc;
                acc -= x_delta;
            }
        }
    }else{
        if (Y1 <= Y2){
            X_Start = X1;
            Y_Start = Y1;
            X_End = X2;
            Y_End = Y2;
        }else{
            X_Start = X2;
            Y_Start = Y2;
            X_End = X1;
            Y_End = Y1;
        }

            y_delta = Y_End - Y_Start;
            x_delta = X_End - X_Start;
            inc = (x_delta >= 0)?1:-1;
            x_delta = abs(x_delta);
            acc = y_delta/2;
            x = X_Start;

            for(y=Y_Start;y<Y_End;y++){
                DRAW_Pixel(pCanvas, x, y, Color);
                acc +=  x_delta;
                if (acc >= y_delta){
                    x += inc;
                    acc -= y_delta;
                }
            }

    }


}

void DRAW_Rect(LCD_CANVAS *pCanvas, int X1, int Y1, int X2, int Y2, int Color){
    DRAW_Line(pCanvas, X1, Y1, X2, Y1, Color);
    DRAW_Line(pCanvas, X2, Y1, X2, Y2, Color);
    DRAW_Line(pCanvas, X2, Y2, X1, Y2, Color);
    DRAW_Line(pCanvas, X1, Y2, X1, Y1, Color);
}

void DRAW_Circle(LCD_CANVAS *pCanvas, int x0, int y0, int Radius, int Color){
  int x = Radius, y = 0;
  int radiusError = 1-x;
 
  while(x >= y)
  {
    DRAW_Pixel(pCanvas,x + x0, y + y0, Color);
    DRAW_Pixel(pCanvas,y + x0, x + y0, Color);
    DRAW_Pixel(pCanvas,-x + x0, y + y0, Color);
    DRAW_Pixel(pCanvas,-y + x0, x + y0, Color);
    DRAW_Pixel(pCanvas,-x + x0, -y + y0, Color);
    DRAW_Pixel(pCanvas,-y + x0, -x + y0, Color);
    DRAW_Pixel(pCanvas,x + x0, -y + y0, Color);
    DRAW_Pixel(pCanvas,y + x0, -x + y0, Color);
 
    y++;
        if(radiusError<0)
                radiusError+=2*y+1;
        else
        {
                x--;
                radiusError+=2*(y-x)+1;
        }
  }
}


void DRAW_Clear(LCD_CANVAS *pCanvas, int nValue){
#if 1
    int y,x;
    for(y=0;y<pCanvas->Height;y++){
        for(x=0;x<pCanvas->Width;x++){
            DRAW_Pixel(pCanvas, x, y, nValue);
        }
    }
#else
    int y,x;
    uint8_t *pFrame, Mask;


    pFrame = pCanvas->pFrame;
    memset(pFrame, 0x00, pCanvas->FrameSize);
    for(y=0;y<pCanvas->Height;y++){
        Mask = 0x80;
        for(x=0;x<pCanvas->Width;x++){
            if (nValue != 0x00){
                *pFrame |= Mask;
            }
            Mask >>= 1;
            if (Mask == 0x00){
                Mask = 0x80;
                pFrame++;
            }

        }
    }
#endif


}


#ifdef SUPPORT_LCD_FONT
////////////////////////////////////////////////
/// FONT API ///////////////////////////////////
////////////////////////////////////////////////

void DRAW_PrintChar(LCD_CANVAS *pCanvas, int X0, int Y0, char Text, int Color, FONT_TABLE *font_table){
    unsigned char *pFont;
    uint8_t Mask;
    int x, y, p;

    for(y=0;y<2;y++){
        Mask = 0x01;
        for(p=0;p<8;p++){
            pFont = font_table->pBitmap[(unsigned char)Text][y];
        	
//            pFont = font_table[y];
//            pFont = (unsigned char *)&(font_table[Text][y][0]);
//            pFont = &(font_courier_new_16x16[(unsigned char)Text][y][0]);
            for(x=0;x<16;x++){
                if (Mask & *pFont)
                    DRAW_Pixel(pCanvas, X0+x, Y0+y*8+p, Color);
                pFont++;
            }
            Mask <<= 1;
        }
    }

}

void DRAW_PrintString(LCD_CANVAS *pCanvas, int X0, int Y0, char* pText, int Color, FONT_TABLE *font_table){

    int nLen, i;

    nLen = strlen(pText);

    for(i=0;i<nLen;i++){
        DRAW_PrintChar(pCanvas, X0+i*font_table->FontWidth, Y0, *(pText+i), Color, font_table);
    }

}


#endif //SUPPORT_LCD_FONT
