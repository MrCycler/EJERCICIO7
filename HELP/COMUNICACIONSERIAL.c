
//CONFIGURA LA COMUNICACION SERIAL
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
	GPIO_PORTA_DEN_R |= 0x03; // Habilitación PA0 y PA1 para señales digitales.
}

//ENVIA UN CARACTER
void txcar_uart0(uint32_t car){
	while ((UART0_FR_R & UART_FR_TXFF)!=0); //Espera que esté disponible para transmitir
	UART0_DR_R = car;
}

//ENVIA UN ARREGLO DE CARACTERES
void txmens_uart0(uint8_t mens[]){
	uint8_t letra;
	uint8_t i=0;
	letra= mens[i++];
	while (letra != '\0'){ //Se envían todos los caracteres hasta el fin de cadena
	txcar_uart0(letra);
	letra= mens[i++];
	}
}

//RECIBE UN CARACTER
uint8_t rxcar_uart0(void){
	uint8_t temp;
	while ((UART0_FR_R & UART_FR_RXFE)!=0); // Se espera que llegue un dato
	temp= UART0_DR_R&0xFF; // Se toman solo 8 bits
	return temp;
}