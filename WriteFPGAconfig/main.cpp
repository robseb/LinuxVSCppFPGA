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
//		    ----	Write the FPGA configuration with Linux Example Application ----
//
//			@Description:
//				Write the FPGA Configuration of a Intel Cyclone V SoC-FPGA via the FPGA Manager
//			    with the Intel hwlib (HPS periphery libary)  
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
#include "hwlib.h"					// Intel hwlib 
#include "alt_fpga_manager.h"		// Intel hwlib driver for the Cyclone V FPGA manger

#include <iostream>
#include <fstream>
#include <string.h>
#include "socal/hps.h"
using namespace std;

//
// Enable the diagnostic print mode of the Intel hwlib 
//
#define DEBUG_ALT_FPGA_MANAGER

//
// Demo configuration
//

#define DEMO_DEVBOARD_DE10NANO 1 // Terasic DE10 Nano
#define DEMO_DEVBOARD_DE10STD  2 // Terasic DE10 Standard
#define DEMO_DEVBOARD_UNKNOWN  0


#if DEMO_SELECTED_BOARD == DEMO_DEVBOARD_UNKNOWN
#error "Please select your development board!"
#endif 

// Path of the FPGA configuration file to write to the FPGA fabirc inside the Linux rootfs 
#if DEMO_SELECTED_BOARD == DEMO_DEVBOARD_DE10STD
#define DEMO_FPGACONF_PATH	"/home/root/blinky_std.rbf"
#elif  DEMO_SELECTED_BOARD == DEMO_DEVBOARD_DE10NANO
#define DEMO_FPGACONF_PATH	"/home/root/blinky_nano.rbf"
#endif

// In rsyocto is orginal FPGA configuration witch was used by the secondary bootloader
//   located in following path: /usr/rsyocto/running_bootloader_fpgaconfig.rbf
#define DEMO_ORIGINAL_FPGACONF_PATH "/usr/rsyocto/running_bootloader_fpgaconfig.rbf"

#pragma region File access- and convert functions

bool write_FpgaConf(const char* configFileAdress);


/*! \brief Check that the FPGA configuration files exist
 *  \param fileName the name of the file to check 
 */
bool is_file_exist(const char* fileName)
{
	ifstream infile(fileName);
	return infile.good();
}

/*! \brief Convert the MSEL selection code to a ASCI II string message 
 *  \param cfg MSEL selection code
 */
string cfg2str(ALT_FPGA_CFG_MODE_t cfg)
{
	switch (cfg)
	{
	case ALT_FPGA_CFG_MODE_PP16_FAST_NOAES_NODC:
		return "ALT FPGA CFG MODE: PP16 -  FAST - NOAES - NODC";
	case	ALT_FPGA_CFG_MODE_PP16_FAST_AES_NODC:
		return "ALT FPGA CFG MODE: PP16 -  FAST - AES - NODC";
	case ALT_FPGA_CFG_MODE_PP16_FAST_AESOPT_DC:
		return "ALT FPGA CFG MODE: PP16 -  FAST - AESOP - DC";
	case ALT_FPGA_CFG_MODE_PP16_SLOW_NOAES_NODC:
		return "ALT FPGA CFG MODE: PP16 -  SLOW - NOAES - NODC";
	case	ALT_FPGA_CFG_MODE_PP16_SLOW_AES_NODC:
		return "ALT FPGA CFG MODE: PP16 -  SLOW - AES - NODC";
	case	ALT_FPGA_CFG_MODE_PP16_SLOW_AESOPT_DC:
		return "ALT FPGA CFG MODE: PP16 -  SLOW - AESOPT - DC";
	case	ALT_FPGA_CFG_MODE_PP32_FAST_NOAES_NODC:
		return "ALT FPGA CFG MODE: PP32 -  FAST - NOAES - NODC";
	case	ALT_FPGA_CFG_MODE_PP32_FAST_AES_NODC:
		return "ALT FPGA CFG MODE: PP32 -  FAST - AES - NODC";
	case	ALT_FPGA_CFG_MODE_PP32_FAST_AESOPT_DC:
		return "ALT FPGA CFG MODE: PP32 -  FAST - AESOPT - DC";
	case	ALT_FPGA_CFG_MODE_PP32_SLOW_NOAES_NODC:
		return "ALT FPGA CFG MODE: PP32 -  SLOW - NOAES - NODC";
	case	ALT_FPGA_CFG_MODE_PP32_SLOW_AES_NODC:
		return "ALT FPGA CFG MODE: PP32 -  SLOW - AES - NODC";
	case	ALT_FPGA_CFG_MODE_PP32_SLOW_AESOPT_DC:
		return "ALT FPGA CFG MODE: PP32 -  SLOW - AESOPT - DC";
	default:
		return "ALT FPGA CFG MODE: unknown";
	}
}

/*! \brief Convert the FPGA fabric Status code to a ASCI II string message
 *  \param cfg FPGA fabric Status 
 */
string std2str(ALT_FPGA_STATE_t std)
{
	switch (std)
	{
	case ALT_FPGA_STATE_POWER_UP:
		return "FPGA State: Power Up";
	case ALT_FPGA_STATE_RESET:
		return "FPGA State: Reset";
	case ALT_FPGA_STATE_CFG:
		return "FPGA State: Configuration Phase";
	case ALT_FPGA_STATE_INIT:
		return "FPGA State: Initialization Phase";
	case ALT_FPGA_STATE_USER_MODE:
		return "FPGA State: FPGA in User Mode";
	case ALT_FPGA_STATE_UNKNOWN:
		return "FPGA State: unknown";
	default:
		return "FPGA State: unknown";
	}
}
#pragma endregion


/*
 * Main function
 */
int main(int argc, const char* argv[])
{
	///////// Initialisation the Virtual Memory for I/O accsess /////////
	__VIRTUALMEM_SPACE_INIT();

	cout << "\n\nHPS - FPGA Manager Demo to write the FPGA fabric " << endl;

	/////////	 Initialisation of the FPGA Manager	 /////////
	alt_fpga_init();

	// Take the right to controll the FPGA
	alt_fpga_control_enable();

	/////////  Read the MSEL-switch postion			/////////
	ALT_FPGA_CFG_MODE_t msel_pos = alt_fpga_cfg_mode_get();
	cout <<"+ Reading the MSEL-switch postion" << endl<< cfg2str(msel_pos) << endl;

	///////// Check that the MSEL switch of the development board is in a compatible position for the FPGA configuration file  /////////
	if (!(DEMO_TYPE_OF_FPGACONF(msel_pos)))
	{
		cout << "\nERROR: The MSEL Switch is in the wrong postion!" << endl;
		cout << "It is not posible to write the FPGA configuration" << endl;
		alt_fpga_control_disable();
		return -1;
	}

	///////// Read the status of the FPGA		/////////
	ALT_FPGA_STATE_t fabric_status = alt_fpga_state_get();
	cout <<"+ Reading the status of the FPGA" <<endl<< std2str(fabric_status) << endl;


	///////// Check that the MSEL switch of the development board is in a compatible position for the FPGA configuration file  /////////
	if (!(msel_pos == ALT_FPGA_CFG_MODE_PP16_SLOW_AES_NODC) && 
		(msel_pos == ALT_FPGA_CFG_MODE_PP16_SLOW_NOAES_NODC) && (msel_pos == ALT_FPGA_CFG_MODE_PP16_FAST_NOAES_NODC))
	{
		cout << "\nERROR: The MSEL Switch is in the wrong postion!" << endl;
		cout << "It is not posible to write the FPGA configuration" << endl;
		alt_fpga_control_disable();
		return -1;
	}

	///////// Write a new configuration to the FPGA   /////////
	bool configState = write_FpgaConf(DEMO_FPGACONF_PATH);

	// Check that the FPGA writing was succsesfull 
	if (!configState)
	{
		cout << "\nERROR: Writing of the FPGA configuration failed" << endl;
		alt_fpga_control_disable();
		return -1;
	}

	///////// Check that the FPGA Fabric is back in USER Mode ///////// 
	fabric_status = alt_fpga_state_get();
	cout << "+ Reading the status of the FPGA" << endl << std2str(fabric_status) << endl;

	if (!(fabric_status== ALT_FPGA_STATE_USER_MODE))
	{
		cout  << "\nERROR: Something went wrong.." << endl;
		cout << "       After Configuration change is the fabric not in user mode" << endl;
		alt_fpga_control_disable();
		return -1;
	}

	// Wait for any key input
	do
	{
		cout << "Press any key re-write the FPGA with the orignal Configuration ..." << endl;
	} while (cin.get() != '\n');


	///////// Write the orginal FPGA configuration back  /////////
	write_FpgaConf(DEMO_ORIGINAL_FPGACONF_PATH);


	// Give the right to controll the FPGA
	alt_fpga_control_disable();

	cout << "End of Application\n" << endl;

	return 1;
}


/*! \brief Chnage the FPGA configuration
 *  \param configFileAdress Path of the rootfs FPGA configuration file to write 
 *  \return was the change successful 
 */
bool write_FpgaConf(const char* configFileAdress)
{
	// Check that the selected file exist 
	if (is_file_exist(configFileAdress))
	{
		cout << "Start writing to the FPGA\n" << endl;

		// Open the ".rbf"-onfiguration file
		// and load it to an binary buffer to the Memory
		FILE* f = fopen(configFileAdress, "rb");
		fseek(f, 0, SEEK_END);
		long fsize = ftell(f);
		fseek(f, 0, SEEK_SET);

		// Load the file to a local buffer value
		char* buf = new char[fsize + 1];
		fread(buf, 1, fsize, f);
		fclose(f);

		// Start to write the FPGA Configuration
		ALT_STATUS_CODE status = alt_fpga_configure(buf, fsize);

		// Check thet the writing was successful 
		if (status != ALT_E_SUCCESS)
		{
			cout << "\nERROR: Writing the FPGA failed" << endl;
			return false;
		}
		else
			cout << "\n--> The FPGA runs now with the new configuration" << endl;
	}
	else
	{
		cout << "\nERROR: The config file does not exsist!" << endl;
		cout << "Please insired a FPGA RAW (16-Bit Parallel) .rbf configuration file " << endl;
		cout << "with the name \"socfpga.rbf\" to the home directory." << endl;
		return false;
	}
	return true;
}