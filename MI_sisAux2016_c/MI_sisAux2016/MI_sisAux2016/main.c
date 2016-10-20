/**
 * @file main.c
 * @brief MI_sisAux2016
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

#define MSG_START	0xFD

typedef union estados_t{		//! estrutura que armazena o estado do sistema
	struct {
		uint8 estBomba1 : 1;		//! estado da bomba 1, ativa em alto
		uint8 estBomba2 : 1;		//! estado da bomba 2, ativa em alto
		uint8 estMPPT   : 1;		//! estado do mppt, ativo em alto
		uint8 estAux1   : 1;
		uint8 estAux2   : 1;
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

/**
 * @brief Atualiza o estado do sistema de acordo com as chaves
 *
 * @param *estados é o ponteiro da variável que armazena os estados
 */
void updateSystemStatus(estados_t *estados)
{
	estados->estBomba1 = isBitSet(PIND, aciBomba1);
	estados->estBomba2 = isBitSet(PIND, aciBomba2);
	estados->estMPPT   = isBitSet(PIND, aciMPPT);
	estados->estAux1   = isBitSet(PIND, aux1);
	estados->estAux2   = isBitSet(PIND, aux2);
}

/**
 * @brief Envia o estado do sistema via CAN
 *
 * @param *estados é o ponteiro da variável que armazena os estados
 *
 */
void sendSystemStatus(estados_t *estados)
{
	// Cria e define o tamanho do pacote
	can_t msg;
	msg.id = 0x0030;			    // define o ID da mensagem
	msg.flags.rtr = 0;				// define como um data frame
	msg.length = 1;					// define o comprimento da mensagem
	msg.data[0] = 0;

	// Empacota os dados a serem enviados via can.
	msg.data[0] = estados->estBomba1   << 4;
	msg.data[0] = estados->estBomba2   << 3;
	msg.data[0] = estados->estMPPT     << 2;
	msg.data[0] = estados->estAux1     << 1;
	msg.data[0] = estados->estAux2     << 0;

	// Envia o pacote
	can_send_message(&msg);
}

void sendSystemStatus2(estados_t *estados, uint8 length)
{
	uint8 data[length];

	// Empacota os dados a serem enviados.
	data[0] = MSG_START;
	data[1] = estados->estBomba1;
	data[2] = estados->estBomba2;
	data[3] = estados->estMPPT;
	data[4] = estados->estAux1;
	data[5] = estados->estAux2;
	
	// Envia o pacote
	for(uint8 i = 0; i<length+1; i++)
		usartTransmit(data[i]);
}

/**
 * @brief Configura a CAN
 */
void canConfig(void)
{
	// Initialize MCP2515
	can_init(BITRATE_125_KBPS);

	// Load filters and masks
	can_static_filter(can_filter);

	// Set normal mode
	can_set_mode(NORMAL_MODE);
}

/**
 * @brief Configura as entradas e saídas
 */
void ioConfig(void)
{
	// set as input
	clrBit(DDRD, aciBomba1);//!<Coloca o pino em zero "Como entrada"
	clrBit(DDRD, aciBomba2);
	clrBit(DDRD, aciMPPT);
	clrBit(DDRD, aux1);
	clrBit(DDRD, aux2);

	// set pull-up
	setBit(PORTD, aciBomba1);
	setBit(PORTD, aciBomba2);
	setBit(PORTD, aciMPPT);
	setBit(PORTD, aux1);
	setBit(PORTD, aux2);
}

/**
 * @brief Configura todo o sistema
 */
void configSystem(void)
{
	// Configure CAN
	canConfig();

	// Configure USART
	//usartConfig(USART_MODE_ASYNCHRONOUS, USART_BAUD_9600, USART_DATA_BITS_8, USART_PARITY_NONE, USART_STOP_BIT_SINGLE);
	//usartEnableTransmitter();
	//usartEnableReceiver();
	//usartStdio();

	// Configure inputs and outputs
	ioConfig();
}

/**
 * @brief Programa principal
 */
int main(void)
{
	// Configure system
	configSystem();

	// Initialize system status
	estados_t estados;
	estados.all = ESTADO_INICIAL;

	// main loop: reads a message and sends it via USART
	can_t received_msg;
	while (1)
	{

		// Checks if a new message was received
		if (can_check_message()){
			

			// Try to read the message
			if(can_get_message(&received_msg));

				/*// Sends via USART
				for(uint8 i=0; i<received_msg.length ;i++)
					usartTransmit(received_msg.data[i]);*/
		}

		// Updates system status
		updateSystemStatus(&estados);

		// Sends system status via CAN
		sendSystemStatus(&estados);
		_delay_ms(500);

	}
}
