/**
* @file main.c
* @brief MI_sisAux2016.c
*
* @author Natan Ogliari
*
* @date 7/08/2016
*/

#define F_CPU 16000000UL
#include "globalDefines.h"
#include <avr/pgmspace.h>
#include "ATmega328.h"
//#include "avr/wdt.h"
#include <string.h>
#include "can.h"

#define aciBomba1	PD3			//! Definição de pino de saida
#define aciBomba2	PD4			//! Definição de pino de saida
#define aciMPPT	    PD5			//! Definição de pino de saida
#define aux1	    PD6			//! Definição de pino de saida
#define aux2	    PD7			//! Definição de pino de saida


typedef union estados_t{	//!cria uma estrutura para ser enviado no pacote.
		struct {
			unsigned char estBomba1 : 1;
			unsigned char estBomba2 : 1;
			unsigned char estMPPT   : 1;
			unsigned char estAux1   : 1;
			unsigned char estAux2   : 1;
		};
		unsigned char todas;
	} estados_t;

volatile estados_t estados;		//<!Define estrutura como dado Volatile.


const	uint8_t can_filter[] PROGMEM;		//!< Declara a função de filtro do canBus

/// @Brief Função principal.
/// here...
int main(void)
{
	_delay_ms(100);
	
	DDRD &= (0 << aciBomba1) & (0 << aciBomba2) & (0 << aciMPPT) & (0 << aux1) & (0 << aux2);
	PORTD |= (1 << aciBomba1) & (1 << aciBomba2) & (1 << aciMPPT) & (1 << aux1) & (1 << aux2);
 	//wdt_enable(WDTO_2S); //!<habilita watchdog em 2s
	estados.todas = 0;
	// Initialize MCP2515
	can_init(BITRATE_125_KBPS);		//!<Define a velocidade de comunicação.
	
	// Load filters and masks
	can_static_filter(can_filter);
	
	can_t msg;		//!Cria o pacote a ser enviado.
	
	msg.id = 0x123;
	//msg.flags.rtr = 0;
	
	msg.length = 5;			//!Define o tamanho do pacote a ser enviado.
	
	
	while (1)
	{
		//wdt_reset(); //!<diz que esta ok para watchdog
		
		if ((PIND >> aciBomba1) & 1)//!<Realiza a leitura Digital na porta PD3
		{
			estados.estBomba1 = !estados.estBomba1;
		}
			
		if ((PIND >> aciBomba2) & 1)//!<Realiza a leitura Digital na porta PD4
		{
			estados.estBomba2 = !estados.estBomba2;
		}
		
		if ((PIND >> aciMPPT) & 1)//!<Realiza a leitura Digital na porta PD5
		{
			estados.estMPPT = !estados.estMPPT;
		}
		
		if ((PIND >> aux1) & 1)//!<Realiza a leitura Digital na porta PD6
		{
			estados.estAux1 = !estados.estAux1;
		}
		
		if ((PIND >> aux2) & 1)//!Realiza a leitura Digital na porta PD7
		{
			estados.estAux2 = !estados.estAux2;
		}
		//!<Empacota os dados a serem enviados.
		msg.data[0] = estados.estBomba1;
		msg.data[1] = estados.estBomba2;
		msg.data[2] =   estados.estMPPT;
		msg.data[3] =   estados.estAux1;
		msg.data[4] =   estados.estAux2; 
		 
	   can_send_message(&msg);		//!Envia um o pacote
		 
		//!Check if a new messag was received
		if (can_check_message())
		{
			can_t dado;
			
			//!Try to read the message
			if (can_get_message(&dado))
			{
				
				//!Send the new message
				can_send_message(&dado);
			}
		 }
		
		
		//wdt_reset(); //!diz que esta ok para watchdog	
	}
	return 0;
}

/// @Brief Seta o filtro do canBus a ser usado
/// here...
/// @param O filtro a ser usado.
const	uint8_t can_filter[] PROGMEM = {
	// Group 0
	MCP2515_FILTER(0),				// Filter 0
	MCP2515_FILTER(1),				// Filter 1
	
	// Group 1
	MCP2515_FILTER(0),				// Filter 2
	MCP2515_FILTER(0),				// Filter 3
	MCP2515_FILTER(0),				// Filter 4
	MCP2515_FILTER(0),				// Filter 5
	
	MCP2515_FILTER(0),				// Mask 0 (for group 0)
	MCP2515_FILTER(0),				// Mask 1 (for group 1)
};