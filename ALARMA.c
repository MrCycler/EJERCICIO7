#include"tm4c123gh6pm.h"
#include"stdint.h"

void config_uart0 (void){
	unsigned long temp;
	SYSCTL_RCGC1_R |= SYSCTL_RCGC1_UART0; // Se activa el reloj del UART
	temp = SYSCTL_RCGC2_R; // Espera de unos ciclos de reloj
	
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOA; // Se activa el reloj del puerto A
	temp = SYSCTL_RCGC2_R; // Espera de unos ciclos de reloj

	//comunicacion serial 9600 bps
	UART0_CTL_R &= ~ UART_CTL_UARTEN; // Se desactiva el UART
	UART0_IBRD_R = (UART0_IBRD_R & ~UART_IBRD_DIVINT_M)|104; // Se configura DIVINT 16MHz/(16*9600) Parte entera
	UART0_FBRD_R = (UART0_FBRD_R & ~UART_FBRD_DIVFRAC_M)|10; //Se configura DIVFRAC Parte fraccionaria*64
	UART0_LCRH_R = ((UART0_LCRH_R & ~0x000000FF)|(UART_LCRH_WLEN_8)|(UART_LCRH_FEN)); // Se configuran los bits de datos, 1 bit de parada, sin paridad y habilita el FIFO
	UART0_CTL_R |= UART_CTL_UARTEN; // Se habilita el UART
	GPIO_PORTA_AFSEL_R |= 0x03; // Se activan las funciones alternas de PA0 y PA1.
	GPIO_PORTA_DEN_R |= 0x03; // Habilitaci�n PA0 y PA1 para se�ales digitales.
}

void txcar_uart0(uint32_t car){
	while ((UART0_FR_R & UART_FR_TXFF)!=0); //Espera que est� disponible para transmitir
	UART0_DR_R = car;
}

void txmens_uart0(uint8_t mens[]){
	uint8_t letra;
	uint8_t i=0;
	letra= mens[i++];
	while (letra != '\0'){ //Se env�an todos los caracteres hasta el fin de cadena
	txcar_uart0(letra);
	letra= mens[i++];
	}
}

uint8_t rxcar_uart0(void){
	uint8_t temp;
	while ((UART0_FR_R & UART_FR_RXFE)!=0); // Se espera que llegue un dato
	temp= UART0_DR_R&0xFF; // Se toman solo 8 bits
	return temp;
}

void config_sw1(void){
	// activamos la se�al de reloj del puerto F
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5;
	// esperamos a que realmente se active
	while( (SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R5)==0) { }
	// SW1 est� conectado al pin PF4
	GPIO_PORTF_DIR_R &= ~(0x10); // PF4 pin de entrada
	GPIO_PORTF_AFSEL_R &= ~(0x10); // no usamos funci�n alternativa
	GPIO_PORTF_PUR_R |= 0x10; // activamos resistencia de pull-up en pin PF4
	GPIO_PORTF_DEN_R |= 0x10; // PF4 pin digital
}

void config_leds(void){
	// activamos la se�al de reloj del puerto F
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5;
	// esperamos a que realmente se active
	while( (SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R5)==0) { }
	GPIO_PORTF_DIR_R |= 0x0E; // Configura el bit 1 del puerto F como salida
	GPIO_PORTF_DR8R_R |=0x0E; // se activa el driver de 8 mA en el pin 1.
	GPIO_PORTF_DEN_R |=0x0E; // se activa el pin 1, como salida digital.
	GPIO_PORTF_DATA_R &= ~(0X0E); // apagamos el led
}

void main(void){
	
	config_leds();
	config_sw1();
	config_uart0(); // Se configura el UART
	uint8_t mensaje[]= "HA ENTRADO UN LADRON A TU CASA";
	

	while(1){ // Se esperan los aciertos de la longitud de la palabra
		
		if ((GPIO_PORTF_DATA_R & 0x10) == 0) // si SW1(conectado a PF4) est� presionado
		{
			txmens_uart0(mensaje);
			GPIO_PORTF_DATA_R |= 0x02;
			while((GPIO_PORTF_DATA_R&0x10)==0x00);
		}	
		else // si SW1 no est� presionado
		{	
			GPIO_PORTF_DATA_R &= ~0x02; // apagamos el led
		}
		
	}

}