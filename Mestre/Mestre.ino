#include <SoftwareSerial.h>

int botoes[] = { 9, 8, 7, 6, 5, 4, 3 };
int piezo = 2;
int potPin = A0;

float notas[] = { 261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88 };

SoftwareSerial ledSerial(10, 11); // RX, TX (apenas TX será usado)

void setup() {
	for (int i = 0; i < 7; i++) {
		pinMode(botoes[i], INPUT_PULLUP);
	}
	pinMode(piezo, OUTPUT);
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
