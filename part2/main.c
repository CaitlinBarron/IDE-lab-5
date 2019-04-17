/*
* Rochester Institute of Technology
* Department of Computer Engineering
* CMPE 460  Interfacing Digital Electronics
* Spring 2016
*
* Filename: main_timer_template.c
*/

#include "MK64F12.h"
#include "uart.h"
#include "isr.h"
#include <stdio.h>

/*From clock setup 0 in system_MK64f12.c*/
#define DEFAULT_SYSTEM_CLOCK 20485760u /* Default System clock value */

void initPDB(void);
void initGPIO(void);
void initFTM(void);
void initInterrupts(void);

int part1_main(void)
{
	//initializations
	uart_init();
	initPDB();
	initFTM();
	initGPIO();
	initInterrupts();

	for(;;)
	{
		//To infinity and beyond
	}
}

void initPDB(void){
	//Enable clock for PDB module
	SIM_SCGC6 |= SIM_SCGC6_PDB_MASK;
	

	// Set continuous mode, prescaler of 128, multiplication factor of 20,
	// software triggering, and PDB enabled
	PDB0_SC |= PDB_SC_CONT_MASK; // Continuous mode
	PDB0_SC |= PDB_SC_PRESCALER_MASK; //Prescaler of 128
	PDB0_SC |= (PDB_SC_MULT_MASK & (2 << PDB_SC_MULT_SHIFT));//Multiplication factor of 20
	PDB0_SC |= PDB_SC_TRGSEL_MASK;//Software trigger
	PDB0_SC |= PDB_SC_PDBEN_MASK;//PDB enable
	



	//Set the mod field to get a 1 second period.
	//There is a division by 2 to make the LED blinking period 1 second.
	//This translates to two mod counts in one second (one for on, one for off)
	
	//PDB0_MOD |= (PDB_MOD_MOD_MASK & 0x00005B8D); //23437 -- calculated
	
	PDB0_MOD = DEFAULT_SYSTEM_CLOCK/ (128*20); // 50,000,000 / 50,000 = 1000 //given

	//Configure the Interrupt Delay register.
	PDB0_IDLY = 10;

	//Enable the interrupt mask.
	PDB0_SC |= PDB_SC_PDBIE_MASK;//enable interrupt

	//Enable LDOK to have PDB0_SC register changes loaded.
	PDB0_SC |= PDB_SC_LDOK_MASK;//load register changes

	PDB0_SC |= PDB_SC_SWTRIG_MASK;


	return;
}

void initFTM(void){
	//Enable clock for FTM module (use FTM0)
	SIM_SCGC6 |= SIM_SCGC6_FTM0_MASK;

	//turn off FTM Mode to  write protection;
	FTM0_MODE |= FTM_MODE_WPDIS_MASK;

	//divide the input clock down by 128,
	FTM0_SC |= (FTM_SC_PS_MASK & 7);

	//reset the counter to zero
	FTM0_CNT = 0;


	//Set the overflow rate
	//(Sysclock/128)- clock after prescaler
	//(Sysclock/128)/1000- slow down by a factor of 1000 to go from
	//Mhz to Khz, then 1/KHz = msec
	//Every 1msec, the FTM counter will set the overflow flag (TOF) and
	FTM0->MOD = (DEFAULT_SYSTEM_CLOCK/(1<<7))/1000;

	//Select the System Clock
	FTM0_SC |=( 1 << FTM_SC_CLKS_SHIFT);

	//Enable the interrupt mask. Timer overflow Interrupt enable
	FTM0_SC |= FTM_SC_TOIE_MASK;

	return;
}

void initGPIO(void){
	//initialize push buttons and LEDs
	LED_init();
	SW3_INIT();
	SW2_INIT();


	//initialize clocks for each different port used.


	//Configure Port Control Register for Inputs with pull enable and pull up resistor

	// Configure mux for Outputs


	// Switch the GPIO pins to output mode (Red and Blue LEDs)


	// Turn off the LEDs

	// Set the push buttons as an input


	// interrupt configuration for SW3(Rising Edge) and SW2 (Either)
	

	return;
}

