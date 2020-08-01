
#include "LCDM.h"

#include <iostream>					// For "cout"
#include "hwlib.h"					// Intel hwlib 
#include "socal/hps.h"				// Intel hwlib 
#include "alt_generalpurpose_io.h"  // Intel hwlib driver HPS GPIO 

#include <thread>					// Required for putting task to sleep 
#include <chrono>					// Required for putting task to sleep 

#include <fstream>					// POSIX: for acessing Linux drivers
#include <sys/mman.h>				// POSIX: memory maping
#include <fcntl.h>					// POSIX: "PROT_WRITE", "MAP_SHARED", ...
#include <unistd.h>					// POSIX: for closing the Linux driver access

#include <linux/spi/spidev.h>
#include <sys/ioctl.h>

using namespace std;

spi_ioc_transfer spim_xfer;
volatile int fd_spim;

// 
// Terasic DE10 Standard 128x64 SPI LCD Module (LCDM)
// (in direction of the SoC-FPGA)
//
//	Signal	Name	  |	 Type		|	HPS GPIO	|	hwlib Port	|	hwlib Pin	| Description 
//	--------------------------------------------------------------------------------------------------------------------
//	HPS_LCM_D_C		  |	 Output		|	HPS_GPIO41	|	  GPIOB		|	   12		| HPS LCM Data bit is Data/Command 
//	HPS_LCM_RST_N     |	 Output		|	HPS_GPIO44	|	  GPIOB		|	   15		| HPS LCM Reset 
//	HPS_LCM_BK        |	 Input		|	HPS_GPIO37	|	  GPIOB		|	    3		| HPS LCM Blacklight Display On/Off ????	
//	HPS_LCM_SPIM_CLK  |  SPIMO		|		-		|	  GPIOA		|      23		| SPI Clock
//	HPS_LCM_SPIM_MOSI |  SPIMO		|		-		|     GPIOC		|      22		| SPI Master Output and Slave Input
//  HPS_LCM_SPIM_SS   |  SPIMO		|		-		|     GPIOH		|	   20		| SPI Slave Select
//

#define HPS_LCM_D_C_MSK		(1<<12)
#define HPS_LCM_RST_N_MSK	(1<<15)
#define HPS_LCM_BK_MSK		(1<< 3)
#define DEMO_LED_PIN		(1<<24)

/*
 * \brief Initialisation of the 128x64 SPI LCD Module 
 */
bool LCDM_init(void)
{
	///////// Initialisation the Virtual Memory for I/O accsess /////////
	__VIRTUALMEM_SPACE_INIT();

	/////////	 Initialisation of the GPIO Pins of LCDM	 /////////
	alt_gpio_init();


	// 
	// Configure HPS_LCM_D_C, HPS_LCM_RST_N as output 
	//
	alt_gpio_port_config(ALT_GPIO_PORTB,						// GPIO Port
						HPS_LCM_D_C_MSK | DEMO_LED_PIN,	    // GPIO Pin
						ALT_GPIO_PIN_OUTPUT,		// GPIO direcory		 -> Output 
						ALT_GPIO_PIN_EDGE_TRIG_INT, // Interupt Trigger type -> Don't care of this demo 
						ALT_GPIO_PIN_ACTIVE_HIGH,	// GPIO Pin polarity	 -> High active 
						ALT_GPIO_PIN_NODEBOUNCE,    // Debounce the signal	 -> Off 
						0);			
	

	alt_gpio_port_config(ALT_GPIO_PORTB,						// GPIO Port
		HPS_LCM_BK_MSK,	    // GPIO Pin
		ALT_GPIO_PIN_OUTPUT,		// GPIO direcory		 -> Output 
		ALT_GPIO_PIN_EDGE_TRIG_INT, // Interupt Trigger type -> Don't care of this demo 
		ALT_GPIO_PIN_ACTIVE_HIGH,	// GPIO Pin polarity	 -> High active 
		ALT_GPIO_PIN_NODEBOUNCE,    // Debounce the signal	 -> Off 
		0);						    // Output value 		 -> Reset 	


	alt_gpio_port_config(ALT_GPIO_PORTB,						// GPIO Port
		HPS_LCM_RST_N_MSK,	    // GPIO Pin
		ALT_GPIO_PIN_OUTPUT,		// GPIO direcory		 -> Output 
		ALT_GPIO_PIN_EDGE_TRIG_INT, // Interupt Trigger type -> Don't care of this demo 
		ALT_GPIO_PIN_ACTIVE_HIGH,	// GPIO Pin polarity	 -> High active 
		ALT_GPIO_PIN_NODEBOUNCE,    // Debounce the signal	 -> Off 
		0);						    // Output value 		 -> Reset 	


	//
	// Reset the LCDM
	//
	//alt_gpio_port_data_write(ALT_GPIO_PORTB, HPS_LCM_RST_N_MSK, HPS_LCM_RST_N_MSK);

	alt_gpio_port_data_write(ALT_GPIO_PORTB, HPS_LCM_RST_N_MSK, 0);
	//alt_gpio_port_data_write(ALT_GPIO_PORTB, HPS_LCM_D_C_MSK, 0);
	//alt_gpio_port_data_write(ALT_GPIO_PORTB, DEMO_LED_PIN, DEMO_LED_PIN);

	std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::microseconds(62500));

	alt_gpio_port_data_write(ALT_GPIO_PORTB, HPS_LCM_RST_N_MSK, HPS_LCM_RST_N_MSK);
	//alt_gpio_port_data_write(ALT_GPIO_PORTB, HPS_LCM_D_C_MSK, HPS_LCM_RST_N_MSK);
	//alt_gpio_port_data_write(ALT_GPIO_PORTB, DEMO_LED_PIN, 0);

	std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::microseconds(62500));
	
	
	//
	// Reset HPS_LCM_D_C
	//
	alt_gpio_port_data_write(ALT_GPIO_PORTB, HPS_LCM_D_C_MSK, 0);
	
	// alt_gpio_port_data_write(ALT_GPIO_PORTB, (1 << 24), 0);
	// alt_gpio_port_data_write(ALT_GPIO_PORTB, (1 << 24), (1 << 24));

	//
	// Init the SPI Master Devuce Driver 
	//

	// Open the Linux memory driver with read and write privalages 
	fd_spim = open("/dev/spidev0.0", (O_RDWR | O_SYNC));

	// Check that the opening was sucsessfull 
	if (fd_spim < 0)
	{
		cout << "\nERROR: Failed to read the SPI Master driver!" << endl;
		return false;
	}

	uint8_t    mode, lsb, bits;
	uint32_t speed = 2500000, max_speed;
	
	if (ioctl(fd_spim, SPI_IOC_RD_MODE, &mode) < 0)
	{
		return false;
	}
	if (ioctl(fd_spim, SPI_IOC_RD_LSB_FIRST, &lsb) < 0)
	{
		return false;
	}
	if (ioctl(fd_spim, SPI_IOC_RD_BITS_PER_WORD, &bits) < 0)
	{
		return false;
	}
	if (ioctl(fd_spim, SPI_IOC_RD_MAX_SPEED_HZ, &max_speed) < 0)
	{
		return false;
	}

	spim_xfer.len = 3; /* Length of  command to write*/
	spim_xfer.cs_change = 0; /* Keep CS activated */
	spim_xfer.delay_usecs = 0; //delay in us
	spim_xfer.speed_hz = (speed > max_speed) ? max_speed : speed;//2500000, //speed
	spim_xfer.bits_per_word = bits;//8, // bites per word 8                

    return true;
}

void SPIM_WriteTxData(uint8_t Data)
{
	int status;

	spim_xfer.tx_buf = (unsigned long)&Data;
	spim_xfer.len = 1; /* Length of  command to write*/
	status = ioctl(fd_spim, SPI_IOC_MESSAGE(1), &spim_xfer);
	if (status < 0)
	{
		return;
	}
}

void LCDHW_Write8(uint8_t bIsData, uint8_t Data) {

	static uint8_t bPreIsData = 0xFF;

	// set A0
	if (bPreIsData != bIsData) {
		// Note. cannot change D_C until all tx dara(or command) are sent. i.e. fifo is empty
		//  while(!SPIM_IsTxEmpty()); // wait if buffer is not empty

		if(bIsData)
			alt_gpio_port_data_write(ALT_GPIO_PORTB, HPS_LCM_D_C_MSK, HPS_LCM_D_C_MSK);
		else
			alt_gpio_port_data_write(ALT_GPIO_PORTB, HPS_LCM_D_C_MSK, 0);

		bPreIsData = bIsData;
	}
	else {
		// wait buffer is not full
		//  while(SPIM_IsTxFull()); // wait if buffer is full 
	}


	SPIM_WriteTxData(Data);
}


