/**
 * @file main.c
 * @brief MA_sisAux2016
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
#define BOMBA1		PD6
#define DDR_BOMBA1	DDRD
#define PORT_BOMBA1	PORTD
#define BOMBA2	PB0
#define DDR_BOMBA2	DDRB
#define PORT_BOMBA2	PORTB
#define TensaoBat	PD4

#define MSG_START	0xFD

typedef union estados_t {
	struct {
		uint8_t Bomba1 : 1;
		uint8_t Bomba2 : 1;
		//uint8_t tensaoBat;
	};
	uint8_t all;
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

/*
 * @brief Configura USART
 *
void usartSetup(void)
{
	// USART Configuration
	usartConfig(USART_MODE_ASYNCHRONOUS, USART_BAUD_9600, USART_DATA_BITS_8, USART_PARITY_NONE, USART_STOP_BIT_SINGLE);
	usartEnableReceiver();
	usartEnableTransmitter();
	usartClearReceptionBuffer();
	usartActivateReceptionCompleteInterrupt();
	usartStdio();
}
*/
/**
 * @brief Configura as entradas e saídas
 */
void ioConfig(void)
{
	// set as output
	setBit(DDR_BOMBA1, BOMBA1);
	setBit(DDR_BOMBA2, BOMBA2);
}

/**
 * @brief Configura todo o sistema
 */
void configSystem(void)
{
	// Configure CAN
	canConfig();

	// Configure USART
	//usartSetup();

	// Configure inputs and outputs
	ioConfig();

	// Enable global interrupts
	sei();
}
/*
void usartRead(estados_t *estados, uint8 length)
{
	uint8 msg[length];
	// Enquanto houver dados no buffer
	// Descarta até encontrar o inicio da mensagem
	while((!usartIsReceiverBufferEmpty()) && (usartGetDataFromReceiverBuffer() != MSG_START));

	for(uint8 i = 0; i< length; i++){
		msg[length] = usartGetDataFromReceiverBuffer();
	}

	(*estados).Bomba1 = msg[0];
	(*estados).Bomba2 = msg[1];
}*/

void updateOutputs(estados_t *estados)
{
	if( estados->Bomba1 ) setBit(PORT_BOMBA1, BOMBA1);
	else clrBit(PORT_BOMBA1, BOMBA1);

	//ATUALIZA_BOMBA1();

	if( estados->Bomba2 ) setBit(PORT_BOMBA2, BOMBA2);
	else clrBit(PORT_BOMBA2, BOMBA2);
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
	while (1){

		// Checks if a new message was received
		if (can_check_message()){
			can_t received_msg;

			// Try to read the message
			if(can_get_message(&received_msg)){
								
				if ( 0x000 == received_msg.id){
					
					estados.Bomba1  = received_msg.data[0] << 4; //000A B000
					estados.Bomba2 |= received_msg.data[0] << 3;
					updateOutputs(&estados);
				}
			
			}	
		}
	}
}
