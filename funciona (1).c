/**********************

ALUNO: Casio Pacheco Krebs
RGA: 2015.1905.003-8

************************/

#include "config.c"
#include "def_pinos.h"
#include <stdio.h>
//#include "amostras2.c"

int muda_numero = 0;
//unsigned char DIR, ESQ;

#define NOP4()	NOP();NOP();NOP();NOP()
#define NOP8()	NOP4();NOP4()
#define NOP12()	NOP4();NOP8()

/********************

Para o display grafico

**********************/
/*
#define CS1 P2_0
#define CS2 P2_1
#define RS P2_2
#define RW P2_3
#define E P2_4
#define RST P2_5
#define DB P4
#define ESQ 0
#define DIR 1
*/

/******************

Para o display alfanumérico

**********************/
#define RS P3_0
#define E P3_1
#define DB P3

#define NI 0		//nibble
#define BY 1		//byte
#define CO 0		//comando
#define DA 1  		//dado

void delay_ms(unsigned int t);//delay em milisegundos
void delay25_8ms(unsigned int t);//delay em milisegundos
void delay_us(unsigned int t);//delay em microsegundos
void display7(unsigned int n,unsigned int p,unsigned int del);//n é o numero do display, p é se vai ligar ou nao e del o delay
void mostra_car2(unsigned char n,unsigned int t);//procura o caracter na matrix
void numberzero(unsigned char lin,unsigned int t);//procura o caracter da matriz
void print_LEDS(unsigned char * pont, unsigned int t);//liga os leds  deslocando a cada momento
void display7_point(unsigned int n,unsigned int pont);//liga o ponto do display
//void funcao_isr(void) __interrupt 5; //interrupção no microcontrolador
void esc_LCDan(unsigned char dado, __bit nb, __bit cd); //escreve dado/comando no display LCD
void Ini_LCDan(void);  // Inicializa o LCD para 2 linhas e caracteres 5x8
void putchar(char c);  // Usada pela printf_fast_f para escrever no LCD
void desl_string(int t, __code char * p, unsigned char nd); // Faz as string no display alfanumérico
void conf_Y(unsigned char y, __bit cs);
void conf_pag(unsigned char pag, __bit cs);
void esc_glcd(unsigned char dado, __bit cd, __bit cs);
void limpa_glcd(__bit cs);
void ini_glcd(void);
/*
- aguardar os controladores do lcd terminarem o reset por power up
- ligar o display
- zerar os indicadores de posição
-  definir a condição inicial de E, REST, CS1,CS2 e DB
*/
unsigned char le_glcd( __bit cd, __bit cs);
void putchar2( char c);

/******************
*
* 		P2
*
*******************/
int le_teclas (int porta0,int porta1);
void contarpulso();
float le_pulso(void);
void velocidade();
unsigned int le_hcsr04(void);
void esc_DAC0 (float v);
void tocamusica(void) __interrupt 5;
unsigned int le_adc0(unsigned char canal,unsigned char ganho);
void esc_RAM_SPI (unsigned int end, unsigned char dado);
unsigned char le_RAM_SPI (unsigned int end);

/***************************
Para teclas

***************************/

#define IT0 P0_0
//#define IE0 P0_1
//#define IE1 P0_2
#define TECS_OPEN 21
volatile unsigned int C = 0;
volatile unsigned char NUM=21;
volatile float cont=0;
/***************************
Para conversão A/D

***************************/

#define AIN0 0
#define AIN1 1
#define AIN2 2
#define AIN3 3
#define HVDA 4
#define AGND 5
#define P3EVEN 6
#define P3ODD 7
#define TEMP 8

#define G1 0
#define G2 1
#define G4 2
#define G8 3
#define G16 4
#define G05 6


/*******************

SPI

*******************/

#define CS P2_3
#define HOLD P2_2

__code unsigned char caract[44][4]={
	{0xff,0x81,0x81,0xff},//0
	{0x84,0x82,0xff,0x80},//1
	{0xe2,0x91,0x89,0x86},//2
	{0x42,0x89,0x89,0x76},//3
	{0x0f,0x08,0x08,0xff},//4
	{0x8f,0x89,0x89,0x71},//5
	{0x7e,0x89,0x89,0x72},//6
	{0xc3,0x31,0x0d,0x03},//7
	{0xff,0x89,0x89,0xff},//8
	{0x4e,0x91,0x91,0x7e},//9

	{0x00,0x66,0x66,0x00},//:
	{0x00,0x86,0x66,0x00},//;
	{0x18,0x24,0x42,0x81},//<
	{0x24,0x24,0x24,0x24},//=
	{0x81,0x42,0x24,0x18},//>
	{0x06,0xb1,0x91,0x0e},//?
	{0x7e,0x99,0xa5,0x5e},//@

	{0xfe,0x11,0x11,0xfe},//A
	{0xff,0x89,0x89,0x76},//B
	{0x7e,0x81,0x81,0x42},//C
	{0xff,0x81,0x81,0x7e},//D
	{0xff,0x89,0x89,0x89},//E
	{0xff,0x09,0x09,0x09},//F
	{0x7e,0x81,0x91,0x72},//G
	{0xff,0x08,0x08,0xff},//H
	{0x81,0xff,0xff,0x81},//I
	{0x61,0x81,0xff,0x01},//J
	{0xff,0x18,0x34,0xc3},//K
	{0xff,0x80,0x80,0x80},//L
	{0xff,0x06,0x06,0xff},//M
	{0xff,0x0c,0x30,0xff},//N
	{0x7e,0x81,0x81,0x7e},//O
	{0xff,0x11,0x11,0x0e},//P
	{0x7e,0xa1,0xc1,0xbe},//Q
	{0xff,0x11,0x71,0xce},//R
	{0x66,0x89,0x89,0x72},//S
	{0x03,0xff,0xff,0x03},//T
	{0x7f,0x80,0x80,0x7f},//U
	{0x3f,0x60,0xc0,0x3f},//V
	{0xff,0x30,0x60,0xff},//W
	{0xe7,0x18,0x18,0xe7},//X
	{0x07,0x08,0xf8,0x07},//Y
	{0xe3,0x91,0x89,0xc7},//Z

	{0xff,0xff,0xff,0xff} //tela cheia
};
__code unsigned char fonte[96][5] = {
{0x00, 0x00, 0x00, 0x00, 0x00},// (space)
{0x00, 0x00, 0x5F, 0x00, 0x00},// !
{0x00, 0x07, 0x00, 0x07, 0x00},// "
{0x14, 0x7F, 0x14, 0x7F, 0x14},// #
{0x24, 0x2A, 0x7F, 0x2A, 0x12},// $
{0x23, 0x13, 0x08, 0x64, 0x62},// %
{0x36, 0x49, 0x55, 0x22, 0x50},// &
{0x00, 0x05, 0x03, 0x00, 0x00},// '
{0x00, 0x1C, 0x22, 0x41, 0x00},// (
{0x00, 0x41, 0x22, 0x1C, 0x00},// )
{0x08, 0x2A, 0x1C, 0x2A, 0x08},// *
{0x08, 0x08, 0x3E, 0x08, 0x08},// +
{0x00, 0x50, 0x30, 0x00, 0x00},// ,
{0x08, 0x08, 0x08, 0x08, 0x08},// -
{0x00, 0x30, 0x30, 0x00, 0x00},// .
{0x20, 0x10, 0x08, 0x04, 0x02},// /
{0x3E, 0x51, 0x49, 0x45, 0x3E},// 0
{0x00, 0x42, 0x7F, 0x40, 0x00},// 1
{0x42, 0x61, 0x51, 0x49, 0x46},// 2
{0x21, 0x41, 0x45, 0x4B, 0x31},// 3
{0x18, 0x14, 0x12, 0x7F, 0x10},// 4
{0x27, 0x45, 0x45, 0x45, 0x39},// 5
{0x3C, 0x4A, 0x49, 0x49, 0x30},// 6
{0x01, 0x71, 0x09, 0x05, 0x03},// 7
{0x36, 0x49, 0x49, 0x49, 0x36},// 8
{0x06, 0x49, 0x49, 0x29, 0x1E},// 9
{0x00, 0x36, 0x36, 0x00, 0x00},// :
{0x00, 0x56, 0x36, 0x00, 0x00},// ;
{0x00, 0x08, 0x14, 0x22, 0x41},// <
{0x14, 0x14, 0x14, 0x14, 0x14},// =
{0x41, 0x22, 0x14, 0x08, 0x00},// >
{0x02, 0x01, 0x51, 0x09, 0x06},// ?
{0x32, 0x49, 0x79, 0x41, 0x3E},// @
{0x7E, 0x11, 0x11, 0x11, 0x7E},// A
{0x7F, 0x49, 0x49, 0x49, 0x36},// B
{0x3E, 0x41, 0x41, 0x41, 0x22},// C
{0x7F, 0x41, 0x41, 0x22, 0x1C},// D
{0x7F, 0x49, 0x49, 0x49, 0x41},// E
{0x7F, 0x09, 0x09, 0x01, 0x01},// F
{0x3E, 0x41, 0x41, 0x51, 0x32},// G
{0x7F, 0x08, 0x08, 0x08, 0x7F},// H
{0x00, 0x41, 0x7F, 0x41, 0x00},// I
{0x20, 0x40, 0x41, 0x3F, 0x01},// J
{0x7F, 0x08, 0x14, 0x22, 0x41},// K
{0x7F, 0x40, 0x40, 0x40, 0x40},// L
{0x7F, 0x02, 0x04, 0x02, 0x7F},// M
{0x7F, 0x04, 0x08, 0x10, 0x7F},// N
{0x3E, 0x41, 0x41, 0x41, 0x3E},// O
{0x7F, 0x09, 0x09, 0x09, 0x06},// P
{0x3E, 0x41, 0x51, 0x21, 0x5E},// Q
{0x7F, 0x09, 0x19, 0x29, 0x46},// R
{0x46, 0x49, 0x49, 0x49, 0x31},// S
{0x01, 0x01, 0x7F, 0x01, 0x01},// T
{0x3F, 0x40, 0x40, 0x40, 0x3F},// U
{0x1F, 0x20, 0x40, 0x20, 0x1F},// V
{0x7F, 0x20, 0x18, 0x20, 0x7F},// W
{0x63, 0x14, 0x08, 0x14, 0x63},// X
{0x03, 0x04, 0x78, 0x04, 0x03},// Y
{0x61, 0x51, 0x49, 0x45, 0x43},// Z
{0x00, 0x00, 0x7F, 0x41, 0x41},// [
{0x02, 0x04, 0x08, 0x10, 0x20},// "\"
{0x41, 0x41, 0x7F, 0x00, 0x00},// ]
{0x04, 0x02, 0x01, 0x02, 0x04},// ^
{0x40, 0x40, 0x40, 0x40, 0x40},// _
{0x00, 0x01, 0x02, 0x04, 0x00},// `
{0x20, 0x54, 0x54, 0x54, 0x78},// a
{0x7F, 0x48, 0x44, 0x44, 0x38},// b
{0x38, 0x44, 0x44, 0x44, 0x20},// c
{0x38, 0x44, 0x44, 0x48, 0x7F},// d
{0x38, 0x54, 0x54, 0x54, 0x18},// e
{0x08, 0x7E, 0x09, 0x01, 0x02},// f
{0x08, 0x14, 0x54, 0x54, 0x3C},// g
{0x7F, 0x08, 0x04, 0x04, 0x78},// h
{0x00, 0x44, 0x7D, 0x40, 0x00},// i
{0x20, 0x40, 0x44, 0x3D, 0x00},// j
{0x00, 0x7F, 0x10, 0x28, 0x44},// k
{0x00, 0x41, 0x7F, 0x40, 0x00},// l
{0x7C, 0x04, 0x18, 0x04, 0x78},// m
{0x7C, 0x08, 0x04, 0x04, 0x78},// n
{0x38, 0x44, 0x44, 0x44, 0x38},// o
{0x7C, 0x14, 0x14, 0x14, 0x08},// p
{0x08, 0x14, 0x14, 0x18, 0x7C},// q
{0x7C, 0x08, 0x04, 0x04, 0x08},// r
{0x48, 0x54, 0x54, 0x54, 0x20},// s
{0x04, 0x3F, 0x44, 0x40, 0x20},// t
{0x3C, 0x40, 0x40, 0x20, 0x7C},// u
{0x1C, 0x20, 0x40, 0x20, 0x1C},// v
{0x3C, 0x40, 0x30, 0x40, 0x3C},// w
{0x44, 0x28, 0x10, 0x28, 0x44},// x
{0x0C, 0x50, 0x50, 0x50, 0x3C},// y
{0x44, 0x64, 0x54, 0x4C, 0x44},// z
{0x00, 0x08, 0x36, 0x41, 0x00},// {
{0x00, 0x00, 0x7F, 0x00, 0x00},// |
{0x00, 0x41, 0x36, 0x08, 0x00},// }
{0x08, 0x08, 0x2A, 0x1C, 0x08},// ->
{0x08, 0x1C, 0x2A, 0x08, 0x08} // <-
};

//__code unsigned char numbers[13]={ 0x7e,0x30,0x6d,0x79,0x33,0x5b,0x5f,0x70,0x7f,0x7b,0x00,0xff,0x4f};

/*******************************
*
*
*			MAIN
*
*
********************************/
int main(void)
{
	unsigned int ladc;
	unsigned int cont, end;
	int valor, erro = 0;
	Init_Device();
	SFRPAGE=LEGACY_PAGE;
	Ini_LCDan();

	/* Inicializa glcd

	ini_glcd();
	limpa_glcd(DIR);
	limpa_glcd(ESQ);

	*/
	valor = 148;

	for (cont=0;cont<8192;cont++)
	{
		end = cont;
		esc_RAM_SPI(cont,valor);

		ladc = le_RAM_SPI(cont);
		if (ladc == valor)
		{
			printf_fast_f("Valor: %u",ladc);
			printf_fast_f("                              End: %u",cont);
		}
		else{
			erro = 1;
			break;
			}
		delay_ms(500);
		Ini_LCDan();
 	}

	if (erro ==1)  printf_fast_f(" ERROR End: %u",cont);
		else printf_fast_f(" Fim do teste");

	while(1);
}
/*******************************
*
*
*			DELAY
*
*
********************************/
void delay_ms(unsigned int t)  // conta t milisegundos
{
	TMOD = TMOD | 0x01;
	TMOD = TMOD & ~0x02;
	for(;t>0; t--)
	{
		TR0 = 0;
		TF0 = 0;
		TH0 = 0x9e;
		TL0 = 0x58;
		TR0 = 1;    // aciona o temporizador

		while(TF0 == 0);   // permanece no estado até que alcance o overflow
	}
}
void delay25_8ms(unsigned int t)// conta t milisegundos
{
	TMOD = TMOD | 0x01;
	TMOD = TMOD & ~0x02;
	for(;t>0; t--)
	{
		TR0 = 0;
		TF0 = 0;
		TH0 = 0xf3;
		TL0 = 0xcb;
		TR0 = 1;    // aciona o temporizador
		while(TF0 == 0);   // permanece no estado até que alcance o overflow
	}
}
void delay_us(unsigned int t)
{
	TMOD = TMOD | 0x01;
	TMOD = TMOD & ~0x02;
	for(;t>0; t--)
	{
		TR0 = 0;
		TF0 = 0;
		TH0 = 0xff;
		TL0 = 0xe7;
		TR0 = 1;
		while(TF0 == 0);
	}
}
/*******************************
*
*
*			MATRIX DE LEDS
*
*
********************************/
/*
void mostra_car2(unsigned char n,unsigned int t)
{
	if(n >= 48 && n <= 90)
	numberzero(n-48,t);
	else
	numberzero(43,t);

}
void numberzero(unsigned char n,unsigned int t)
{
	int p;
	for(p = 0; p < 4; p++)
	{
		SFRPAGE=LEGACY_PAGE;
		delay_ms(t);
		P0 = P1;
		P1 = P2;
		P2 = P3;
		SFRPAGE=CONFIG_PAGE;
		P3 = P4;
		P4 = P5;
		P5 = P6;
		P6 = P7;
		//P7 = caract[n][p];
		SFRPAGE=LEGACY_PAGE;
	}
}
void print_LEDS(unsigned char *pont,unsigned int t )
{
	int o;
	for(o = 0;pont[o] != '\0'; o++)
	{
		mostra_car2(pont[o],t);
	}
}
*/
/*******************************
*
*
*		DISPLAY 7 SEGMENTOS
*
*
********************************/
/*
void display7_point(unsigned int n,unsigned int pont)
{

//	P0 = numbers[n]; // recebe o numero correspondente no vetor number
	pont = pont<<7;  // coloca o bit pont na posicao correta para acionaar ou não o ponto
	P0 = P0 + pont;
}

void display7(unsigned int n,unsigned int p,unsigned int del)
{

	//P0 = numbers[n]; // recebe o numero correspondente no vetor number
	P1 = p;  		// 0x00 ambos ligados, 0x02 primeiro ligado, 0x01 segundo ligado, 0x03 ambos desligados
	delay25_8ms(del);
}


void funcao_isr(void) __interrupt 5
{
	TF2=0;
	if(muda_numero%2 == 0)
	{
		P1 = 0x01;  // LIGA DIREITA
		P0 = numbers[DIR];
	}else{
		P1 = 0x02;
		P0 = numbers[ESQ];
	}
	muda_numero++;
}


*/


/*******************************
*
*
*		DISPLAY ALFANUMÉRICO
*
*
********************************/

void esc_LCDan(unsigned char dado, __bit nb, __bit cd)
{

	unsigned char aux;
	RS = cd;
	NOP();
	E=1;
	aux=(dado>>2)&0xfc;
	aux = aux | 0x02 | cd;
	DB = aux;
	NOP();
	E=0;
	if(nb)
	{
		delay_us(1);
		E=1;
		aux=(dado<<2)&0xfc;
		aux = aux | 0x02 | cd;
		DB = aux;
		NOP();
		E=0;
	}
	if(dado<4 && cd==CO) delay_us(1520);
		else delay_us(43);
}

void Ini_LCDan(void)
{
	E=0;
	delay_ms(16);
	esc_LCDan(0x30,NI,CO);
	delay_ms(5);
	esc_LCDan(0x30,NI,CO);
	delay_us(101);
	esc_LCDan(0x30,NI,CO);
	esc_LCDan(0x20,NI,CO);
	esc_LCDan(0x28,BY,CO);
	esc_LCDan(0x08,BY,CO);
	esc_LCDan(0x01,BY,CO);
	esc_LCDan(0x06,BY,CO);
	esc_LCDan(0x0c,BY,CO);
}

void putchar(char c)
{
	esc_LCDan(c,BY,DA);
}

void desl_string(int t, __code char * p, unsigned char nd)
{
	int i;
	for(i = 0; i < nd; i++)
	{
		unsigned char dim = 0;
		esc_LCDan(0x01,BY,CO); // limpa display
		esc_LCDan(0x90,BY,CO); // limpa display
		printf_fast_f(p);

		while(p[dim] != 0)
			dim++;

		dim = dim + 16;

		esc_LCDan(0x02,BY,CO);  //Display na condição inicial
		for(; dim>0; dim--)
		{
			esc_LCDan(0x18,BY,CO); // Desloca um caractere
			delay_ms(t);
		}
	}
}

/*******************************
*
*
*			DISPLAY GLCD
*
*
********************************/
/*
unsigned char le_glcd( __bit cd, __bit cs)
{
	unsigned char byte;

	RW = 1;
	RS = cd;
	CS1 = cs;
	CS2 = !cs;
	NOP4();
	E = 1;
	NOP8();
	SFRPAGE=CONFIG_PAGE;
	byte = DB;
	SFRPAGE=LEGACY_PAGE;
	NOP4();
	E = 0;
	NOP12();
	return (byte);
}
void esc_glcd(unsigned char dado, __bit cd, __bit cs)
{
	while(le_glcd(CO,cs) & 0x80); // ler estado para verificar se pode escrever
	RW = 0;
	CS1 = cs;
	CS2 = !cs;
	RS = cd;
	SFRPAGE=CONFIG_PAGE;
	DB = dado;
	SFRPAGE=LEGACY_PAGE;
	NOP4();
	E = 1;
	NOP12();
	E = 0;
	SFRPAGE=CONFIG_PAGE;
	DB = 0xff;     // precisa fazer isso para o transistor ficar aberto para o DB ser modificado
	SFRPAGE=LEGACY_PAGE;
	NOP12();
}

void ini_glcd(void)
{
	E = 0;
	RST = 1;
	CS1 = 1;
	CS2 = 1;
	SFRPAGE=CONFIG_PAGE;
	DB = 0xff;
	SFRPAGE=LEGACY_PAGE;
	while(le_glcd(CO,ESQ) & 0x10);
	while(le_glcd(CO,DIR) & 0x10);
	esc_glcd(0x3f,CO,ESQ);
	esc_glcd(0x3f,CO,DIR);
	esc_glcd(0x40,CO,ESQ);
	esc_glcd(0x40,CO,DIR);
	esc_glcd(0xb8,CO,ESQ);
	esc_glcd(0xb8,CO,DIR);
	esc_glcd(0xc0,CO,ESQ);
	esc_glcd(0xc0,CO,DIR);
}

void conf_Y(unsigned char y, __bit cs)
{
	y &= 0x3f;
	esc_glcd(0x40|y, CO,cs);
}

void conf_pag(unsigned char pag, __bit cs)
{
	pag &= 0x07;
	esc_glcd(0xb8|pag, CO, cs);
}

void limpa_glcd(__bit cs)
{
	unsigned char i,j;
	for(i = 0; i<0x08;i++)
	{
		conf_pag(i,cs);
		conf_Y(0,cs);
		for (j=0; j<64;j++)
			esc_glcd(0x00,DA,cs);
	}
}
void putchar ( char c)
{
	__bit lado;
	static unsigned char cont_car=0;

	if (c<9)
	{
		conf_pag(c-1,ESQ);
		conf_pag(c-1,DIR);
		conf_Y(0,ESQ);
		conf_Y(0,DIR);
		cont_car=0;
	}
	else
	{
		if (cont_car<8) lado =ESQ;
			else lado =DIR;
		c = c-32;
		esc_glcd(fonte[c][0], DA, lado);
		esc_glcd(fonte[c][1], DA, lado);
		esc_glcd(fonte[c][2], DA, lado);
		esc_glcd(fonte[c][3], DA, lado);
		esc_glcd(fonte[c][4], DA, lado);
		esc_glcd(0x00, DA, lado);
		esc_glcd(0x00, DA, lado);
		esc_glcd(0x00, DA, lado);
		cont_car++;
	}
}
*/
/*******************************
*
*
*				P2
*
*
********************************/
void isr_ext0 () __interrupt 0
{
	C++;
	IE0 = 0; // Zera a flag de interupção
}

void isr_ext1 () __interrupt 2
{
/*************

	ORIGINAL

	delay_ms(20); // debounce das ocilações 1-0
	C++;
	while (P0_1==0); // enquanto a tecla esta prescionada
	delay_ms(20); // debounce das ocilações 0-1
	IE1 = 0;  // Zera a flag de interupção

*************/

	C++;
	EX1 = 0;	// desabilita a interrupção ext1 externa
	TR2 = 1;	// Liga TC2

}
/*void ISR_TC2 (void) __interrupt 5 // ORIGINAL
{
	static unsigned char ni =0; // conta numero de ints
	TF2 = 0; // zera a flag de overflow

	if (P0_1)
	{
		ni++;
		if (ni>=2)
		{
			ni = 0;
			TR2 = 0; 	// Desliga o TC2
			IE1 = 0; 	// Zera a flag de interrupção ext1
			EX1 = 1;	// Reabilita a interrupçãp ext1
		}
	}
}

void ISR_TC2 (void) __interrupt 5
{
	static unsigned char est_antes, est_atual;
	TF2 = 0; // zera a flag de overflow
	est_antes = est_atual;
	est_atual = le_teclas(P0,P1);
	if(est_antes == TECS_OPEN && est_atual != TECS_OPEN)
	{
		NUM = le_teclas(P0,P1);
	}
}

int le_teclas (int porta0, int porta1)
{
	int c1=255-porta0;
	int c2=255-porta1;
	int i = 0;
	while(i != 21)
	{
		c1=255-porta0;
		c2=255-porta1;
		switch(c1)
		{
			case 1:
				return 0;
				break;
			case 2:
				return 1;
				break;
			case 4:
				return 2;
				break;
			case 8:
				return 3;
				break;
			case 16:
				return 4;
				break;
			case 32:
				return 5;
				break;
			case 64:
				return 6;
				break;
			case 128:
				return 7;
				break;
		}
		switch(c2)
		{
			case 1:
				return 8;
				break;
			case 2:
				return 9;
				break;
			case 4:
				return 10;
				break;
			case 8:
				return 11;
				break;
			case 16:
				return 12;
				break;
			case 32:
				return 13;
				break;
			case 64:
				return 14;
				break;
			case 128:
				return 15;
				break;
			default:
				i = 21;
				return 21;
			}
		}
}
*/
void contarpulso()
{
	int cont=0;
	TF1 = 1;
	TH1 = 0x00;
	TL1 = 0x00;
	TR1 = 1;
	while(1)
	{
		cont = TH1 << 8;
		cont = cont + TL1;
		printf_fast_f("\x01 PULSOS: %d",cont);
		//delay_ms(20);
	}
}

float le_pulso(void)
{
	unsigned int  t_pulso;
	while(P0_0 == 1); // Espera enquanto P0_0 for 1 ( não precionado )

    cont = 0;
    IE1 = 0; 	// Zera a flag da int externa 1
    TR1 = 1; 	// Liga TC1 que contará só quando pino INT1=1
    while(IE1 == 0);	// Espera 1-0 no pino INT1 ( soltar )
    TR1 = 0;	// Desliga TC1
    TF1 = 0;	// Zera flag de overflow do TC1
    t_pulso = (unsigned int)TH1*256 + (unsigned int)TL1;
    TL1 = 0;    // Zera os valores iniciais
    TH1 = 0;    // Zera os valores iniciais
    cont = cont + (float)t_pulso/25000000;
    return (cont);
}

void int_tc1 (void) interrupt 3
{
	TF1 = 0;    // Zera flag de overflow do TC1
	cont = cont + 0.00262144; // tempo até o overflow
	if(cont>0.05) IE1 = 1; // era > 1 no experimento de rpm |

}

void velocidade()
{
	float  tempo , vel;
	tempo = le_pulso();
	vel = 3/tempo;
	printf_fast_f("\x01 T: %0.4f s",tempo);
	printf_fast_f("\x02 V:%0.2f RPM",vel);
	delay_ms(500);

}
unsigned int le_hcsr04(void)
{
	float  lar;

	while(1){
		P0_1 = 1; // pino Trig
		delay_us(10);
		P0_1 = 0; // Pino echo
		lar = le_pulso();
		lar = lar * 1000000; // converter de segundo para microsegundo
		printf_fast_f("\x01 LARGURA:");
		printf_fast_f("\x02 %0.1f us",lar);
		lar = lar / 58;
		printf_fast_f("\x04 DISTANCIA:");
		printf_fast_f("\x05 %0.1f cm",lar);
		delay_ms(100);
	}
}

void esc_DAC0 (float v)
{
	unsigned int code1;

	code1 = (4096*v)/2.43;

	DAC0L = code1;      // 8 bits menos significativos
	DAC0H = code1>>8;   // 4 bits mais significativos

}

unsigned int le_adc0(unsigned char canal,unsigned char ganho)
{
    unsigned int code1;

	ADC0CF = ADC0CF & 0xf8;  // zerar os 3 bits menos significativos
	ADC0CF = ADC0CF | ganho;

	AMX0SL = canal; // seleciona o canal de converção

	AD0BUSY = 1;    // INICIA CONVERSÃO AD
	while(AD0BUSY); // espera conversão terminar

	code1 = (ADC0H<<8 | ADC0L); // retorna 12 bits
	return (float)(code1*2.43)/4096;    // retorna 12 bits

}

void tocamusica(void) __interrupt 5
{
	DAC0L = 0;
//	DAC0H = amostras[C];
	C++;

	if(C>=30000)
		C = 0;

	TF2 = 0;
}

void esc_RAM_SPI (unsigned int end, unsigned char dado)
{
	unsigned char end_L, end_H;

	end_L = end;
	end_H = end>>8;
	CS = 0;

	SPI0DAT = 0x02;     // configura para escrita
	while(!TXBMT);      // espera até os bits serem transferidos ao shift register
	SPI0DAT = end_H;    // 8 bits mais significativos
	while(!TXBMT);
	SPI0DAT = end_L;    // 8 bits menos significativos
	while(!TXBMT);
	SPI0DAT = dado;     // escreve o dado na memória
	while(!TXBMT);
	SPIF = 0;           // zera a flag de interupção do SPI
	while(!SPIF);
	SPIF = 0;
	CS = 1;
}

unsigned char le_RAM_SPI (unsigned int end)
{
	unsigned char end_L, end_H;

	end_L = end;
	end_H = end>>8;
	CS = 0;

	SPI0DAT = 0x03;
	while(!TXBMT);
	SPI0DAT = end_H;
	while(!TXBMT);
	SPI0DAT = end_L;
	while(!TXBMT);
	SPI0DAT = 0x00;
	while(!TXBMT);
	SPIF = 0;
	while(!SPIF);
	SPIF = 0;
	CS = 1;
	return (SPI0DAT);
}
