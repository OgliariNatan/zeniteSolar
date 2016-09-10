/*
 * MI_sisAux2016.c
 *
 * Created: 05/08/2016 20:36:55
 * Author : cliente
 */ 

#include <avr/io.h>
#include <avr/pgmspace.h>
#include "can.h"
#include "avr/wdt.h"
//#include "util/delay.h"

// #define F_CPU 16000000UL
#define aciBomba1 PD3
#define aciBomba2 PD4
#define aciMPPT PD5
#define aux1 PD6
#define aux2 PD7

int main(void)
{
	
	can_init(BITRATE_125_KBPS);//define velocidade de comunicação
	can_static_filter(can_filter);//carrega os filtros 
	can_t msg;//mensagem a ser enviada
	
	
	
    /* Replace with your application code */
    DDRB = 0b00100000;//Configura o PB5 como saida e os demais como entrada
	DDRD = 0b00000000;// Todas a portas D como entrada 
	
	typedef union estados_t{
		struct{
			unsigned char estBomba1 : 1;
			unsigned char estBomba2 : 1;
			unsigned char estMPPT	: 1;
			unsigned char estAux1	: 1;
			unsigned char estAux2	: 1;	
			};
		unsigned char todas;
		}estados_t;
	
	volatile estados_t estados;
	
	
	wdt_enable(WDTO_4S);//habilita o watchdog 
	
	while (1) 
    {
		wdt_reset();//diz que esta tudo ok
		PORTB^=(1<<PB5);//inverte o pino PB5
		//_delay_ms(2000);
		wdt_reset();//diz que esta tudo ok		
    }
}

