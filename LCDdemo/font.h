#ifndef __FONT_H__
#define __FONT_H__

// font table information
//   font size                : 8 x 16 (pixels)
//   font cell size for lcd   : 16 x 16 (pixels)  16 x 2 (byte_array)
//   ascii coding index range : 0x00 ~ 0xff
//   cell looking up method   : &font_table[ascii_code][0][0]

//#define FONT_SIZE_X (8)
//#define FONT_SIZE_Y (16)

//#define CELL_SIZE_X (16)
//#define CELL_SIZE_Y (16)

#define LCD_CELL_SIZE_X (16)
#define LCD_CELL_SIZE_Y (2)

//#define FONTTBL_ASCII_CODE_MIN (0x00)
//#define FONTTBL_ASCII_CODE_MAX (0xff)


typedef unsigned char FONT_BITMAP[LCD_CELL_SIZE_Y][LCD_CELL_SIZE_X];

typedef struct{
    int FontWidth;
    int FontHeight;
    int CellWidth;
    int CellHeight;
    int CodeStart;
    int CodeEnd;
    int BitPerPixel;
    FONT_BITMAP *pBitmap;
}FONT_TABLE;

// hint: unsigned char font_table[ascii_code][lcd_cell_height/8][lcd_cell_width]
extern FONT_TABLE  font_16x16;

#endif // __FONT_H__

