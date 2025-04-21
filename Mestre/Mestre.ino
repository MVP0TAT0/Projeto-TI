#include <SoftwareSerial.h>	// Biblioteca para melhor controlo de serial communication

int botoes[] = { 9, 8, 7, 6, 5, 4, 3 };	// Pinos dos botões
int piezo = 2;													// Pino piezo buzzer
int potPin = A0;												// Pino potenciómetro

float notas[] = { 261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88 };	// Frequências das notas

SoftwareSerial ledSerial(10, 11); // RX, TX (Sendo o mestre, só usa TX)

void setup() {

	// Setup dos botões usando as resistências interiores
	for (int i = 0; i < 7; i++) {
		pinMode(botoes[i], INPUT_PULLUP);
	}
	
	pinMode(piezo, OUTPUT);

	// Setup de Serial Communication
	ledSerial.begin(9600);
	Serial.begin(9600);
}

void loop() {
	bool notaTocada = false;

	for (int i = 0; i < 7; i++) {
		if (digitalRead(botoes[i]) == LOW) {
			float pot = analogRead(potPin); // Valor entre 0–1023
			float pitchFactor = map(pot, 0, 1023, 50, 150); // Fator entre 50% e 150%
			float frequencia = notas[i] * (pitchFactor / 100.0); // Ajuste da nota base

			tone(piezo, frequencia, 250); // Toca nota ajustada por 250ms

			if (i == 0) { // Botão 9 → NEO
				ledSerial.println("NEO");
			} else if (i == 1) { // Botão 8 → LED
				ledSerial.println("LED");
			}

			notaTocada = true;
			break;
		}
	}

	if (!notaTocada) {
		noTone(piezo);
	}

	delay(10);
}
