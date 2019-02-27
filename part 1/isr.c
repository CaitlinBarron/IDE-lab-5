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
int SW3_Pressed = 0;
int redON = 0;

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
	PDB0_SC &= ~PDB_SC_PDBIF_MASK;

	//Check if the blinking is toggled or not
	if(SW3_Pressed){
		//Blinking is toggled, check if LED is currently on
		if(redON){
			//LED is on, turn it off
			Off_LED();
			redON = 0;
		}
		else{
			//LED is off, turn it on
			Off_LED();//for safety, should not be needed
			Red_LED();
			redON = 1;
		}
	}
	
	return;
}
	
void FTM0_IRQHandler(void)
{ //For FTM timer
	FTM0_CNT = 0;
	FTM0_SC &= ~FTM_SC_TOF_MASK;
	
	
	if(SW2_Pressed){
		timerCount++;
	}
	
	return;
}
	
void PORTA_IRQHandler(void)
{ //For switch 3
	PORTA_PCR4 |= PORT_PCR_ISF_MASK;		//clear flag

	if(SW3_Pressed){//Toggle blinking off
		SW3_Pressed = 0;//clear global signal flag
		Off_LED();//Turn all LEDs off
	}
	else{//Toggle blinking on
		SW3_Pressed = 1;//Set global signal flag

	}
	
	return;
}
	
void PORTC_IRQHandler(void)
{ //For switch 2
	PORTC_PCR6 |= PORT_PCR_ISF_MASK;		//clear flag
	
	//Button pressed/held
	if(!(GPIOC_PDIR & ( 1 << 6))){//checks if SW2 is pressed
		SW2_Pressed = 1;//set local variable
		FTM0_CNT = 0x00;//reset FTM counter
		timerCount = 0;//reset counter local variable
		Blue_LED();//turn on blue led
	}
	else{//Buttom released
		SW2_Pressed = 0;//clear local variable
		Off_LED();//turn off the led
		
		//print how long the button was held for
		put("Button held for ");
		putnumU(timerCount);
		put(" ms\n\r");
	}

	
	
	return;
}
