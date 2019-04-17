/*
* Rochester Institute of Technology
* Department of Computer Engineering
* CMPE 460  Interfacing Digital Electronics
* Spring 2016
*
* Filename: main_A2D_template.c
*/
 
#include "uart.h"
#include "MK64F12.h"
#include "stdio.h"
#include "isr.h"
#define DEFAULT_SYSTEM_CLOCK 20485760u /* Default System clock value */


 
void PDB_INIT(void) {
    //Enable PDB Clock
    SIM_SCGC6 |= SIM_SCGC6_PDB_MASK;
    //PDB0_CNT = 0x0000;
    PDB0_MOD = 50000; // 50,000,000 / 50,000 = 1000

    PDB0_SC = PDB_SC_PDBEN_MASK | PDB_SC_CONT_MASK | PDB_SC_TRGSEL(0xf)
                                    | PDB_SC_LDOK_MASK;
    PDB0_CH1C1 = PDB_C1_EN(0x01) | PDB_C1_TOS(0x01);
}

void FTM_INIT(void){
//Enable clock for FTM module (use FTM0)
	SIM_SCGC6 |= SIM_SCGC6_FTM0_MASK;

	//turn off FTM Mode to  write protection;
	FTM0_MODE |= FTM_MODE_WPDIS_MASK;

	//divide the input clock down by 128,
	FTM0_SC |= (FTM_SC_PS_MASK & 7);

	//reset the counter to zero
	FTM0_CNT = 0;
	
	// Set the Counter Initial Value to 0
	FTM0_CNTIN = 0;


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

}

void SW3_INIT(){
	//Configure SW3 to be interrupted on the rising edge
	SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;		//Enables Port A clock
	PORTA_PCR4 = PORT_PCR_MUX(1);				//PTA4
	GPIOA_PDDR = (0 << 4);
	PORTA_PCR4 |= PORT_PCR_IRQC(0x9);		//rising edge
	PORTA_PCR4 |= PORT_PCR_ISF_MASK;		//clear flag
}

void SW2_INIT(){
	//Configure SW2 to be interrupted on both rising and falling edge
	SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;		//Enables Port C clock
	PORTC_PCR6 |= PORT_PCR_MUX(1);				//PTC6
	GPIOC_PDDR = (0 << 6);
	PORTC_PCR6 |= PORT_PCR_IRQC(0xB);		//rising and falling edge
	PORTC_PCR6 |= PORT_PCR_ISF_MASK;		//clear flag
}
 
void ADC1_INIT(void) {
    unsigned int calib;
 
    // Turn on ADC1 clock
		SIM_SCGC3 |= SIM_SCGC3_ADC1_MASK;
 
    // Configure CFG Registers
    // Configure ADC to divide 50 MHz down to 6.25 MHz AD Clock, 16-bit single ended
    ADC1_CFG1 |= ADC_CFG1_ADIV_MASK;
		ADC1_CFG1 |= (ADC_CFG1_MODE_MASK);
	
 
    // Do ADC Calibration for Singled Ended ADC. Do not touch.
    ADC1_SC3 = ADC_SC3_CAL_MASK;
    while ( (ADC1_SC3 & ADC_SC3_CAL_MASK) != 0 );
    calib = ADC1_CLP0;
    calib += ADC1_CLP1;
    calib += ADC1_CLP2;
    calib += ADC1_CLP3;
    calib += ADC1_CLP4;
    calib += ADC1_CLPS;
    calib = calib >> 1;
    calib |= 0x8000;
    ADC1_PG = calib;
 
    // Configure SC registers.
    // Select hardware trigger.
    ADC1_SC2 |= ADC_SC2_ADTRG_MASK;
		ADC1_SC3 |= ADC_SC3_ADCO_MASK;
 
 
    // Configure SC1A register.
    // Select ADC Channel and enable interrupts. Use ADC1 channel DADP3  in single ended mode.
    ADC1_SC1A &= ~(ADC_SC1_DIFF_MASK);//Single Ended
		ADC1_SC1A &= ~(ADC_SC1_ADCH_MASK); //Flip all the bits!
		//ADC1_SC1A |= (ADC_SC1_ADCH_MASK & 0x03); //DAP3
		
		ADC1_SC1A |= (ADC_SC1_AIEN_MASK);
}
 


void DAC0_INIT(void) {
    //enable DAC clock
    SIM_SCGC2 |= SIM_SCGC2_DAC0_MASK;
    DAC0_C0 = DAC_C0_DACEN_MASK | DAC_C0_DACRFS_MASK;
    DAC0_C1 = 0;
}
 void initInterrupts(void){
	/*Can find these in MK64F12.h*/
	// Enable NVIC for portA,portC, PDB0,FTM0
	NVIC_EnableIRQ(PORTA_IRQn);
	NVIC_EnableIRQ(PORTC_IRQn);
	NVIC_EnableIRQ(PDB0_IRQn);
	NVIC_EnableIRQ(FTM0_IRQn);

	return;
}
int main(void) {
   
    // Initialize modules
    uart_init();  
		FTM_INIT();	
    ADC1_INIT();
    PDB_INIT();
		initInterrupts();
 
    // Start the PDB (ADC Conversions)
    PDB0_SC |= PDB_SC_SWTRIG_MASK;
 
    while(1);
 
    return 0;
}


int photo_main(void) {
    int i; char str[100];
   
    // Initialize modules
    uart_init();       
    DAC0_INIT();
    ADC1_INIT();
    PDB_INIT();
 
    // Start the PDB (ADC Conversions)
    PDB0_SC |= PDB_SC_SWTRIG_MASK;
 
    for(;;) 
		{
			//photo sensor
			sprintf(str,"\n Decimal: %d Hexadecimal: %x \n\r",ADC1_RA,ADC1_RA);
			put(str);
			for( i=0; i < 5000000; ++i )
			{
												 
			}
    }
 
    return 0;
}

