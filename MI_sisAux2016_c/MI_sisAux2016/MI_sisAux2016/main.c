/**
 * @file main.c
 * @brief MI_sisAux2016.c
 *
 * @author Natan Ogliari, João Antônio Cardoso
 *
 * @date 10/09/2016
 */

#include <avr/io.h>
#include <avr/pgmspace.h>

#include "globalDefines.h"
#include "ATmega328.h"
#include "can.h"

#define F_CPU 16000000UL

#define ESTADO_INICIAL 0b00000000
#define aciBomba1	PD3			//! botão para ativar a bomba 1
#define aciBomba2	PD4			//! botão para ativar a bomba 2
#define aciMPPT	    PD5			//! botão para ativar os mppts
#define aux1	    PD6			//! <sem função>
#define aux2	    PD7			//! <sem função>

typedef union estados_t{	//< cria uma estrutura para ser enviado no pacote.
	struct {
		uint8 estBomba1;		//! estado da bomba 1, ativa em alto
		uint8 estBomba2;		//! estado da bomba 2, ativa em alto
		uint8 estMPPT;			//! estado do mppt, ativo em alto
		uint8 estAux1;
		uint8 estAux2;
	};
	uint8 all;
} estados_t;

/**
 * @brief Seta o filtro do canBus a ser usado
 *
 * @param O filtro a ser usado.
 */
const uint8_t can_filter[] PROGMEM = {
	// Group 0
	MCP2515_FILTER(0),				// Filter 0
	MCP2515_FILTER(0),				// Filter 1

	// Group 1
	MCP2515_FILTER(0),				// Filter 2
	MCP2515_FILTER(0),				// Filter 3
	MCP2515_FILTER(0),				// Filter 4
	MCP2515_FILTER(0),				// Filter 5

	MCP2515_FILTER(0),				// Mask 0 (for group 0)
	MCP2515_FILTER(0),				// Mask 1 (for group 1)
};

void updateSystemStatus(estados_t *estados)
{
		(*estados).estBomba1 = isBitSet(PIND, aciBomba1);

		(*estados).estBomba2 = isBitSet(PIND, aciBomba2);
		
		(*estados).estMPPT = isBitSet(PIND, aciMPPT);

		(*estados).estAux1 = isBitSet(PIND, aux1);

		(*estados).estAux2 = isBitSet(PIND, aux2);
}

void sendSystemStatus(estados_t *estados)
{
	// Cria e define o tamanho do pacote
	can_t msg;
	msg.id = 0;
	msg.length = 5;
	msg.flags.rtr = 0;
	

	// Empacota os dados a serem enviados.
	estados->all=0;
	msg.data[0] = (*estados).estBomba1;
	msg.data[1] = (*estados).estBomba2;
	msg.data[2] = (*estados).estMPPT;
	msg.data[3] = (*estados).estAux1;
	msg.data[4] = (*estados).estAux2;

	// Envia o pacote
	can_send_message(&msg);
}

void canConfig(void)
{
	// Initialize MCP2515
	can_init(BITRATE_125_KBPS);

	// Load filters and masks
	can_static_filter(can_filter);
	
	//can_set_filter(0, &filter0);

	// Set normal mode
	can_set_mode(NORMAL_MODE);
}

void inputsConfig(void)
{
	// set as input
	DDRD &= (0 << aciBomba1) & (0 << aciBomba2) & (0 << aciMPPT) & (0 << aux1) & (0 << aux2);
	// set pull-up
	PORTD |= (1 << aciBomba1) & (1 << aciBomba2) & (1 << aciMPPT) & (1 << aux1) & (1 << aux2);
}

void configSystem(void)
{
	// Configure CAN
	canConfig();

	// Configure USART
	usartConfig(USART_MODE_ASYNCHRONOUS, USART_BAUD_9600, USART_DATA_BITS_8, USART_PARITY_NONE, USART_STOP_BIT_SINGLE);
	usartEnableTransmitter();

	// Configure inputs
	inputsConfig();
}

int main(void)
{
	// Configure system
	configSystem();

	// Initialize system status
	estados_t estados;
	estados.all = ESTADO_INICIAL;

	// main loop: reads a message and sends it via USART
	for(;;){

		// Checks if a new message was received
		if (can_check_message()){
			can_t received_msg;

			// Try to read the message
			if(can_get_message(&received_msg))
				// Sends via USART
				for(uint8 i=0; i<received_msg.length ;i++)
					usartTransmit(received_msg.data[i]);
		}

		// Updates system status
		updateSystemStatus(&estados);

		// Sends system status via CAN
		sendSystemStatus(&estados);

	}
}

