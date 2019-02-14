/*
Title:				IDE Lab 4 part 2
Description:	controls LED's using input from bluetooth board
Authors: 			Caitlin Barron
Date: 				2/6/2019

Some of the contents are obtained by the courtesy of the material provided by Freescale Semiconductor, Inc.
Copyright (c) 2014, Freescale Semiconductor, Inc.
All rights reserved.
*/

#include <stdio.h>
#include "MK64F12.h"
#define BAUD_RATE 9600      //default baud rate 
#define SYS_CLOCK 20485760 //default system clock (see DEFAULT_SYSTEM_CLOCK  in system_MK64F12.c)

int khkjbg_main()
{
	//enable PDB0 clock
	PDB0_SC |= PDB_SC_PDBEN_MASK;
	
	//Enable the PDB0 IRQ, using NVIC_EnableIRQ macro
	NVIC_EnableIRQ(PDB0_IRQn);
	
	//Enable the clock for the FTM0 module
	FTM_SC_CLKS(0x01);
	
	//Enable the FTM0 IRQ, use NVIC_EnableIRQ macro
	NVIC_EnableIRQ(FTM0_IRQn);
	
	//Configure SW3 to be interrupted on the rising edge
	SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;		//Enables Port A clock
	PORTA_PCR4 = PORT_PCR_MUX(1);				//PTA4
	GPIOA_PDDR |= (0 << 4);
	PORTA_PCR4 = PORT_PCR_IRQC(0x9);		//rising edge
	PORTA_ISFR = PORT_ISFR_ISF(0x10);		//clear flag
	NVIC_EnableIRQ(PORTA_IRQn);
	
	
	//Configure SW2 to be interrupted on both rising and falling edge
	SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;		//Enables Port C clock
	PORTC_PCR6 = PORT_PCR_MUX(1);				//PTC6
	GPIOC_PDDR |= (0 << 6);
	PORTC_PCR6 = PORT_PCR_IRQC(0xB);		//rising and falling edge
	PORTC_ISFR = PORT_ISFR_ISF(0x40);		//clear flag
	NVIC_EnableIRQ(PORTC_IRQn);
	
	
	//Enable the clock for ADC1 module
	SIM_SCGC3 |= SIM_SCGC3_ADC1_MASK;
	
	//Enable the ADC1 IRQ, use NVIC_EnableIRQ macro
	NVIC_EnableIRQ(ADC1_IRQn);
	
	//Read result from ADC1
	unsigned short i = ADC1_RA >> 4; 
	
	//Set DAC output value (12 bits) to variable i above
    DAC_DATL_DATA0(i);
		DAC_DATH_DATA1(i);
}
