#ifndef _INC_LCD_GRAPHIC_H_
#define _INC_LCD_GRAPHIC_H_

#define SUPPORT_LCD_FONT

#ifdef __cplusplus
extern "C" {
#endif


typedef struct{
    int Width;
    int Height;
    int BitPerPixel;
    int FrameSize;
    uint8_t *pFrame;
}LCD_CANVAS;

#define LCD_WHITE   0x00
#define LCD_BLACK   0xFF

void DRAW_Clear(LCD_CANVAS *pCanvas, int Color);
void DRAW_Line(LCD_CANVAS *pCanvas, int X1, int Y1, int X2, int Y2, int Color);
void DRAW_Pixel(LCD_CANVAS *pCanvas, int X, int Y, int Color);
void DRAW_Rect(LCD_CANVAS *pCanvas, int X1, int Y1, int X2, int Y2, int Color);
void DRAW_Circle(LCD_CANVAS *pCanvas, int x0, int y0, int Radius, int Color);
void DRAW_Refresh(LCD_CANVAS *pCanvas);


#ifdef SUPPORT_LCD_FONT

#include "font.h"
void DRAW_PrintChar(LCD_CANVAS *pCanvas, int X0, int Y0, char Text, int Color, FONT_TABLE *font_table);
void DRAW_PrintString(LCD_CANVAS *pCanvas, int X0, int Y0, char *pText, int Color, FONT_TABLE *font_table);

#endif //SUPPORT_LCD_FONT


#ifdef __cplusplus
}
#endif


#endif

