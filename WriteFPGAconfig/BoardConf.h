#pragma once


// 
// Terasic DE10 Boards HPS LED and HPS Push-button Pin assignment 
//
//	Signal	|	HPS GPIO	|	Register	| hwlib Port	|	hwlib Pin	
//	---------------------------------------------------------------------
//	HPS_LED |	 GPIO53		|	GPIO1[24]	|	GPIOB		|	24
//	HPS_KEY |	 GPIO54		|	GPIO1[25]	|	GPIOB		|	25
//
//

// GPIO assigment for HPS_LED
#define DEMO_LED_PORT ALT_GPIO_PORTB
#define DEMO_LED_PIN 24

// GPIO assigment for HPS_KEY
#define DEMO_KEY_PORT ALT_GPIO_PORTB
#define DEMO_KEY_PIN 25

//
// Demo configuration
//
#define DEMO_DEVBOARD_DE10NANO    1 // Terasic DE10 Nano
#define DEMO_DEVBOARD_DE10STD     2 // Terasic DE10 Standard
#define DEMO_DEVBOARD_DE0NANOSOC  3 // Terasic DE0 Nano SoC
#define DEMO_DEVBOARD_UNKNOWN     0

// Select your Development Board
#define DEMO_SELECTED_BOARD DEMO_DEVBOARD_UNKNOWN

#if DEMO_SELECTED_BOARD == DEMO_DEVBOARD_UNKNOWN
#error "Please select your development board!"
#endif 

#if DEMO_SELECTED_BOARD == DEMO_DEVBOARD_DE10STD
#define DEMO_LW_SYSTEMID_OFFSET 0x30				// System ID for Intel FPGA address offset
#define DEMO_LW_SYSTEMID		0xCAFEACDC			// Unique ID of the Intel FPGA address offset
#define DEMO_LW_LED_MAXVALUE	1023				// Maximal countebil vaule on the LEDs
#define DEMO_LW_LED_OFFSET		0x20				// PIO (Parallel I/O) Intel FPGA for accessing the FPGA LEDs address offset
#define DEMO_LW_KEYS_OFFSET		0x10				// PIO (Parallel I/O) Intel FPGA for accessing the FPGA Keys address offset
#define DEMO_LW_SWICHES_OFFSET	0x00				// PIO (Parallel I/O) Intel FPGA for accessing the FPGA Swiches address offset
#define DEMO_EN_SEVENSIG		1					// Enable the Seven Sigment Display 
#define DEMO_LW_SEVENSIG_OFFSET 0x38				// Seven Sigment Display IP address offset 
#define DEMO_LW_ADC_OFFSET		0x40				// Analog Deives LTC2308 ADC IP address offset 
#elif DEMO_SELECTED_BOARD == DEMO_DEVBOARD_DE10NANO
#define DEMO_LW_SYSTEMID_OFFSET 0x30				// System ID for Intel FPGA address offset
#define DEMO_LW_SYSTEMID		0xCAFEACDC			// Unique ID of the Intel FPGA address offset
#define DEMO_LW_LED_MAXVALUE	254					// Maximal countebil vaule on the LEDs
#define DEMO_LW_LED_OFFSET		0x20				// PIO (Parallel I/O) Intel FPGA for accessing the FPGA LEDs address offset
#define DEMO_LW_KEYS_OFFSET		0x10				// PIO (Parallel I/O) Intel FPGA for accessing the FPGA Keys address offset
#define DEMO_LW_SWICHES_OFFSET	0x00				// PIO (Parallel I/O) Intel FPGA for accessing the FPGA Swiches address offset
#define DEMO_EN_SEVENSIG		0					// Disable the Seven Sigment Display 
#define DEMO_LW_SEVENSIG_OFFSET 0					// Seven Sigment Display IP address offset 
#define DEMO_LW_ADC_OFFSET		0x40				// Analog Deives LTC2308 ADC IP address offset  
#elif DEMO_SELECTED_BOARD == DEMO_DEVBOARD_DE0NANOSOC
#define DEMO_LW_SYSTEMID_OFFSET 0x30				// System ID for Intel FPGA address offset
#define DEMO_LW_SYSTEMID		0xCAFEACDC			// Unique ID of the Intel FPGA address offset
#define DEMO_LW_LED_MAXVALUE	254					// Maximal countebil vaule on the LEDs
#define DEMO_LW_LED_OFFSET		0x20				// PIO (Parallel I/O) Intel FPGA for accessing the FPGA LEDs address offset
#define DEMO_LW_KEYS_OFFSET		0x10				// PIO (Parallel I/O) Intel FPGA for accessing the FPGA Keys address offset
#define DEMO_LW_SWICHES_OFFSET	0x00				// PIO (Parallel I/O) Intel FPGA for accessing the FPGA Swiches address offset
#define DEMO_EN_SEVENSIG		0					// Disable the Seven Sigment Display 
#define DEMO_LW_SEVENSIG_OFFSET 0					// Seven Sigment Display IP address offset 
#define DEMO_LW_ADC_OFFSET		0x40				// Analog Deives LTC2308 ADC IP address offset 
#endif 


// Path of the FPGA configuration file to write to the FPGA fabirc inside the Linux rootfs 
#if DEMO_SELECTED_BOARD == DEMO_DEVBOARD_DE10STD
#define DEMO_FPGACONF_PATH	"/home/root/blinky_std.rbf"
#elif  DEMO_SELECTED_BOARD == DEMO_DEVBOARD_DE10NANO
#define DEMO_FPGACONF_PATH	"/home/root/blinky_nano.rbf"
#elif  DEMO_SELECTED_BOARD == DEMO_DEVBOARD_DE0NANOSOC
#define DEMO_FPGACONF_PATH	"/home/root/blinky_de0.rbf"
#endif

// In rsyocto is orginal FPGA configuration witch was used by the secondary bootloader
//   located in following path: /usr/rsyocto/running_bootloader_fpgaconfig.rbf
#define DEMO_ORIGINAL_FPGACONF_PATH "/usr/rsyocto/running_bootloader_fpgaconfig.rbf"
