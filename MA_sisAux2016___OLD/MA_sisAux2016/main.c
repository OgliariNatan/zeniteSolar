/*
 * MA_sisAux2016.c
 *
 * Created: 27/07/2016 23:38:16
 * Author : Natan Ogliari
 * Equipe zênite solar
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/pgmspace.h>
//#include "avr/wdt.h" //biblioteca whatdog
#include "can.h"
#include <util/delay.h>

#define BOMBA1 PD6
#define BOMBA2 PB0
#define TensaoBat PD4

typedef union estados_t {
	struct {
		unsigned char Bomba1	 : 1;
		unsigned char Bomba2	 : 1;
		unsigned char Inter		 : 1;
		unsigned char tensaoBat	 : 1;
	};
	unsigned char todas;
} estados_t;

volatile estados_t estados;

const uint8_t can_filter[] PROGMEM;


int main(void)
{
	DDRD = (1 << BOMBA1) || (1 << BOMBA2) || (1 << TensaoBat);
	//int rId = 0;
	// Initialize MCP2515
	can_init(BITRATE_125_KBPS);
	
	//can_set_mode(NORMAL_MODE);//!<Modo de operação normal
	
	//can_disable_filter(0xff);
	
	// Load filters and masks
	can_static_filter(can_filter);
	
	
	//!<Cria mensagem a ser enviada
	can_t msg;
	
	msg.length = 5;
	msg.data[0] = 0xde;
	msg.data[1] = 0xad;
	msg.data[2] = 0xbe;
	msg.data[3] = 0xef;
	msg.data[4] = 0xef;
	
	msg.id = 0x0000;
	msg.flags.rtr = 0;
	//rId = mcp2515_read_id();
		
	while (1)
	{	
		can_send_message(&msg);
		can_t dado;		
		if (can_check_message()){//!<Verifica se uma nova mensagem foi recebida
		
			if (can_get_message(&dado));//!<Tenta ler a mensagem
		}
		//int n, a, t, o, g;
		
		
		
				
		if (1) {//!<inverte o estado da bomba1 de porão.
			PORTD^=(1<<BOMBA1);
		}
		
		if (1) {//!<inverte o estado da bomba2 de porão.
			PORTD^=(0<<BOMBA2);
		}
		
		
	_delay_ms(1000);
	}
	
	return 0;
}

const uint8_t can_filter[] PROGMEM = 
{
	// Group 0
	MCP2515_FILTER(0),				// Filter 0
/*	MCP2515_FILTER(0),				// Filter 1
	
	MCP2515_FILTER(0),				// Mask 0 (for group 0)
	*/
};