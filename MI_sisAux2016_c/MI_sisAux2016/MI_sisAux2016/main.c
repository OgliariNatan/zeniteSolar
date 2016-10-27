/**
 * @file main.c
 * @brief MI_sisAux2016
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

#define MY_CAN_ID			ID_MI_SisAux2016    //! O ID deste módulo
#define MY_CAN_MSG_LENGHT	1					//! Número de bits enviados na mensagem

#define F_CPU 16000000UL

// PORTS, PINS and BITS definitions
#define BIT_aciBomba1	PD3			//! botão para ativar a bomba 1
#define PIN_aciBomba1	PIND
#define PORT_aciBomba1	PORTD
#define BIT_aciBomba2	PD4			//! botão para ativar a bomba 2
#define PIN_aciBomba2	PIND
#define PORT_aciBomba2	PORTD
#define BIT_aciMPPT	    PD5			//! botão para ativar os mppts
#define PIN_aciMPPT	    PIND
#define PORT_aciMPPT	PORTD
#define BIT_aux1	    PD6			//! <sem função>
#define PIN_aux1	    PIND
#define PORT_aux1	    PORTD
#define BIT_aux2	    PD7			//! <sem função>
#define PIN_aux2	    PIND
#define PORT_aux2	    PORTD

#define ESTADO_INICIAL  0
typedef uint8_t estados_t;
enum bits_estado{
	 estBomba1 = 0,				//! estado da bomba 1, ativa em alto
	 estBomba2, 				//! estado da bomba 2, ativa em alto
	 estMPPT,					//! estado do mppt, ativo em alto
	 estAux1,
	 estAux2
};

// Local aliases based on the global definitions of can_defines.h
#define sw_bomba1_msk       data_MI_SisAux2016_A
#define sw_bomba1_byte      byte_MI_SisAux2016_A
#define sw_bomba2_msk       data_MI_SisAux2016_B
#define sw_bomba2_byte      byte_MI_SisAux2016_B
#define sw_mppt_msk         data_MI_SisAux2016_C
#define sw_mppt_byte        byte_MI_SisAux2016_C
#define sw_aux1_msk         data_MI_SisAux2016_D
#define sw_aux1_byte        byte_MI_SisAux2016_D
#define sw_aux2_msk         data_MI_SisAux2016_E
#define sw_aux2_byte        byte_MI_SisAux2016_E

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
	UPDATE_BIT(		PIN_aciBomba1,	BIT_aciBomba1,	*estados,	estBomba1		);
	UPDATE_BIT(		PIN_aciBomba2,	BIT_aciBomba2,	*estados,	estBomba2		);
	UPDATE_BIT(		PIN_aciMPPT,	BIT_aciMPPT,	*estados,	estMPPT			);
	UPDATE_BIT(		PIN_aux1,		BIT_aux1,		*estados,	estBomba1		);
	UPDATE_BIT(		PIN_aux2,		BIT_aux2,		*estados,	estBomba1		);
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
    //msg.data[0] = estados;
	msg.data[0] = 123;

	// Envia o pacote
	can_send_message(&msg);
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
	// set as input
	clrBit(DDRD, BIT_aciBomba1);//!<Coloca o pino em zero "Como entrada"
	clrBit(DDRD, BIT_aciBomba2);
	clrBit(DDRD, BIT_aciMPPT);
	clrBit(DDRD, BIT_aux1);
	clrBit(DDRD, BIT_aux2);

	// set pull-up
	setBit(PORTD, BIT_aciBomba1);
	setBit(PORTD, BIT_aciBomba2);
	setBit(PORTD, BIT_aciMPPT);
	setBit(PORTD, BIT_aux1);
	setBit(PORTD, BIT_aux2);
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
 */
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
		if (can_check_message()){

			// Try to read the message
			if(can_get_message(&received_msg)){

				// Sends via USART
                sendSystemStatusUSART(received_msg.data[0]);

			}
		}else{
			usartTransmit(0);
		}

		// Updates system status
		updateSystemStatus(&estados);

		// Sends system status via CAN
		sendSystemStatusCAN(estados);

		_delay_ms(500);

	}
}
