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
//		    ----					   HPS2FPGA Bdrige Demo				   ----
//			----				for the Intel Cyclone V SoC-FPGA		   ----
//			@Description:
//				Acess the HPS to FPGA Leight wight (LW2FPGA) Bridge 
//							to interact with FPGA Soft-IP
//			
//				+ Acess the Bridge Memory interface
//				+ Read the UNIQUE ID over the LW2FPGA Bridge 	
//				+ Count the FPGA LEDs and FPGA 7Sigment Display Soft-IP Up
//				+ Read the FPGA Push Button and FPGA Swiches  
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
#include <iostream>					// For "cout"
#include <thread>					// Required for putting task to sleep 
#include <chrono>					// Required for putting task to sleep 

#include <fstream>					// POSIX: for acessing Linux drivers
#include <sys/mman.h>				// POSIX: memory maping
#include <fcntl.h>					// POSIX: "PROT_WRITE", "MAP_SHARED", ...
#include <unistd.h>					// POSIX: for closing the Linux driver access

#include "hps.h"					// hwlib: With system constants for the Cyclone V SoC-FPGA

using namespace std;

//
// Denination 
//
// Address space of the LW2FPGA Bridge for the Cyclone V famaly 
#define LWH2F_RANGE 0x07FFFF
// Address space of the HPS2FPGA Bridge for the Cyclone V famaly 
#define H2F_RANGE  0x07FFFF

//
// Demo configuration
//
#define DEMO_DEVBOARD_DE10NANO 1 // Terasic DE10 Nano
#define DEMO_DEVBOARD_DE10STD  2 // Terasic DE10 Standard
#define DEMO_DEVBOARD_UNKNOWN  0

// Select your Development Board
#define DEMO_SELECTED_BOARD DEMO_DEVBOARD_DE10STD

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
#endif 

// Number of durations for the demo loop 
#define DEMO_DURATIONS 100
/*
 * Main function
 */
int main(int argc, const char* argv[])
{
	cout << "\n\nHPS Getting Started Demo " << endl;

	///////////////// 1.Step: Acess the Bridge Memory interface /////////////////
	void* lwbridgeMap;
	cout << "1.	 Access the Light Wight HPS to FPGA Brige " << endl;

	// Open the Linux memory driver with read and write privalages 
	int fd = open("/dev/mem", (O_RDWR | O_SYNC));

	// Check that the opening was sucsessfull 
	if (fd < 0)
	{
		cout << "\nERROR: Failed to read the memory driver!" << endl;
		return -1;
	}

	// Create a Memory Map to acess the entire LW2FPGA Bridge address space 
	lwbridgeMap = mmap(NULL, LWH2F_RANGE, PROT_WRITE | PROT_READ, MAP_SHARED, fd,
		ALT_LWFPGASLVS_OFST);

	// Check that the opening was sucsessful
	if (lwbridgeMap == MAP_FAILED)
	{
		cout << "\nERROR: Failed to open the memory Map!" << endl;
		return -1;
	}
	// Allocate a pointer to the maped address space 
	uint32_t* ptrLwBridge = (uint32_t*)lwbridgeMap;

	/////////////////  2.Step: Read the UNIQUE ID over the LW2FPGA Bridge  /////////////////
	cout << "2.	 Read the Uniqe ID over the LW2FPGA Bridge" << endl;
	
	*(ptrLwBridge + (DEMO_LW_SEVENSIG_OFFSET/4)) = 0;

	uint32_t systemID = *(ptrLwBridge + (DEMO_LW_SYSTEMID_OFFSET / 4));

	cout << "The ID is: " <<hex<< systemID <<dec<< endl;

	// Check that the system ID is vailed
	if (systemID != DEMO_LW_SYSTEMID)
	{
		cout << "\nERROR: The readed System ID is not vailed!" << endl;
		return -1;
	}
	cout << "The readed System ID is vailed" << endl;

	/////////////////  3.Step: Count the LEDs and 7Sigment Display Up  /////////////////
	cout << "3.	Count the FPGA LEDs Up" << endl;

#if DEMO_EN_SEVENSIG == 1
	cout << "   and count the seven sigment Display Value up" << endl;
#endif

	//
	// Enter Demo Loop
	//
	for (uint16_t i = 0; i < DEMO_LW_LED_MAXVALUE; i++)
	{
#if DEMO_EN_SEVENSIG == 1
		// Write the counting value to the 7sigment Display IP
		* (ptrLwBridge + (DEMO_LW_SEVENSIG_OFFSET / 4)) =(uint32_t) i;
#endif 
		// Write the counting value to the LEDs 
		* (ptrLwBridge + (DEMO_LW_LED_OFFSET / 4)) = (uint32_t) i;

		cout << "  Duration: " << i << "/" << DEMO_LW_LED_MAXVALUE << endl;

		// C++11: Put this task for 10ms to sleep 
		std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::milliseconds(10));
	}

	/////////////////  4.Step: Read the Push Button and Swiches  /////////////////
	cout << "4.	Read the Push Buttons and Swiches" << endl;
	cout << "  Write the values to the LEDs" << endl;
	cout << "  0-7  Bit: swiches" << endl;
	cout << "  8-15 Bit: Push Buttons" << endl;

#if DEMO_EN_SEVENSIG == 1
	cout << "   and count the seven sigment Display Value up" << endl;
#endif

	//
	// Enter Demo Loop
	//
	for (uint16_t i = 0; i < DEMO_DURATIONS; i++)
	{
		// Read the FPGA Push buttons 
		uint16_t pushButtons = (uint16_t) *(ptrLwBridge + (DEMO_LW_KEYS_OFFSET / 4)) & 0xFF;

		// Read the FPGA Swiches 
		uint16_t swiches = (uint16_t) *(ptrLwBridge + (DEMO_LW_SWICHES_OFFSET / 4)) & 0xFF;

		// Print the values to the console 
		cout << "PB: " << pushButtons << " | SW: " << swiches << endl;

		// Compaine booth value togther for writing it to the LEDs/7sig Display
		uint16_t compainedValue = ((uint16_t)(swiches & 0xFF)) | ((uint16_t)(pushButtons << 8));

#if DEMO_EN_SEVENSIG == 1
			// Write the comained value to the 7sigment Display IP
			* (ptrLwBridge + (DEMO_LW_SEVENSIG_OFFSET / 4)) = compainedValue;
#endif 
		// Write the comained value to the LEDs 
		* (ptrLwBridge + (DEMO_LW_LED_OFFSET / 4)) = compainedValue;

		// C++11: Put this task for 100ms to sleep 
		std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::milliseconds(100));
	}


#if DEMO_EN_SEVENSIG == 1
	// Clear the 7sigment Display
	* (ptrLwBridge + (DEMO_LW_SEVENSIG_OFFSET / 4)) = 0;
#endif
	// Clear the LEDs 
	* (ptrLwBridge + (DEMO_LW_LED_OFFSET / 4)) = 0;

	// Close the Memory Map
	int res = munmap(lwbridgeMap,LWH2F_RANGE);

	// Check that the closing was succsesfull
	if (res < 0)
	{
		cout << "\nERROR: Closing of the memory map failed!" << endl;
	}

	// Close the driver port 
	close(fd);


	cout << "End of Application\n" << endl;

	return 1;
}