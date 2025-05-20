// ---------------------
// Bibliotecas
// ---------------------
#include <SoftwareSerial.h>  // Biblioteca para melhor controlo de serial communication

// ---------------------
// Definições de pinos
// ---------------------
int botoes[] = { 9, 8, 7, 6, 5, 4, 3 };  // Pinos dos botões
int piezo = 2;                           // Pino piezo buzzer
int potPin = A0;                         // Pino potenciómetro

// ---------------------
// Notas musicais
// ---------------------
float notas[] = {
	261.63,  // Dó
	293.66,  // Ré
	329.63,  // Mi
	349.23,  // Fá
	392.00,  // Sol
	440.00,  // Lá
	493.88   // Si — Iremos remover futuramente porque só iremos ter 6 teclas
};

// ---------------------
// Comunicação serial com o escravo (NeoPixel)
// ---------------------
SoftwareSerial ledSerial(10, 11);  // RX, TX (Sendo o mestre, só usa TX, ou seja, transmite)

// ---------------------
// Setup
// ---------------------
void setup() {

	// Setup dos botões usando as resistências internas
	for (int i = 0; i < 7; i++) {
		pinMode(botoes[i], INPUT_PULLUP);
	}

	// Buzzer
	pinMode(piezo, OUTPUT);

	// Setup de Serial Communication
	ledSerial.begin(9600);
	Serial.begin(9600);
}

// ---------------------
// Loop
// ---------------------
void loop() {
	bool notaTocada = false;

	// Se algum dos botões for pressionar, executa o seguinte código
	for (int i = 0; i < 7; i++) {
		if (digitalRead(botoes[i]) == LOW) {
			// Leitura do potenciómetro
			float pot = analogRead(potPin);                       // Valor entre 0–1023

			// Mapear o valor do potenciómetro para um pitch entre 50% a 150%
			float pitchFactor = map(pot, 0, 1023, 50, 150);

			// Ajustar frequência da nota (o som fica baixo sem isto)
			float frequencia = notas[i] * (pitchFactor / 100.0);

			// Toca a nota
			tone(piezo, frequencia, 250);  // Toca nota ajustada por 250ms

			// Enviar comandos via Serial Communication, dependendo do botão
			if (i == 0) {  								// Primeiro botão envia comando para ligar a fita Neopixel
				ledSerial.println("LED1");
			} else if (i == 1) {  				// Segundo botão envia comando para ligar o LED normal (não temos mais fitas neopixel por enquanto)
				ledSerial.println("LED2");
			}

			notaTocada = true;
			break;
		}
	}

	// Se o botão não está a ser tocado, desliga o som do piezo buzzer
	if (!notaTocada) {
		noTone(piezo);
	}

	delay(10);
}
