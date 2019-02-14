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

 
void PDB_INIT(void) {
    //Enable PDB Clock
    SIM_SCGC6 |= SIM_SCGC6_PDB_MASK;
    PDB0_CNT = 0x0000;
    PDB0_MOD = 50000; // 50,000,000 / 50,000 = 1000

    PDB0_SC = PDB_SC_PDBEN_MASK | PDB_SC_CONT_MASK | PDB_SC_TRGSEL(0xf)
                                    | PDB_SC_LDOK_MASK;
    PDB0_CH1C1 = PDB_C1_EN(0x01) | PDB_C1_TOS(0x01);
}

void FTM_INIT(void){
	//Enable the clock for the FTM0 module
	FTM_SC_CLKS(0x01);
	
	//Enable the FTM0 IRQ, use NVIC_EnableIRQ macro
	NVIC_EnableIRQ(FTM0_IRQn);
}

void SW3_INIT(){
	//Configure SW3 to be interrupted on the rising edge
	SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;		//Enables Port A clock
	PORTA_PCR4 = PORT_PCR_MUX(1);				//PTA4
	GPIOA_PDDR |= (0 << 4);
	PORTA_PCR4 = PORT_PCR_IRQC(0x9);		//rising edge
	PORTA_ISFR = PORT_ISFR_ISF(0x10);		//clear flag
	NVIC_EnableIRQ(PORTA_IRQn);
}

void SW2_INIT(){
	//Configure SW2 to be interrupted on both rising and falling edge
	SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;		//Enables Port C clock
	PORTC_PCR6 = PORT_PCR_MUX(1);				//PTC6
	GPIOC_PDDR |= (0 << 6);
	PORTC_PCR6 = PORT_PCR_IRQC(0xB);		//rising and falling edge
	PORTC_ISFR = PORT_ISFR_ISF(0x40);		//clear flag
	NVIC_EnableIRQ(PORTC_IRQn);
}
 
void ADC1_INIT(void) {
    unsigned int calib;
 
    // Turn on ADC1 clock
		SIM_SCGC3 |= SIM_SCGC3_ADC1_MASK;
 
    // Configure CFG Registers
    // Configure ADC to divide 50 MHz down to 6.25 MHz AD Clock, 16-bit single ended
    ADC1_CFG1 |= (0b11 << ADC_CFG1_ADIV_SHIFT);
		ADC1_CFG1 |= (0b11 << ADC_SC3_CFG1_MODE_SHIFT);
	
 
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
 
 
    // Configure SC1A register.
    // Select ADC Channel and enable interrupts. Use ADC1 channel DADP3  in single ended mode.
    ADC1_SCIA |= (ADC_SC1_ADCH_MASK & 0b00011); //DAP3
		ADC1_SCIA &= ~(ADC_SC1_DIFF_MASK);//Single Ended
 
 
    // Enable NVIC interrupt
		NVIC_EnableIRQ(ADC1_IRQn);
}
 
// ADC1 Conversion Complete ISR
void ADC1_IRQHandler(void) {
    // Read the result (upper 12-bits). This also clears the Conversion complete flag.
    unsigned short i = ADC1_RA >> 4;

    //Set DAC output value (12bit)
    DAC_DATL_DATA0(i);
		DAC_DATH_DATA1(i);
}

void DAC0_INIT(void) {
    //enable DAC clock
    SIM_SCGC2 |= SIM_SCGC2_DAC0_MASK;
    DAC0_C0 = DAC_C0_DACEN_MASK | DAC_C0_DACRFS_MASK;
    DAC0_C1 = 0;
}
 
int main(void) {
    int i; char str[100];
   
    // Initialize UART
    uart_init();
               
    DAC0_INIT();
    ADC1_INIT();
    PDB_INIT();
 
    // Start the PDB (ADC Conversions)
    PDB0_SC |= PDB_SC_SWTRIG_MASK;
 
    for(;;) {
		sprintf(str,"\n Decimal: %d Hexadecimal: %x \n\r",ADC1_RA,ADC1_RA);
		put(str);
		for( i=0; i < 5000000; ++i ){
                       
        }
    }
 
    return 0;
}
 
