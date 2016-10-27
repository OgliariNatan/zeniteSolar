/**
 * @file main.c
 * @brief MA_sisAux2016
 *
 * @author Natan Ogliari, João Antônio Cardoso
 *
 * @date 26/10/2016
 */

#include <avr/io.h>
#include <avr/pgmspace.h>

#include "ATmega328.h"
#include "globalDefines.h"
#include "can_defines.h"
#include "globalMacros.h"
#include "can.h"

#define MY_CAN_ID			ID_MA_SisAux2016    //! O ID deste módulo
#define MY_CAN_MSG_LENGHT	1					//! Número de bits enviados na mensagem

#define F_CPU 16000000UL

// PORTS, PINS and BITS definitions
#define BIT_BOMBA1		PD6
#define DDR_BOMBA1	    DDRD
#define PORT_BOMBA1	    PORTD
#define BIT_BOMBA2	    PB0
#define DDR_BOMBA2	    DDRB
#define PORT_BOMBA2	    PORTB
#define BIT_TensaoBat	PD4
#define DDR_TensaoBat   DDRD
#define PORT_TensaoBat  PORTD

#define ESTADO_INICIAL  0
typedef uint8_t estados_t;
enum bits_estado{
	 estBomba1 = 0,				//! estado da bomba 1, ativa em alto
	 estBomba2 					//! estado da bomba 2, ativa em alto
};

// Local aliases based on the global definitions of can_defines.h
#define sw_bomba1_msk       data_MI_SisAux2016_A
#define sw_bomba1_byte      byte_MI_SisAux2016_A
#define sw_bomba1_shift		shift_MI_SisAux2016_A
#define sw_bomba2_msk       data_MI_SisAux2016_B
#define sw_bomba2_byte      byte_MI_SisAux2016_B
#define sw_bomba2_shift		shift_MI_SisAux2016_B
/*
#define sw_mppt_msk         data_MI_SisAux2016_C
#define sw_mppt_byte        byte_MI_SisAux2016_C
#define sw_mppt_shift		shift_MI_SisAux2016_C
#define sw_aux1_msk         data_MI_SisAux2016_D
#define sw_aux1_byte        byte_MI_SisAux2016_D
#define sw_aux1_shift		shift_MI_SisAux2016_D
#define sw_aux2_msk         data_MI_SisAux2016_E
#define sw_aux2_byte        byte_MI_SisAux2016_E 
#define sw_aux2_shift		shift_MI_SisAux2016_E*/
/* 
#define on_off_msk          data_MA_SisAux2016_A
#define on_off_byte         byte_MA_SisAux2016_A
#define on_off_shift        shift_MA_SisAux2016_A
#define voltage_H_msk       data_MA_SisAux2016_B_H
#define voltage_H_byte      byte_MA_SisAux2016_B_H
#define voltage_L_msk       data_MA_SisAux2016_B_L
#define voltage_L_byte      byte_MA_SisAux2016_B_L
#define voltage_shift       shift_MA_SisAux2016_B */

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
 */
void updateSystemStatus(estados_t *estados)
{
	
}

/**
 * @brief Atualiza as saidas do sistema de acordo com os estados passados
 */
void updateOutputs(estados_t *estados)
{
	UPDATE_BIT(*estados, estBomba1, PORT_BOMBA1, BIT_BOMBA1);
	UPDATE_BIT(*estados, estBomba2, PORT_BOMBA2, BIT_BOMBA2);
}

/**
 * @brief Envia o estado do sistema via CAN
 *
 * @param estados é a variável que armazena os estados
 *
 */
void sendSystemStatusCAN(estados_t estados)
{
	// Cria e define o tamanho do pacote
	can_t msg;
	msg.id = MY_CAN_ID;			    // define o ID da mensagem
	msg.flags.rtr = 0;				// define como um data frame
	msg.length = MY_CAN_MSG_LENGHT;	// define o comprimento da mensagem
	
	// Empacota os dados a serem enviados via can.
    msg.data[0] = estados;

	// Envia o pacote
	can_send_message(&msg);
}

/**
 * @brief Desempacota uma mensagem do can e sobre-escreve os estados
 */
void readMsg(estados_t *estados, can_t *msg)
{
    // Try to read the message
    if(can_get_message(msg)){
		switch (msg->id){
			case ID_MI_SisAux2016:

				UPDATE_BIT(	
					getByte(msg->data, sw_bomba1_byte, sw_bomba1_msk),
					sw_bomba1_shift,
					*estados,
					estBomba1
				);

				UPDATE_BIT(
					getByte(msg->data, sw_bomba2_byte, sw_bomba2_msk),
					sw_bomba2_shift,
					*estados,
					estBomba2
				);

				
				break;
			// end of case ID_MI_SisAux2016
			default:
				/* Your code here */
			break;
		}
    }
}

/**
 * @brief Envia o estado do sistema via USART
 */
//void sendSystemStatusUSART(estados_t estados){};
#define sendSystemStatusUSART(x)	usartTransmit(x)

/**
 * @brief Configura a CAN
 */
void canSetup(void)
{
	// Initialize MCP2515
	can_init(BITRATE_125_KBPS);

	// Load filters and masks
	can_static_filter(can_filter);

	// Set normal mode
	can_set_mode(CAN_MODE);
}

/**
 * @brief Configura as entradas e saídas
 */
void ioSetup(void)
{
	// set as output
	setBit(DDR_BOMBA1, BIT_BOMBA1);
	setBit(DDR_BOMBA2, BIT_BOMBA2);
}

/**
 * @brief Configura a USART
 */
void usartSetup(void)
{
	usartConfig(    USART_MODE_ASYNCHRONOUS, 
                    USART_BAUD_9600, 
                    USART_DATA_BITS_8, 
                    USART_PARITY_NONE, 
                    USART_STOP_BIT_SINGLE   );
	usartEnableTransmitter();
	//usartEnableReceiver();
	//usartStdio();
	
	// Envia 8 bits para sinalizar a serial.
	for(uint8 i = 0; i<8; i++)		usartTransmit(1<<i);
}

/**
 * @brief Configura todo o sistema
 */
void configSystem(void)
{
	// Configure USART
	usartSetup();

	// Configure CAN
	canSetup();

	// Configure inputs and outputs
	ioSetup();
	
	// Enable global interrupts
	//sei();
}

/**
 * @brief Programa principal

int main(void)
{
	// Configure system
	configSystem();

	// Initialize system status
	estados_t estados;
	estados = ESTADO_INICIAL;

	can_t received_msg;     //! Armazena a mensagem recebida via CAN

	// main loop: reads a message and sends it via USART
	for(;;){

		// Checks if a new message was received
		if(can_check_message()){
			
			usartTransmit(1);

			//readMsg(&estados, &received_msg);
			
			// Try to read the message
			if(can_get_message(&received_msg)){

				usartTransmit(2);

				// Sends via USART
				usartTransmit(received_msg.data[0]);

			}

			// Atualiza as saídas
			//updateOutputs(&estados);

			// Envia via USART
			//sendSystemStatusUSART(estados);

		}else{
			usartTransmit(0);
		}
		
		_delay_ms(500);
		
	}
}
*/

int main(void)
{
	// Initialize MCP2515
	can_init(BITRATE_125_KBPS);
	
	// Load filters and masks
	can_static_filter(can_filter);
	
	// Create a test messsage
	can_t msg;
	
	/*msg.id = MY_CAN_ID;
	msg.flags.rtr = 0;
	
	msg.length = 4;
	msg.data[0] = 0xde;
	msg.data[1] = 0xad;
	msg.data[2] = 0xbe;
	msg.data[3] = 0xef;

	
	// Send the message
	can_send_message(&msg);

*/
	
	while (1)
	{
		// Check if a new messag was received
		if (can_check_message())
		{
			can_t msg;
			
			// Try to read the message
			if (can_get_message(&msg))
			{
				// Send the new message
				//can_send_message(&msg);
				usartTransmit(msg.data[0]);
				usartTransmit(msg.data[1]);
				usartTransmit(msg.data[2]);
				usartTransmit(msg.data[3]);

			}
		}else usartTransmit(0);
	}
	
	return 0;
}
