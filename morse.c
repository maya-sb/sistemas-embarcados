// Definições de E / S
#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
// Atraso de tempo
#include <util/delay.h>
// Definição do clock do sistema
#define F_CPU 16000000UL
// Definição da taxa de transmissão
#define BAUD 9600
// Biblioteca auxiliar para comunicação serial
#include <util/setbaud.h>
// Procedimento de inicialização da UART 0
void inicializar_uart0(){
    // Configurando taxa de transmissão
    UBRR0H = UBRRH_VALUE ;
    UBRR0L = UBRRL_VALUE ;
    // Habilitando recepção e transmissão
    UCSR0B = (1 << RXEN0 ) | (1 << TXEN0 ) ;
    // Modo assíncrono e sem paridade
    // Quadro com 8 bits com 1 bit de parada
    UCSR0C = (1 << UCSZ01 ) | (1 << UCSZ00 ) ;
}

// Função para envio de dado pela UART 0
int enviar_dado_uart0(char dado, FILE* fluxo){
    // Checando por quebra de linha
    if (dado == '\n') {
        enviar_dado_uart0('\r', fluxo) ;
    }
    // Esperando por envio pendente
    while(!(UCSR0A & (1 << UDRE0))) ;
    // Enviando dado
    UDR0 = dado;
    // Retornando ok
    return 0;
}

// Função para recebimento de dado pela UART 0
int receber_dado_uart0(FILE* fluxo) {
    // Esperando por recebimento
    while(!(UCSR0A & (1 << RXC0)));
    // Retornando registrador de dado
    return UDR0;
}

void ponto(){
    // Ligando o LED
	PORTB = PORTB | (1 << PORTB5);
    // Atraso de 0.5 segundo
	_delay_ms(500);
    // Desligando o LED
	PORTB = PORTB & ~(1 << PORTB5);
    // Atraso de 0.5 segundo
    _delay_ms(500);
}

void traco(){
    // Ligando o LED
	PORTB = PORTB | (1 << PORTB5);
    // Atraso de 1.5 segundos
	_delay_ms(1500);
    // Desligando o LED
	PORTB = PORTB & ~(1 << PORTB5);
    // Atraso de 0.5 segundo
    _delay_ms(500);
}

int upperAscii(char letra){
    int asc = letra;
    int novoAsc = asc;

    //Verificando se é uma letra minúscula
    if (asc >= 97 && asc <= 122){
        //Tornando-a maiúscula
        novoAsc = (asc - 97) + 65;
    }
    //Verificando se está fora do range suportado pelo morse
    if (novoAsc < 33 || novoAsc > 95){
        //Posição para gerar saída vazia
        novoAsc = 94;
    }
    return novoAsc;
}

void codificar(int asc){

    // !"x$x&'()xx,-./0123456789:;x=x?@abcdefghijklmnopqrstuvwxyzxxxx_    
    char caracteres[63][7] = {"tptptt", "ptpptp", " ", "ppptppt", " ", "ptppp", "pttttp", "tpttp", "tpttpt", " ", " ", "ttpptt", "tppppt", "ptptpt", "tpptp",
    "ttttt", "ptttt", "ppttt", "ppptt", "ppppt", "ppppp", "tpppp", "ttppp", "tttpp", "ttttp", 
    "tttppp", "tptptp", " ", "tpppt", " ", "ppttpp", "pttptp", 
    "pt", "tppp", "tptp", "tpp", "p", "pptp", "ttp", "pppp", "pp", "pttt", "tpt", "ptpp", "tt", 
    "tp", "ttt", "pttp", "ttpt", "ptp", "ppp", "t", "ppt", "pppt", "ptt", "tppt", "tptt", "ttpp", 
    " ", " ", " ", " ", "ppttpt"}; 
    
    char codigo;
    int posicao = asc - 33;
    int i;

    for (i=0; i<strlen(caracteres[posicao]);i++){
        codigo = caracteres[posicao][i];
        switch (codigo)
        {
        case 't':
            traco();
            break;
        case 'p':
            ponto();
            break;
        default:
            //Espaço
            _delay_ms(2500);
            break;
        }
    }
}

// Criando fluxos de E/S para serial
FILE stdin_uart0 = FDEV_SETUP_STREAM(NULL, receber_dado_uart0, _FDEV_SETUP_READ);
FILE stdout_uart0 = FDEV_SETUP_STREAM(enviar_dado_uart0, NULL, _FDEV_SETUP_WRITE);

// Caracteres de texto
char texto [100] = {0};

// Função principal
int main () {
    // Ajustando pino como saida
    DDRB = DDRB | (1 << DDB5);
    // Inicializando UART 0
    inicializar_uart0();
    // Reconfigu rando E/S padrão
    stdin = &stdin_uart0;
    stdout = &stdout_uart0;

    int asc; 
    int tamanho;

    // Laço infinito
    while (1) {
        // Recebendo  dados
        gets(texto);
        int i;
        tamanho = strlen(texto) - 1;
        for (i=0; i<tamanho; i++){
            asc = upperAscii(texto[i]);
            //printf("%d\n", asc);
            codificar(asc);
            //Espaçamento entre letras
            _delay_ms(1000);
        }
    }
}
