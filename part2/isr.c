/*
 * isr.c
 */

#include "isr.h"
#include "MK64F12.h"
#include <stdio.h>
#include "uart.h"

#define PPG_MAX (10000)

//variables global to the IRQ handlers which dictates if timer is enabled &  timer counter
int timerCount = 0;
int SW2_Pressed = 0;
int SW3_Pressed = 0;
int redON = 0;

//Lab 8 ADC
int PPG[PPG_MAX];
unsigned int PPG_CNT = 0;
unsigned int ppg_ready = 0;

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

	
	if(SW3_Pressed){
		if(redON){
			Off_LED();
			redON = 0;
		}
		else{
			Off_LED();
			Red_LED();
			redON = 1;
		}
	}
	
	return;
}

void calculateHeartBeat(){
	
	int line[PPG_MAX];
	int negCount = 0;
	int posCount = 0;
	int heartRate = 0;
	
	for(int i = 1; i < PPG_MAX - 1; i ++){
		line[i] = PPG[i+1] - PPG[i-1];
		if(line[i] > 0){
			posCount++;
		}
		if(line[i] < 0){
			negCount++;
			posCount = 0;
		}
		if(negCount == 3){
			heartRate++;
		}
		if(posCount >= 3){
			negCount = 0;
			posCount = 0;
		}
	}
}
	
void FTM0_IRQHandler(void)
{ //For FTM timer
	FTM0_SC &= ~FTM_SC_TOF_MASK;
	
	//add a millisecond to timerCount
	timerCount++;
	PPG[PPG_CNT] = ADC1_RA >> 4;
	if(PPG_CNT >= PPG_MAX){
		PPG_CNT = 0;
	}
	PPG_CNT++;
	
	if(timerCount >= 10000){
		calculateHeartBeat();
	}
	
	return;
}
	
void PORTA_IRQHandler(void)
{ //For switch 3
	PORTA_PCR4 |= PORT_PCR_ISF_MASK;		//clear flag

	if(SW3_Pressed){
		SW3_Pressed = 0;
		Off_LED();
	}
	else{
		SW3_Pressed = 1;

	}
	
	return;
}
	
void PORTC_IRQHandler(void)
{ //For switch 2
	PORTC_PCR6 |= PORT_PCR_ISF_MASK;		//clear flag
	
	if(!(GPIOC_PDIR & ( 1 << 6))){
		SW2_Pressed = 1;
		FTM0_CNT = 0x00;
		timerCount = 0;
		Blue_LED();
	}
	else{
		SW2_Pressed = 0;
		Off_LED();
		put("Button held for ");
		putnumU(timerCount);
		put(" ms\n\r");
	}

	
	
	return;
}

// ADC1 Conversion Complete ISR
void ADC1_IRQHandler(void) {
    // Read the result (upper 12-bits). This also clears the Conversion complete flag.

}
