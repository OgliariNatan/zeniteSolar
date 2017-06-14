/*--------------------------------
--Modulo interface dos sistemas auxiliares, via can bus
--Autor Natan Ogliari
--Equipe Zênite solar
--PD3; PD4; PD5; PD6; PD7;
*/
#include <mcp_can.h>
#include <SPI.h>
//#include   avr/wdt.h //biblioteca do Watchdog
//define um nome para as entradas
#define aciBomba1 3
#define aciBomba2 4
#define aciMPPT 5
#define aux1 6
#define aux2 7


long canId = 0x000;//define o id de modulo
unsigned char stmp[8] = {0, 0, 0, 0, 0, 0, 0, 0};

typedef union estados_t{
  struct {
    unsigned char estBomba1 : 1;
    unsigned char estBomba2 : 1;
    unsigned char estMPPT   : 1;
    unsigned char estAux1   : 1;
    unsigned char estAux2   : 1;
    };
  unsigned char todas;
} estados_t;

volatile estados_t estados;

MCP_CAN CAN0(10);                               // Set CS to pin 10


void setup()
{
  Serial.begin(115200);
  
  CAN0.begin(CAN_125KBPS);
  while (CAN_OK != CAN0.begin(CAN_125KBPS)) {
    Serial.println("Falha na inicialização");
    Serial.println("Carregando...");
    delay(500);
  }
  Serial.println("Iniciado com sucesso");
  //wdt_enable(WDTO_4S); //Função que ativa e altera o Watchdog(_1S _2S _4S _8S)tempos
}


void loop()
{
  //wdt_reset(); //diz que esta tudo ok
  estados.todas = 0;
  Serial.print("Inicio union");
  Serial.println(estados.todas);
  estados.estBomba1 = !digitalRead(aciBomba1);
  estados.estBomba2 = !digitalRead(aciBomba2);
  estados.estMPPT   = !digitalRead(aciMPPT);
  estados.estAux1   = !digitalRead(aux1);
  estados.estAux2   = !digitalRead(aux2);
  
  //arruma o vetor a ser enviado 

  Serial.print("Fim union");
  Serial.println(estados.todas);
  *stmp = estados.todas;
   
  // send data:  id = 0x00, standrad flame, data len = 8, stmp: data buf
  CAN0.sendMsgBuf(canId, 0, 3, stmp);
  Serial.print("dado enviado");
  Serial.println(*stmp);
  delay(100);
  //wdt_reset(); //diz que esta tudo ok
}
