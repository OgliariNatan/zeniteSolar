/*----------------------------------------------
--Modulo Acionamento dos Sistemas auxiliares----
--Autor Natan Ogliari --------------------------
--Equipe Zênite---------------------------------
--PD6; PB0; PA0---------------------------------
*/
#include "CAN_BUS_Shield/mcp_can.h"
#include <SPI.h>
//#include   avr/wdt.h //biblioteca do Watchdog

unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];

#define BOMBA1  6      //seta pino 6 (PD6) como BOMBA 1
#define BOMBA2  8      //seta pino 8(PB0) como BOMBA 2
#define INTER   A0      //seta A0 (PA0), como INTERrupção do mcp2515

typedef union estados_t {
  struct {
    unsigned char bomba1 :1;
    unsigned char bomba2 :1;
    //...
  };
  unsigned char todas;
} estados_t;

volatile estados_t estados;

MCP_CAN CAN0(10);                               // Set CS to pin 10


void setup()
{
  Serial.begin(115200);
  pinMode(INTER, INPUT);   // Setting pin A0 for /INT input
  pinMode(BOMBA1, OUTPUT);
  pinMode(BOMBA2, OUTPUT);
  Serial.begin(115200);
                        
  while (CAN_OK != CAN0.begin(CAN_125KBPS)) { // init can bus : baudrate = 500k
    Serial.println("Falha na inicialização");
    Serial.println("Carregando...");
    delay(500);
  }
  Serial.println("Iniciado com sucesso");
  estados.todas = 0;
  //wdt_enable(WDTO_4S); //Função que ativa e altera o Watchdog(_1S _2S _4S _8S)tempos
}

void loop()
{
  //wdt_reset(); //diz que esta tudo ok
  if (!digitalRead(INTER))                        // If pin A0 is low, read receive buffer
  {
    rxId = CAN0.getCanId();
    if (rxId == 0x000) {
      CAN0.readMsgBuf(&len, rxBuf);              // Read data: len = data length, buf = data byte(s)
      estados.todas = *rxBuf;
      // Get message ID
      Serial.println("Recebendo");
    }
    //wdt_reset(); //diz que esta tudo ok
    Serial.print("Dados");
    Serial.println(estados.todas);
  }
  digitalWrite(BOMBA1, estados.bomba1);
  digitalWrite(BOMBA2, estados.bomba2);
  //wdt_reset(); //diz que esta tudo ok
}


