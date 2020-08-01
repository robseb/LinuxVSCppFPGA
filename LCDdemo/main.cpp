//
//
//            ########   ######     ##    ##  #######   ######  ########  #######
//            ##     ## ##    ##     ##  ##  ##     ## ##    ##    ##    ##     ##
//            ##     ## ##            ####   ##     ## ##          ##    ##     ##
//            ########   ######        ##    ##     ## ##          ##    ##     ##
//            ##   ##         ##       ##    ##     ## ##          ##    ##     ##
//            ##    ##  ##    ##       ##    ##     ## ##    ##    ##    ##     ##
//            ##     ##  ######        ##     #######   ######     ##     #######
//
//		    ----		LCD Display Demo for the Terasic DE10 Standard		  ----
//
//			@Description:
//				Acessing the 128x64 Display Module of the Terasic DE10 Standard Board
//			     
//			
//				+ Read the MSEL switch postion of the Development board  
//				+ Read the Status of the FPGA fabric		
//				+ Change the FPGA configuration 
//				+ Change the FPGA configuration back 
//
//			@Supported platforms:
//					rsyocto Version 1.032 or later (https://github.com/robseb/rsyocto/)
//
//			@Other Linux:
//					Embedded Linux with Intel Cyclone V SoC-FPGAs with the pre-instalment of the Intel hwlib .h-files
//					(/usr/include)
//
//			@Designer:
// 					Robin Sebastian (https://github.com/robseb) 
//					      git@robseb.de
//
//

//
// Includes 
//



#include "LCDM.h"
#include "LCD_Driver.h"

#include <iostream>
#include <fstream>
#include <string.h>
using namespace std;

#include "lcd_graphic.h"
#include "font.h"


#define LCD_WIDTH     128
#define LCD_HEIGHT    64

//
// Demo configuration
//

void LCD_Init(void);

/*! \brief Check that used Board is a DE10 Standard Development board
 */
bool check_de10standard(void)
{
	std::ifstream input("/usr/rsyocto/suppBoard.txt");
	std::string line;
	getline(input, line);
	return (line.find("Terasic DE10 Standard")!= string::npos);
}

/*
 * Main function
 */
int main(int argc, const char* argv[])
{
	cout << "\n\nHPS - FPGA Manager Demo to write the FPGA fabric " << endl;

	// Check that the demo runs on a Terasic DE10 Standard Board
	if (!check_de10standard())
	{
		cout << "\nERROR: Demo must be executed on a Terasic DE10 Standard Board" << endl;
		return -1;
	}

	// Initialisation of the SPI LCD Display 
	LCDM_init();


	cout << "End of Application\n" << endl;

    LCD_Init();


    LCD_CANVAS LcdCanvas;

    LcdCanvas.Width = LCD_WIDTH;
    LcdCanvas.Height = LCD_HEIGHT;
    LcdCanvas.BitPerPixel = 1;
    LcdCanvas.FrameSize = LcdCanvas.Width * LcdCanvas.Height / 8;
    LcdCanvas.pFrame = (uint8_t*)malloc(LcdCanvas.FrameSize);

    if (LcdCanvas.pFrame == NULL)
    {
        cout << "\nERROR: Failed to allocate Frame buffer" << endl;
        return -1;
    }

    DRAW_Clear(&LcdCanvas, LCD_WHITE);

    // demo grphic api    
    DRAW_Rect(&LcdCanvas, 0, 0, LcdCanvas.Width - 1, LcdCanvas.Height - 1, LCD_BLACK); // retangle
    DRAW_Circle(&LcdCanvas, 10, 10, 6, LCD_BLACK);
    DRAW_Circle(&LcdCanvas, LcdCanvas.Width - 10, 10, 6, LCD_BLACK);
    DRAW_Circle(&LcdCanvas, LcdCanvas.Width - 10, LcdCanvas.Height - 10, 6, LCD_BLACK);
    DRAW_Circle(&LcdCanvas, 10, LcdCanvas.Height - 10, 6, LCD_BLACK);

    // demo font
    DRAW_PrintString(&LcdCanvas, 40, 5, "Hello", LCD_BLACK, &font_16x16);
    DRAW_PrintString(&LcdCanvas, 40, 5 + 16, "SoCFPGA", LCD_BLACK, &font_16x16);
    DRAW_PrintString(&LcdCanvas, 40, 5 + 32, "Terasic ", LCD_BLACK, &font_16x16);
    DRAW_Refresh(&LcdCanvas);


    free(LcdCanvas.pFrame);

	return 1;
}

void LCD_Init(void) {

    //  Display_Reset();


      // (3) ADC select: Normal display (ADC command D0 = “L”)
      //Display_SetADC(true); // normal


      // Common output state selection (~normal)
    LCDDrv_SetOuputStatusSelect(false); // invert to match mechanisum

    // (6) LCD Bias (true:default)
  //  Display_SetBias(true);//    

    // (4 ?) Power control register (D2, D1, D0) = (follower, regulator, booster) = (1, 1, 1)
    LCDDrv_SetPowerControl(0x07);

    // Setting the build-in resistance radio
   // Display_SetResistorRatio(4);

    // Electronic volume control (adjust brightnesss: 0x01~0x3F, 0x20:disable)
    //  Display_SetElectricVolume(0x20);
    //Display_SetOsc(true);

    // delay 
    //usleep(5u);

    // (9) set display start line: at first line
    LCDDrv_SetStartLine(0);


    // (11) Page address register set at page 0
    LCDDrv_SetPageAddr(0);
    // (10) Column address counter set at address 0
    LCDDrv_SetColAddr(0);

    // Display on
    LCDDrv_Display(true);
}
