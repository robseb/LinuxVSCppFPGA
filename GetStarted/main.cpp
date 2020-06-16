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
//		    ----	Getting Started with Visual Studio remote development  ----
//			----		for the Intel Cyclone V and the Intel hwlib		   ----
//			@Description:
//				Controll the HPS (Hard processor system) LED and Push button of 
//				a Terasic DE10 development board  with the Intel hwlib (HPS periphery libary)  
//			
//				+ Configure the HPS_LEY as output
//				+ Configure the HPS_KEY as input 	
//				+ Read the HPS_KEY and write the value to the HPS_LED and console  
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
#include "hwlib.h"					// Intel hwlib 
#include "socal/hps.h"				// Intel hwlib 
#include "alt_generalpurpose_io.h"  // Intel hwlib driver HPS GPIO

#include <iostream>					// For "cout"
#include <thread>					// Required for putting task to sleep 
#include <chrono>					// Required for putting task to sleep 


using namespace std;

//
// Demo configuration
//

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

// Number of durations for the demo loop 
#define DEMO_DURATIONS 100
/*
 * Main function
 */
int main(int argc, const char* argv[])
{
	///////// Initialisation the Virtual Memory for I/O accsess /////////
	__VIRTUALMEM_SPACE_INIT();

	cout << "\n\nHPS Getting Started Demo " << endl;

	/////////	 Initialisation of the GPIO Controller	 /////////
	alt_gpio_init();


	// Configure the HPS_LED GPIO Pin
	alt_gpio_port_config(DEMO_LED_PORT,				// GPIO Port
						(1 << DEMO_LED_PIN),		// GPIO Pin
						ALT_GPIO_PIN_OUTPUT,		// GPIO direcory		 -> Output 
 						ALT_GPIO_PIN_EDGE_TRIG_INT, // Interupt Trigger type -> Don't care of this demo 
						ALT_GPIO_PIN_ACTIVE_HIGH,	// GPIO Pin polarity	 -> High active 
						ALT_GPIO_PIN_NODEBOUNCE,    // Debounce the signal	 -> Off 
						0);						    // Output value 		 -> Reset 	



	// Configure the HPS_KEY GPIO Pin  
	alt_gpio_port_config(DEMO_KEY_PORT,				// GPIO Port
						(1 << DEMO_KEY_PIN),		// GPIO Pin
						ALT_GPIO_PIN_INPUT,			// GPIO direcory	     -> Input 
						ALT_GPIO_PIN_EDGE_TRIG_INT, // Interupt Trigger type -> Don't care of this demo 
						ALT_GPIO_PIN_ACTIVE_HIGH,	// GPIO Pin polarity	 -> High active  
						ALT_GPIO_PIN_DEBOUNCE,      // Debounce the signal   -> On
						0);						    // Output value 		 -> Reset	


	cout << "Read "<< DEMO_DURATIONS<<" times the HPS_KEY and show it on the HPS_LED" << endl;
	
	//
	// Enter demo loop 
	//
	for(uint16_t i=0; i< DEMO_DURATIONS; i++)
	{
		// Read the Value of the HPS_KEY
		uint32_t hps_key = alt_gpio_port_data_read(DEMO_KEY_PORT, (1 << DEMO_KEY_PIN));
		
		// Convert and Invert the value as booloan
		hps_key = (uint32_t)(!((bool)hps_key & ~(1 << DEMO_LED_PIN)));

		// Write the HPS_KEY value to the HPS_LED
		alt_gpio_port_data_write(DEMO_LED_PORT, (1 << DEMO_LED_PIN), 
								hps_key ? (1 << DEMO_LED_PIN):0);

		// Print the HPS_KEY value in the console 
		cout <<"HPS_KEY: " << hps_key<< endl;

		// C++11: Put this task for 100ms to sleep 
		std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::milliseconds(100));
	}

	// Turn the HPS_LED off agian
	alt_gpio_port_data_write(DEMO_LED_PORT, (1 << DEMO_LED_PIN),0);

	cout << "End of Application\n" << endl;

	return 1;
}