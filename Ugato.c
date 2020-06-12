#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
//#define F_CPU 9600000
#include <util/delay.h>
#include <inttypes.h>
#include <avr/sleep.h>

#ifndef BV
	#define BV(bit)			(1<<(bit))
#endif
#ifndef cbi
	#define cbi(reg,bit)	reg &= ~(BV(bit))
#endif
#ifndef sbi
	#define sbi(reg,bit)	reg |= (BV(bit))
#endif


#define PLAY_BIT 2
#define FWD_BIT  3
#define RDY_BIT  0

#define FWD_MAX  4
#define L_LEN 30
#define H_LEN 20
#define W_LEN 90
#define S_LEN 10

//egy impluzus kiadása adott biten
void pulse(uint8_t bit) {
	cbi(PORTB,bit);
	_delay_ms(L_LEN);
	sbi(PORTB,bit);
	_delay_ms(H_LEN);
	cbi(PORTB,bit);
	_delay_ms(L_LEN);		
	return;
}

//adott számú impulzus kiadása egy biten
void pulse_num(uint8_t bit,uint8_t num) {
	uint8_t i;
	for(i=0;i<num;i++) {
		pulse(bit);
	}
	return;
}


/*
PB0: RDY jelet figyeli (!LED)
PB1: csengõt figyeli (INT0)
PB2: PLAY jelet adja ki
PB3: FWD jelet adja ki
*/

int main (void) {
	//9.6Mhz az alapórajel
	//-----------------
	//interruptok beállítása
	//INT0 alacsonyszint-érzékeny
	MCUCR |=0b00000000;		
	//INT0 engedélyezése
	sbi(GIMSK,INT0);
	//sbi(GIMSK,PCIE);

	//-----------------
	//portok beállítása
	//ha ddr=1:output	
	//PORTB1 figyeli a csengõt (INT0) -> input
	//PORTB0 figyeli a RDY jelet -> input
	//       xx543210 
	DDRB = 0b00111100;
	//pull-up resistor
	PORTB |= 0b00000011;
	
	//-----------------
	//sleep mode = power down
	MCUCR |= 0b00010000;
	//sleep engedélyezése
	sbi(MCUCR,SE);
	//-----------------
	//interrupt engedélyezés
	sei();
	//main loop	
	
	//volatile uint8_t k=0;
	while(1) {
		//k++;		
		sleep_cpu();				
	}
	
	
		
	//-----------------
	//ide nem érhetünk soha
	cli();
	return 0;
}

//megnyomták a csengõt
ISR(INT0_vect) {
	cli();	
	volatile int r;
	r = rand()/(RAND_MAX/FWD_MAX+1)+1;
	//r = NextVal()/(RAND_MAX/FWD_MAX+1)+1;
	//r=3;
	//elõretekerés véletlen sokszor
	pulse_num(FWD_BIT,r);
	//véletlen szám lejátszása
	_delay_ms(W_LEN);
	pulse(PLAY_BIT);
	//figyelje a lejátszás végzett-e
	uint8_t rdymsk = 0b00000001;
	volatile uint8_t rdy;
	while(1) {
		_delay_ms(S_LEN);
		rdy = PINB;
		rdy &= rdymsk;
		if(rdy == rdymsk) {
			break;
		}

	}
	sei();	
}
