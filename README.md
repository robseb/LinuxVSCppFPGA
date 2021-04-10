
![GitHub](https://img.shields.io/static/v1?label=Microsoft+Visual+Studio&message=2019&color=yellowgreen)
![GitHub](https://img.shields.io/github/license/robseb/socfpgaPlatformGenerator)
# Microsoft Visual Studio 2019 for C++ Linux Development demo applications to get started with embedded C++ development for Intel SoC-FPGAs

**These three C++ Demo projects show how to interact with FPGA Soft-IP- and Linux Devices on an embedded Linux. Also, it is demonstrated how to use the *Intel hwlib* to change for instance the FPGA configuration.**   
I designed it to give users of my embedded Linux ["*rsyocto*"](https://github.com/robseb/rsyocto) a starting point in C++ development. 
<br>
___

# Demo Projects 

* **GetStarted**
    * **Control the HPS (*Hard processor system*) LED and Push button** of a *Terasic* DE10 development board  with the *Intel hwlib* (*HPS peripheral library*) 
        * **Using the General-Purpose I/O (GPIO) interface modules of Intel SoC-FPGAs with Linux**
        * Configure the *HPS_LEY* as output
        * Configure the *HPS_KEY* as input 	
        * Read the *HPS_KEY* and write the value to the *HPS_LED* and console  
* **HPSbridgeDemo**
    * **Access the HPS to FPGA Lightweight (*LW2FPGA; lwhps2fpga*) Bridge to interact with FPGA Soft-IP**
        * Access the Bridge Memory interface
        * Read the **UNIQUE ID** over the **LW2FPGA Bridge** 	
        * Count the **FPGA LEDs** and **FPGA 7Segment Display Soft-IP** up
        * Read the **FPGA Push Buttons** and **FPGA Switches**
* **WriteFPGAconfig**     
    * **Write the FPGA Configuration of an Intel Cyclone V SoC-FPGA** via the **FPGA Manager** and the *Intel hwlib* (*HPS peripheral library*)
        * **Read the *MSEL* switch position** of the development board  
        * **Read the status of the FPGA fabric**		
        * **Change the FPGA configuration** 
        * Change the FPGA configuration back to the original version
* **ADCdemo**     
    * **Read the Channel 0 of the *Analog Device LTC2308* 12-bit ADC connected via a Soft-IP to the *Lightweight HPS-to-FPGA Bridge***
        * **Configure the HPS_LED as output**
        * **Configure the ADC interface**
        * **Read an ADC value, convert it to voltage and print it out** 
<br>



# Get Started
* Install *Microsoft Visual Studio for Linux development* 
    * Please consider my following [**guide**](https://github.com/robseb/rsyocto/blob/rsYocto-1.04/doc/guides/3_CPP.md#installing-microsoft-visual-studio-for-linux-development)
* Clone this repository 
* Open the "*rsYoctoExamples.sln*" Microsoft Visual Studio Solution file to start Visual Studio
* Connect your Intel SoC-FPGA development board to the network and boot *rsyocto* ([for more information](https://github.com/robseb/rsyocto/blob/rsYocto-1.04/doc/guides/1_Booting.md))
* Connect the Visual Studio remote debugger (*gdb-server*) to the development board
    * by selecting inside Visual Studio `Tools/Options/Cross Platform` the "Remove"-Button and then "Add"-Button to insert the credentials of your board
        * Platform: `ARM`
        * Host Name: `IPv4-Address of your Board`
        * Port: `22`
        * User name: `root`
        * Authentication type: `Password`
        * Password: `eit` 
* Choose the project to debug by right clicking of the project name inside the *Solution Explorer* and selecting *Set as StartUp project* as shown here:
    <br>

    ![Alt text](startup_project_selection.png?raw=true "Visual Studio StartUp project selection")
    <br>

* **Select your FPGA board for the Demo**
    * The pre-installed demo contains a simple stop watch written as FreeRTOS task
    * Choice your FPGA development board for this demo due to specifying "*DEMO_SELECTED_BOARD*" in "*BoardConf.h*"
        ````c
        #define DEMO_DEVBOARD_DE10NANO    1 // Terasic DE10 Nano
        #define DEMO_DEVBOARD_DE10STD     2 // Terasic DE10 Standard
        #define DEMO_DEVBOARD_DE0NANOSOC  3 // Terasic DE0 Nano SoC
        #define DEMO_DEVBOARD_UNKNOWN     0
        
        // Select your Development Board
        #define DEMO_SELECTED_BOARD DEMO_DEVBOARD_DE10NANO
        
        #if DEMO_SELECTED_BOARD == DEMO_DEVBOARD_UNKNOWN
        #error "Please select your development board!"
        #endif 
        ````
* Press the debug button to start the debugging session 
<br>

___


# Author
* **Robin Sebastian**

rsyocto is a project, that I have fully developed on my own. No companies are involved in my projects. 
I’m recently graduated as a master in electrical engineering with the major embedded systems (M. Sc.).

**[Github sponsoring is welcome.](https://github.com/sponsors/robseb)**

[![Gitter](https://badges.gitter.im/rsyocto/community.svg)](https://gitter.im/rsyocto/community?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge)
[![Email me!](https://img.shields.io/badge/Ask%20me-anything-1abc9c.svg)](mailto:git@robseb.de)

[![GitHub stars](https://img.shields.io/github/stars/robseb/rsyocto?style=social)](https://GitHub.com/robseb/rsyocto/stargazers/)
[![GitHub watchers](https://img.shields.io/github/watchers/robseb/rsyocto?style=social)](https://github.com/robseb/rsyocto/watchers)
[![GitHub followers](https://img.shields.io/github/followers/robseb?style=social)](https://github.com/robseb)


