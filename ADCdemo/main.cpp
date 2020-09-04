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
//		    ----				Linux Soft-IP ADC Demo				       ----
//			----		for the Intel Cyclone V and the Intel hwlib		   ----
//			@Description:
//				Read the Soft-IP Analog Devices LTC2308 12-bit ADC with Linux
//			
//				+ Configure the HPS_LED as output
//				+ Configure the ADC interface 	
//				+ Read a ADC value, convert it to voltage and print it out
//				+ Toogle the HPS_LEY
//
//			@Supported platforms:
//					rsyocto Version 1.040 or later (https://github.com/robseb/rsyocto/)
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
#include "BoardConf.h"				// Contains board consts
#include "hwlib.h"					// Intel hwlib 
#include "socal/hps.h"				// Intel hwlib 
#include "alt_generalpurpose_io.h"  // Intel hwlib driver HPS GPIO

#include <iostream>					// For "cout"
#include <thread>					// Required for putting task to sleep 
#include <chrono>					// Required for putting task to sleep 
#include "ADCdriver.h"              // The driver for the Intel University LTC2308 ADC driver  

using namespace std;

// Number of durations for the demo loop 
#define DEMO_DURATIONS 500

/*
 * Main function
 */
int main(int argc, const char* argv[])
{
	///////// Initialisation the Virtual Memory for I/O accsess /////////
	__VIRTUALMEM_SPACE_INIT();

	cout << "\n\nLinux Soft-IP ADC Sensor Demo " << endl;

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

	cout << "Read "<< DEMO_DURATIONS<<" times the ADC Channel 0" << endl;
	
	/////////	 Initialisation of Analog Devices LTC2308 in auto update convesion mode	 /////////
	LTC2308 adc= LTC2308(ALT_LWFPGASLVS_OFST, DEMO_LW_ADC_OFFSET);

	//
	// Enter demo loop 
	//
	for(uint16_t i=0; i< DEMO_DURATIONS; i++)
	{
		static bool enable_led;

		// Read the ADC Value of Channel 0 and print it out as voltage value
		cout << "ADC0: " << adc.readChannelU(0) <<'V'<<endl;

		// C++11: Put this task for 100ms to sleep 
		std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::milliseconds(100));

		// Toogle the HPS LEDs
		enable_led != enable_led;
		alt_gpio_port_data_write(DEMO_LED_PORT, (1 << DEMO_LED_PIN),
			enable_led ? (1 << DEMO_LED_PIN) : 0);
	}

	// Turn the HPS_LED off agian
	alt_gpio_port_data_write(DEMO_LED_PORT, (1 << DEMO_LED_PIN),0);

	cout << "End of Application\n" << endl;

	return 1;
}