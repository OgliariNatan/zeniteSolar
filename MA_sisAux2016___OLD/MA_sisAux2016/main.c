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

#include "can_defines.h"
#include "globalMacros.h"

#define F_CPU 16000000UL

#define BIT_BOMBA1		PD6
#define DDR_BOMBA1	    DDRD
#define PORT_BOMBA1	    PORTD
#define BIT_BOMBA2	    PB0
#define DDR_BOMBA2	    DDRB
#define PORT_BOMBA2	    PORTB
#define BIT_TensaoBat	PD4
#define DDR_TensaoBat   DDRD
#define PORT_TensaoBat  PORTD

// Local aliases based on the global definitions of can_defines.h
#define sw_bomba1_msk       data_MI_SisAux2016_A
#define sw_bomba1_byte      byte_MI_SisAux2016_A
#define sw_bomba2_msk       data_MI_SisAux2016_B
#define sw_bomba2_byte      byte_MI_SisAux2016_B
/*#define sw_mppt_msk         data_MI_SisAux2016_C
#define sw_mppt_byte        byte_MI_SisAux2016_C
#define sw_aux1_msk         data_MI_SisAux2016_D
#define sw_aux1_byte        byte_MI_SisAux2016_D
#define sw_aux2_msk         data_MI_SisAux2016_E
#define sw_aux2_byte        byte_MI_SisAux2016_E */
/* #define on_off_msk          data_MA_SisAux2016_A
#define on_off_byte         byte_MA_SisAux2016_A
#define voltage_H_msk       data_MA_SisAux2016_B_H
#define voltage_H_byte      byte_MA_SisAux2016_B_H
#define voltage_L_msk       data_MA_SisAux2016_B_L
#define voltage_L_byte      byte_MA_SisAux2016_B_L */

typedef struct{
    uint8_t Bomba1;
    uint8_t Bomba2;
    //uint8_t tensaoBat;
}estados_t;

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
	can_init(CAN_BITRATE);

	// Load filters and masks
	can_static_filter(can_filter);

	// Set normal mode
	can_set_mode(NORMAL_MODE);
}

/**
 * @brief Configura USART
 */
void usartSetup(void)
{
	usartConfig(USART_MODE_ASYNCHRONOUS, 
                USART_BAUD_9600, 
                USART_DATA_BITS_8, 
                USART_PARITY_NONE, 
                USART_STOP_BIT_SINGLE);
	usartEnableReceiver();
	usartEnableTransmitter();
	usartClearReceptionBuffer();
	usartActivateReceptionCompleteInterrupt();
	usartStdio();
}

/**
 * @brief Configura as entradas e saídas
 */
void ioConfig(void)
{
	// set as output
	setBit(DDR_BOMBA1, BIT_BOMBA1);
	setBit(DDR_BOMBA2, BIT_BOMBA2);
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

/**
 * @brief Atualiza as saidas do sistema de acordo com os estados passados
 */
void updateOutputs(estados_t *estados)
{
	if( estados->Bomba1 ) setBit(PORT_BOMBA1, BIT_BOMBA1);
	else clrBit(PORT_BOMBA1, BIT_BOMBA1);

	if( estados->Bomba2 ) setBit(PORT_BOMBA2, BIT_BOMBA2);
	else clrBit(PORT_BOMBA2, BIT_BOMBA2);
}

/**
 * @brief Desempacota uma mensagem do can e sobre-escreve os estados
 */
void readMsg(estados_t *estados)
{
    can_t msg;

    // Try to read the message
    if(can_get_message(msg)){
								
        if(msg->id == ID_MI_SisAux2016){
            // para ler um bit como booleano podemos usar como abaixo:
            estados->Bomba1 = isTrue( getByte( msg->data, 
                                                sw_bomba1_byte, 
                                                sw_bomba1_msk
                                                ),
                                      sw_bomba1_msk 
                                      );
            estados->Bomba2 = isTrue(   getByte( msg->data,
                                                sw_bomba2_byte,
                                                sw_bomba2_msk
                                                ),
                                      sw_bomba2_msk 
                                      );

            /* estados->pot = getByte( msg->data,
                                    pot_byte,
                                    pot_msk 
                                    ); */
        }
    }
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
	for(;;){

		// Checks if a new message was received
		if(can_check_message()){

            readMsg(&estados);

            updateOutputs(&estados);
			
		}
	}
}
