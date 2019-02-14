/*
 * isr.c
 */

#include "isr.h"
#include "MK64F12.h"
#include <stdio.h>
#include "uart.h"

//variables global to the IRQ handlers which dictates if timer is enabled &  timer counter
int timerCount = 0;
int SW2_Pressed = 0;


void Blue_LED()
{
	GPIOB_PCOR = (1 << 21);					//Set PTB21 LED to on
	GPIOB_PDOR = (1 << 22);					//Set PTB22 LED to off
	GPIOE_PDOR = (1 << 26);					//Set PTE26 LED to off
}

void Red_LED()
{
	GPIOB_PCOR = (1 << 22);					//Set PTB22 LED to on
	GPIOB_PDOR = (1 << 21);					//Set PTB21 LED to off
	GPIOE_PDOR = (1 << 26);					//Set PTE26 LED to off
}

void Off_LED()
{
	GPIOB_PSOR = (1UL << 21) | (1UL << 22);
	GPIOE_PSOR = 1UL << 26;
}


void PDB0_IRQHandler(void)
{ //For PDB timer
	
	//Clear interupt in PDB0_SC
	PDB0_SC &= ~PDB_SC_PDBIE_MASK;
	//Enable LED
	Red_LED();
	return;
}
	
void FTM0_IRQHandler(void)
{ //For FTM timer
	FTM0_SC &= ~FTM_SC_TOF_MASK;
	
	
	if(GPIOC_PDIR & ( 1 << 6)){
		timerCount++;
	}
	
	return;
}
	
void PORTA_IRQHandler(void)
{ //For switch 3
	PORTA_ISFR = PORT_ISFR_ISF(0x10);		//clear flag

	if(FTM0_MODE & FTM_MODE_FTMEN_MASK){
		FTM0_MODE &= ~FTM_MODE_FTMEN_MASK;
	}
	else{
		FTM0_MODE |= FTM_MODE_FTMEN_MASK;

	}
	
	return;
}
	
void PORTC_IRQHandler(void)
{ //For switch 2
	PORTC_ISFR = PORT_ISFR_ISF(0x40);		//clear flag
	
	if(GPIOC_PDIR & ( 1 << 6)){
		SW2_Pressed = 1;
		FTM0_CNT = 0x00;
		timerCount = 0;
		Blue_LED();
	}
	else{
		SW2_Pressed = 0;
		Off_LED();
		put("Button held for xx ms");
	}

	
	
	return;
}
