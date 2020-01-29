/* Main.c file generated by New Project wizard
 *
 * Created:   qui mai 30 2019
 * Processor: PIC18F4520
 * Compiler:  MPLAB XC8
 * Autores: Sandro Weizenmann e David Pereira
 * Professor: Paulo Valim
 */

#include <xc.h>

# define _XTAL_FREQ 16000000
#pragma WDT = OFF, BOREN = OFF, WDTE = OFF

//========================================================
//--[DEFINI��O DOS BOT�ES INPUTS (DIGITAIS)]--
#define     LCD_DATA    PORTD
#define     LCD_RS      PORTCbits.RC3
#define     LCD_RW      PORTCbits.RC4
#define     LCD_EN      PORTCbits.RC5

#define     LCD_FIRST_ROW       0x80
#define     LCD_SECOND_ROW      0xC0
#define     LCD_CLEAR           0x01

#define     lcd_init        LCD_Init
#define     lcd_cmd         LCD_Cmd
#define     lcd_write       LCD_Write
#define     lcd_write_text  LCD_Write_Text

#define     Lcd_init        LCD_Init
#define     Lcd_cmd         LCD_Cmd
#define     Lcd_write       LCD_Write
#define     Lcd_write_text  LCD_Write_Text

#define     Lcd_Init        LCD_Init
#define     Lcd_Cmd         LCD_Cmd
#define     Lcd_Write       LCD_Write
#define     Lcd_Write_text  LCD_Write_Text

#define     Lcd_Write_Text  LCD_Write_Text

#define     Lcd_Data        LCD_Write
#define     lcd_data        LCD_Write
#define     Lcd_data        LCD_Write

//========================================================
//--[DEFINI��O DOS BOT�ES INPUTS (DIGITAIS)]--
# define ESTAGIO_1 PORTBbits.RB2	// BOT�ES DE estagio
# define EMERGENCIA PORTBbits.RB3	// BOT�O DE EMERG�NCIA
# define RCS PORTBbits.RB1      	// CHAVE RCS
# define LUZ PORTBbits.RB4          // CHAVE LUZ
# define SAS PORTBbits.RB5      	// CHAVE SAS
# define PRN_b1 PORTBbits.RB6		// CHAVE 3 POSI��ES PROGADE...
# define PRN_b2 PORTBbits.RB7		// ...RETROGADE E NORMAL

//--[DEFINI��O DOS BOT�ES INPUTS (ANAL�GICOS)]--
# define ACELERACAO PORTAbits.RA0	// ACELERADOR
# define DIRECAO_X PORTAbits.RA1	// POSI��O X DO JOYSTICK
# define DIRECAO_Y PORTAbits.RA2	// POSI��O Y DO JOYSTICK 

//========================================================
//--[DEFINI��O DOS LEDS OUTPUT]--
# define SAS_VERDE PORTAbits.RA3	// LED SAS
# define SAS_VERMELHO PORTCbits.RC2	// LED SAS
# define LUZ_VERMELHO PORTAbits.RA5	// LED LUZ
# define LUZ_VERDE PORTAbits.RA4	// LED LUZ
# define RCS_VERMELHO PORTCbits.RC0	// LED RCS
# define RCS_VERDE PORTCbits.RC1	// LED RCS
# define RETROGADE PORTEbits.RE0	// LED RETROGADE
# define PROGADE PORTEbits.RE1		// LED PROGADE
# define NORMAL PORTEbits.RE2		// LED NORMAL
//========================================================
//--[VARI�VEIS GLOBAIS]--
char aceleracao,movX,movY;                      // v�riavel respons�vel por transportar o comando
unsigned char leitura[16]={"Testando Leds..."}; // recebe a altitude do programa
char buffer0[17] = {"   EMERGENCIA! "};
unsigned int x;
char stt_LUZ = '0', stt_SAS = '0', stt_RCS = '0', 
stt_PRN1 = '0',stt_PRN2 = '0', stt_EMER = '0', 
stt_AC = '0', stt_ESTAGIO_1 = '0',
stt_X= '0', stt_Y= '0';
//=======================================================
void teste_leds(void);
void leds_status();
char startUSART();
void transmissao (char *);
char UART_Leitura(void);
void UART_Write(char);
void UART_Read_Text(char *, unsigned int );
//=======================================================

void LCD_Cmd(unsigned char command)
{
    LCD_DATA = command;
    LCD_RS = 0;
    LCD_RW = 0;
    LCD_EN = 1;
    __delay_ms(2);
    LCD_EN = 0;
}
void LCD_Init(void)
{
    TRISD = 0x00;
    TRISCbits.TRISC3 = 0;
    TRISCbits.TRISC4 = 0;
    TRISCbits.TRISC5 = 0;
    LCD_Cmd(0x38);
    __delay_ms(30);
    LCD_Cmd(0x0E);
    __delay_ms(20);
    LCD_Cmd(0x0C);
    __delay_ms(20);
    LCD_Cmd(LCD_CLEAR);
    __delay_ms(20);
    LCD_Cmd(LCD_FIRST_ROW);
    __delay_ms(20);
}

void LCD_Write(unsigned char Data)
{
    LCD_DATA = Data;
    LCD_RS = 1;
    LCD_RW = 0;
    LCD_EN = 1;
    __delay_ms(2);
    LCD_EN = 0;
}
void LCD_Write_Text(unsigned char *msg)
{
    while(*msg)
    {
        LCD_Write(*msg);
        msg++;
    }
}


void __interrupt() TrataInt (void){	// tratamento de interrup��o
    if(INTCON3bits.INT2IF){         // interrup��o externa
        if (ESTAGIO_1 != stt_ESTAGIO_1 ){
            UART_Write ('3'); 
            stt_ESTAGIO_1 = ESTAGIO_1;  
        }
        
        __delay_ms(20);
        INTCON3bits.INT2IF = 0;
    }
    if(INTCONbits.TMR0IF){          // interrup��o por timer
        TMR0H = 0x0B;               // valor obtido pela f�rmula A=(t*f)/(4*prescaler)
        TMR0L = 0xDC;               // 2^b-A = ao valor que vai em TMR0H e TMR0L
        UART_Write('9');
        INTCONbits.TMR0IF = 0;		// zera flag do estouro do timer, se n�o zerar o programa para.
        UART_Read_Text(&leitura,16);
        LCD_Cmd(LCD_SECOND_ROW);
        LCD_Write_Text(leitura);
    }
}

void main(void)
 {
    TRISA = 0x07;   	// configura bits da porta A 7:3 como saida e bits 2:0 como entrada
    TRISB = 0xFF;       // configura todos bits da porta B como entrada
    TRISC = 0xC0;       // configura bits 7 e 6 como entrada e demais como sa�da
    TRISD = 0x00;       // configura todos os bits da porta D como sa�da
    TRISEbits.RE0 = 0;	// configura bit como sa�da
    TRISEbits.RE1 = 0;	// configura bit como sa�da
    TRISEbits.RE2 = 0;	// configura bit como sa�da
    TRISAbits.RA6 = 0;
    T0CON = 0x95;       //configra��es abaixo
   /* T08BIT = 0, configura timer para 16 bits
   ** T0CS = 0, configura para usar freq/4 (frequencia interna)
   ** PSA = 0, prescaler atribu�do.
   ** prescaler de 64 
   */
    ADCON0 = 0x09;
    /* bits 5-2 sele��o dos canais anal�gicos 0010 AN02
     * bit 1 convers�o ainda n�o est� em progresso.
     * bit 0 m�dulo de convers�o habilitado */
    ADCON1 = 0x0C;
    /* bit 5 seleciona VSS
     * bit 4 seleciona VDD
     * bit 3-0 seleciona AN0-AN2 como Anal�gicas, restante digitais */
    ADCON2 = 0x1D;
    /* 0b00011101
     * bit 7 - Justifica (formata��o) a esquerda
     * bits 5:3 - Tempo de aquisi��o 011 = 6 us
     * bits 2:0 - Configura��o de origem do clock para 16MHz.
     *      	   16x62,5ns=1000ns (>700ns) */
    
     char buffer[17] = {" Seja bem vindo! "};
     char buffer2[17] = {"Testando Leds..."};
     char buffer3[17] =  {"   Altitude: "};
     char buffer4[17] = {"  0000,000"};
    
    INTCONbits.INT0IE = 1;  //configura��o para tratamento
    INTCONbits.GIE = 1;     // de interrup��o sem ordem de prioridade.
    INTCON3bits.INT2IE = 1;
    INTCONbits.PEIE = 1;
    
    LCD_Init();    
    startUSART();
    LCD_Cmd(LCD_FIRST_ROW);
    LCD_Write_Text(buffer);
    LCD_Cmd(LCD_SECOND_ROW);
    LCD_Write_Text(buffer2);
    teste_leds();
    leds_status();
    LCD_Cmd(LCD_CLEAR);
    LCD_Cmd(LCD_FIRST_ROW);
    LCD_Write_Text(buffer3);
    LCD_Cmd(LCD_SECOND_ROW);
    LCD_Write_Text(buffer4);
    

   
    INTCONbits.TMR0IE = 1;      // Habilita interrup��es timer 0
    TMR0H = 0x0B;               // valor obtido pela f�rmula A=(t*f)/(4*prescaler)
    TMR0L = 0xDC;               //2^b-A = ao valor que vai em TMR0H e TMR0L
   
   while (1){
       

   if (RCS != stt_RCS ){
       RCS_VERDE = RCS;	
       RCS_VERMELHO = ~RCS;
       UART_Write ('5');
       stt_RCS = RCS;
    }
  
    if (LUZ != stt_LUZ ){
       LUZ_VERDE = LUZ;
       LUZ_VERMELHO = ~LUZ;
       UART_Write ('U'); 
       stt_LUZ = LUZ;
    }
    
    if (SAS != stt_SAS){
       SAS_VERDE = SAS;
       SAS_VERMELHO = ~SAS;
       UART_Write ('4'); 
       stt_SAS = SAS; 
    }
     

    if(PRN_b1 != stt_PRN1 || PRN_b2 != stt_PRN2){
      if ((PRN_b1 == 1 && PRN_b2 == 1)){
       RETROGADE = 1;
       
       PROGADE = 0;
       
       NORMAL = 0;
       UART_Write ('G'); 
       UART_Write ('p'); 
       UART_Write ('n'); 
       stt_PRN1 = PRN_b1;
       stt_PRN2 = PRN_b2;
    }
     else if((PRN_b1 == 0 && PRN_b2 ==1)){
       PROGADE = 1;
     
       RETROGADE = 0;
      
       NORMAL = 0;
      
       UART_Write ('g'); 
       UART_Write ('P');       
       UART_Write ('n'); 
       stt_PRN1 = PRN_b1;
       stt_PRN2 = PRN_b2;
     }
     
     else if(PRN_b1 == 1 && PRN_b2 == 0) {
       NORMAL = 1;
     
       PROGADE = 0;

       RETROGADE = 0;
       UART_Write ('g'); 
       UART_Write ('p'); 
       UART_Write ('N'); 
       stt_PRN1 = PRN_b1;
       stt_PRN2 = PRN_b2;
    } 
    }
 
   if( EMERGENCIA  != stt_EMER){
       UART_Write('E');
    stt_EMER = EMERGENCIA; 
    }
    

      ADCON0 = 0b00000011; 		// Configurando convers�o para AN0
      __delay_ms (4);
      aceleracao = ADRESH;		// acelera��o recebe o valor da parte alta do
                                // resultado da convers�o.
      if((((aceleracao - stt_AC) > 1) || (stt_AC - aceleracao) > 1)) {
            UART_Write('0');
            __delay_ms(2);
            UART_Write(aceleracao);
            stt_AC = aceleracao;
             } 
      
      ADCON0 = 0b00000111; 		// Configurando para convers�o AN1
      __delay_ms (4);
      movY = ADRESH;			// movY recebe o valor da parte alta do
                                // resultado da convers�o.
      
      if (((movY-stt_Y) > 1) || ((stt_Y- movY) > 1)){
            __delay_ms(2);
           UART_Write('Y');
            UART_Write(movY);
            stt_Y = movY;
             }
      
      ADCON0 = 0b00001011; 		// Configurando para Convers�o AN2
      __delay_ms (4);
      movX = ADRESH;			// movX recebe o valor da parte alta do
				                // resultado da convers�o.
      if (((movX-stt_X) > 1) || ((stt_X- movX) > 1)){
     UART_Write('X');
	 UART_Write(movX);
	 stt_X = movX;
      }
   }

 }
void leds_status(){
    if (RCS != stt_RCS ){
       RCS_VERDE = RCS;	
       RCS_VERMELHO = ~RCS;
       stt_RCS = RCS;
    }
  
    if (LUZ != stt_LUZ ){
       LUZ_VERDE = LUZ;
       LUZ_VERMELHO = ~LUZ; 
       stt_LUZ = LUZ;
    }
    
    if (SAS != stt_SAS){
       SAS_VERDE = SAS;
       SAS_VERMELHO = ~SAS;
       stt_SAS = SAS; 
    }
     
    if(PRN_b1 != stt_PRN1 || PRN_b2 != stt_PRN2){
            if ((PRN_b1 == 1 && PRN_b2 == 1)){
             RETROGADE = 1;
             PROGADE = 0;
             NORMAL = 0; 
             stt_PRN1 = PRN_b1;
             stt_PRN2 = PRN_b2;
          }
           else if((PRN_b1 == 0 && PRN_b2 ==1)){
             PROGADE = 1;
             RETROGADE = 0;
             NORMAL = 0; 
             stt_PRN1 = PRN_b1;
             stt_PRN2 = PRN_b2;
           }

           else if(PRN_b1 == 1 && PRN_b2 == 0) {
             NORMAL = 1;
             PROGADE = 0;
             RETROGADE = 0; 
             stt_PRN1 = PRN_b1;
             stt_PRN2 = PRN_b2;
           } 
    }
 
   if( EMERGENCIA  != stt_EMER){
    stt_EMER = EMERGENCIA; 
    }
          
      ADCON0 = 0b00000111; 		// Configurando para convers�o AN1
      __delay_ms (4);
      movY = ADRESH;			// movY recebe o valor da parte alta do
				                // resultado da convers�o.
      
      if (((movY-stt_Y) > 1) || ((stt_Y- movY) > 1)){
            __delay_ms(2);
            stt_Y = movY;
             }
      
      ADCON0 = 0b00001011; 		// Configurando para Convers�o AN2
      __delay_ms (4);
      movX = ADRESH;			// movX recebe o valor da parte alta do
				                // resultado da convers�o.
      if (((movX-stt_X) > 1) || ((stt_X- movX) > 1)){
         stt_X = movX;
      }
}
 //==========================================================
 //--[TESTANDO FUNCIONAMENTO DOS LEDS ]--
 /* Mant�m LEDS ligados por 0,8 segundos, para verificar se est�o
  * todos em perfeito funcionamento */
 void teste_leds(){ 	 
    SAS_VERDE = 1;
    SAS_VERMELHO = 1;
    LUZ_VERDE = 1;
    LUZ_VERMELHO = 1;
    RCS_VERMELHO = 1;
    RCS_VERDE = 1;
    RETROGADE =1;
    PROGADE = 1;
    NORMAL = 1;
    __delay_ms(800);
    SAS_VERDE = 0;
    SAS_VERMELHO = 0;
    LUZ_VERDE = 0;
    LUZ_VERMELHO = 0;
    RCS_VERMELHO = 0;
    RCS_VERDE = 0;
    RETROGADE =0;
    PROGADE = 0;
    NORMAL = 0;
}

//=======================================================
//-------[FUN��ES RS-232 MODO ASSINCRONO]--------------||
//=======================================================

char startUSART(){               // INICIALIZA USART
    SPBRG = 25;                  // Escrevendo o Registro SPBRG => baudrate = 9600
    BRGH = 0;
    BRG16 = 0;
    SYNC = 0;                    // Configurando o Modo Ass�ncrono, isto �, o UART
    SPEN = 1;                    // Habilitando porta serial
    TRISC7 = 1;                  // Conforme prescrito no Datasheet
    TRISC6 = 1;                  // Conforme prescrito no Datasheet
    CREN = 1;                    // Habilita a recep��o cont�nua
    TXEN = 1;                    // Habilita transimss�o
    return 1;                    // Retorna 1 indicando sucesso na inicializa��o
 }

void UART_Write(char data)
{
  while(!TRMT);
  TXREG = data;
  
}

void transmissao(char *text)
{
  int i;
  for(i=0;text[i]!='\0';i++)
    UART_Write(text[i]);
}

char UART_Leitura()
{
  while(!RCIF);
  return RCREG;
}

void UART_Read_Text(char *Output, unsigned int length)
{
  unsigned int i;
  for(int i=0;i<length;i++)
  Output[i] = UART_Leitura();
}
