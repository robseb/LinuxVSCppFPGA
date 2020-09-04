#pragma once
// 
//  Robin Sebastian - 04-09-2020-  git@obseb.de
//  Driver for the Intel Universty program Analog Devices LTC2308 Interface ADC Soft-IP 
//

/////// Includes ///////

#include <fstream>					// POSIX: for acessing Linux drivers
#include <sys/mman.h>				// POSIX: memory maping
#include <fcntl.h>					// POSIX: "PROT_WRITE", "MAP_SHARED", ...
#include <unistd.h>					// POSIX: for closing the Linux driver access
#include <stdio.h>                  // uint16_t,....        

/////// Register Set ///////

// ADC data register no for channel (read only)
#define ADC_REG_NO_DATACH(nr) (nr) // ADC data output register address ofset for every channels

// ADC Control register no (write only)
#define ADC_REG_NO_UPDATE      0 // Update the converted values
#define ADC_REG_NO_AUTO_UPDATE 1 // Enables or disables auto-updating

// The number of available number 
#define ADC_REG_RANGE 28

/////// LTC2308 Interface ///////

class LTC2308 {
private:
    void* adcMap;               // void pointer to the assigned virtual memory space
    uint32_t* ptradcMap;        // Virtual memory space pointer converted to register pointer
    int fd;                     // Memory driver handler
    bool map_open = false;      // is the memory map open
    uint32_t offset;            // address offset of the ADC IP
    bool auto_mode;             // ADC convention mode enabled
public:
    /*
    *
    * @brief Constructor
    * @param register_base      Register base address (LWHPS2FPGA-, HPS2FPGA-Bus-offset address)
    * @param register_offset    Register offset address (Offset address of the ADC IP)
    * @param auto_mode          Enable the convention auto update mode (ADC will convert with SCLK)
    *
    */
    inline LTC2308(unsigned int register_base, unsigned int register_offset, bool auto_mode=true)
    {
        // Open the Linux memory driver with read and write privalages 
        this->fd = open("/dev/mem", (O_RDWR | O_SYNC));
        if (this->fd < 0) return;

        // Create a Memory Map to acess the entire ADC128S002 address space 
        this->adcMap = mmap(NULL, register_offset+ ADC_REG_RANGE, PROT_WRITE |\
        PROT_READ, MAP_SHARED, this->fd, register_base);
        if (this->adcMap == MAP_FAILED) return;

        this->ptradcMap = (uint32_t*)this->adcMap;
        this->offset = (register_offset)/4;

        this->map_open = true;

        this->autoUpdate(auto_mode);
    }

    /*
    *
    * @brief Deconstructor
    *
    */
    inline ~LTC2308()
    {
        // Close the memory map
        munmap(this->adcMap, ADC_REG_RANGE);

        // Close the driver port 
        close(this->fd);
    }

    /*
    *
    * @brief   Trigger the conversion of a new ADC value
    * @note    only allowed in the non convention auto update mode
    * @return  success
    *
    */
    inline bool update()
    {
        if ((!this->map_open) || (this->auto_mode)) return false;
  

        *(this->ptradcMap + this->offset+ (ADC_REG_NO_UPDATE)) =0;

        return true;
    }

    /*
    *
    * @brief             Enable the convention auto update mode (ADC will convert with SCLK)
    * @param auto_mode   Enable or Disable
    * @return            success
    *
    */
    inline bool autoUpdate(bool enable_disable = true)
    {
        if (!this->map_open) return false;

        *(this->ptradcMap + this->offset + (ADC_REG_NO_AUTO_UPDATE)) = \
            (enable_disable ? 0x1 : 0);

        this->auto_mode = auto_mode;
        return true;
    }

    /*
    *
    * @brief             Read the 12-bit ADC value of a ADC channel 
    * @param ch          channel number to read 
    * @return            12-bit ADC value as integer 
    *
    */
    inline uint16_t readChannel(uint8_t ch=0)
    {
        if ((!this->map_open) || (ch > 8)) return 0;
        uint32_t temp = ((uint32_t) * (this->ptradcMap + this->offset + (ADC_REG_NO_DATACH(ch))));       
        // Remove Bit 12-31 (Bit 15 is always high) 
        return ((uint16_t)(temp & 0x00000FFF));
    }

    /*
    *
    * @brief             Read the ADC value as voltage 
    * @param ch          channel number to read
    * @return            float value of the ADC input volatge
    */
    inline float readChannelU(uint8_t ch = 0)
    {
       float temp = this->readChannel(ch);
       return (float) ((temp* 5)/4095);
    }

};
