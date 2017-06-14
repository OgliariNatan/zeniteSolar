/*----------------------------------------------
--Modulo Interface do Chopper-PMDC--------------
--Autor Natan Ogliari --------------------------
--Equipe Zênite---------------------------------
--PD6; PB0; PA0---------------------------------
*/
#include <mcp_can.h>
#include <SPI.h>
//#include   avr/wdt.h //biblioteca do Watchdog
//define um nome para as entradas
#define aciON 7
#define aciOFF 5
#define aciChooperPMDC A1
#define aciDEDE 3



long canId = 0x000;//define o id de modulo
unsigned char stmp[8] = {0, 0, 0, 0, 0, 0, 0, 0};

typedef union estados_t{
  struct {
    unsigned char estON            : 1;
    unsigned char estOFF           : 1;
    unsigned char estChooperPMDC   : 1;
    unsigned char estDEDE          : 1;
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
  estados.estON            = !digitalRead(aciON);
  estados.estOFF           = !digitalRead(aciOFF);
  estados.estChooperPMDC   = analogRead(aciChooperPMDC);
  estados.estDEDE          = !digitalRead(aciDEDE);
  
  
  //arruma o vetor a ser enviado 

  Serial.print("Fim union");
  Serial.println(estados.todas);
  *stmp = estados.todas;
   
  // send data:  id = 0x00, standrad flame, data len = 8, stmp: data buf
  CAN0.sendMsgBuf(canId, 0, 1, stmp);//canId <- identificadorproprio, 
  Serial.print("dado enviado");
  Serial.println(*stmp);
  delay(100);
  //wdt_reset(); //diz que esta tudo ok
}
