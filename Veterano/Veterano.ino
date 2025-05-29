#include <SoftwareSerial.h>

// ---------------------
// Definições de pinos
// ---------------------
int botoes[] = { 9, 8, 7, 6, 5, 4 };
int corButton = 3;
int piezo = 2;
int potPin = A0;

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
};

// ---------------------
// Serial communication
// ---------------------
SoftwareSerial ledSerial(10, 11);  // RX, TX

// ---------------------
// Variáveis
// ---------------------
bool corEstadoAtual = HIGH;
bool corEstadoAnterior = HIGH;

// Happy birthdayyyy tooo youuuuuuuuuuuuuuuuuuuu
int sequenciaAlvo[] = { 0, 0, 1, 0, 3, 2 };
int indiceSequencia = 0;
bool tocarMusica = false;

// Modo eco
unsigned long ultimoToque = 0;
const unsigned long tempoMaxEntreToques = 3000;
unsigned long debounceAnterior[6] = { 0 };
const unsigned long tempoDebounce = 200;
bool estadoAnteriorBotoes[6] = { HIGH, HIGH, HIGH, HIGH, HIGH, HIGH };


// Idle
const unsigned long tempoIdle = 30000;  // tempo sem interação (30s)
int ultimosBotoes[10];                  // buffer circular
int posBuffer = 0;											// posição do inicial do buffer
bool bufferCheio = false;

// ---------------------
// Setup
// ---------------------
void setup() {
	for (int i = 0; i < 6; i++) {
		pinMode(botoes[i], INPUT_PULLUP);
	}

	pinMode(corButton, INPUT_PULLUP);
	pinMode(piezo, OUTPUT);

	ledSerial.begin(9600);
	Serial.begin(9600);
}

// ---------------------
// Loop
// ---------------------
void loop() {
	unsigned long agora = millis();

	if (tocarMusica) {
		tocarHappyBirthday();
		tocarMusica = false;
		return;
	}

	// Verifica se está idle durante 30sec
	if (agora - ultimoToque >= tempoIdle && (posBuffer > 0 || bufferCheio)) {
		tocarUltimasNotas();
		ultimoToque = agora;  // evitar repetição contínua
		return;
	}

	bool algumBotaoPressionado = false;

	for (int i = 0; i < 6; i++) {
		bool estadoAtual = digitalRead(botoes[i]);

		if (estadoAnteriorBotoes[i] == HIGH && estadoAtual == LOW) {
			if (agora - debounceAnterior[i] > tempoDebounce) {
				debounceAnterior[i] = agora;

				ultimoToque = agora;  // atualizar atividade

				// Adicionar botão ao buffer
				ultimosBotoes[posBuffer] = i;
				posBuffer = (posBuffer + 1) % 10;
				if (posBuffer == 0) bufferCheio = true;

				// Lógica da sequência
				if (agora - ultimoToque > tempoMaxEntreToques) {
					indiceSequencia = 0;
				}

				if (i == sequenciaAlvo[indiceSequencia]) {
					indiceSequencia++;
					if (indiceSequencia >= 6) {
						Serial.println("Happy birthdayyyy tooo youuuuuu");
						tocarMusica = true;
						indiceSequencia = 0;
						return;
					}
				} else {
					indiceSequencia = 0;
				}
			}
		}

		if (estadoAtual == LOW) {
			algumBotaoPressionado = true;

			float pot = analogRead(potPin);
			float pitchFactor = map(pot, 0, 1023, 50, 150);
			float frequencia = notas[i] * (pitchFactor / 100.0);
			tone(piezo, frequencia);

			if (estadoAnteriorBotoes[i] == HIGH) {
				ledSerial.println("LED" + String(i + 1));
			}
		}

		if (estadoAnteriorBotoes[i] == LOW && estadoAtual == HIGH) {
			noTone(piezo);
		}

		estadoAnteriorBotoes[i] = estadoAtual;
	}

	// Botão cor
	corEstadoAtual = digitalRead(corButton);
	if (corEstadoAnterior == HIGH && corEstadoAtual == LOW) {
		ledSerial.println("cor");
	}
	corEstadoAnterior = corEstadoAtual;

	if (!algumBotaoPressionado) {
		noTone(piezo);
	}

	delay(10);
}

// ---------------------
// Happy birthdayyyy!!
// ---------------------
void tocarHappyBirthday() {
	int melodia[] = {
		264, 264, 297, 264, 352, 330,
		264, 264, 297, 264, 396, 352,
		264, 264, 528, 440, 352, 330, 297,
		466, 466, 440, 352, 396, 352
	};

	int duracoes[] = {
		250, 250, 500, 500, 500, 1000,
		250, 250, 500, 500, 500, 1000,
		250, 250, 500, 500, 500, 500, 1000,
		250, 250, 500, 500, 500, 1000
	};

	for (int i = 0; i < 25; i++) {
		tone(piezo, melodia[i], duracoes[i]);
		delay(duracoes[i] * 1.3);
	}

	noTone(piezo);
}

// -----------------------------------
// Modo eco (toca as ultimas 10 notas)
// -----------------------------------
void tocarUltimasNotas() {
  int totalNotas = bufferCheio ? 10 : posBuffer;
  int idx = bufferCheio ? posBuffer : 0;

  for (int i = 0; i < totalNotas; i++) {
    int notaIdx = ultimosBotoes[(idx + i) % 10];
    float pot = analogRead(potPin);
    float pitchFactor = map(pot, 0, 1023, 50, 150);
    float frequencia = notas[notaIdx] * (pitchFactor / 100.0);
    tone(piezo, frequencia, 300);

    // Ativar LEDs
    ledSerial.println("LED" + String(notaIdx + 1));

    delay(350);
  }

  noTone(piezo);
}