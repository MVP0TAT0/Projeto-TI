// ---------------------
// Bibliotecas
// ---------------------
#include <Adafruit_NeoPixel.h>  // Biblioteca para controlar neopixel
#include <SoftwareSerial.h>     // Biblioteca para melhor controlo de serial communication

// ---------------------
// Definições de pinos
// ---------------------
#define LED_PIN 5    // Fita LED 1
#define LED_PIN2 4   // Fita LED 2
#define LED_PIN3 6   // Fita LED 3
#define LED_PIN4 9   // Fita LED 4
#define LED_PIN5 12  // Fita LED 5
#define LED_PIN6 13  // Fita LED 6
#define NUM_LEDS 5   // Número de leds das fitas
#define TRIG_PIN 2   // Trigger ultrassonico
#define ECHO_PIN 3   // Echo ultrassonico

// ---------------------
// Medir distância com o sensor ultrassónico
// ---------------------
long readDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  long distance = duration * 0.034 / 2;
  return distance;
}

// -----------------------------------
// Inicializações globais e variáveis
// -----------------------------------
SoftwareSerial mySerial(8, 7);  // Iniciar a Serial Communication via SoftwareSerial com pinos digitais

// Definir pins de LEDs e numero de LED strips
#define NUM_STRIPS 6
int ledPins[NUM_STRIPS] = { 5, 4, 6, 9, 12, 13 };  // LED_PIN1, LED_PIN2, etc

// Animação LEDs
Adafruit_NeoPixel* strips[NUM_STRIPS];
bool animationRunning[NUM_STRIPS] = { false };
int position[NUM_STRIPS] = { 0 };
unsigned long ultimoUpdate[NUM_STRIPS] = { 0 };
const unsigned long intervaloAnimacao = 50;

// Modo de cores
bool modoBranco = false;
unsigned long ultimaMudancaModo = 0;
const unsigned long debounceTempo = 300;  // tempo mínimo entre mudanças


// ---------------------
// Setup
// ---------------------
void setup() {

  // Sensor ultrassónico
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  for (int i = 0; i < NUM_STRIPS; i++) {
    strips[i] = new Adafruit_NeoPixel(NUM_LEDS, ledPins[i], NEO_GRB + NEO_KHZ800);
    strips[i]->begin();
    strips[i]->setBrightness(26);
    strips[i]->show();
  }

  // Serial Communication
  mySerial.begin(9600);
  Serial.begin(9600);
}

// ---------------------
// Loop
// ---------------------
void loop() {
	// Leitura de comandos do mestre
	if (mySerial.available()) {
		String comando = mySerial.readStringUntil('\n');
		comando.trim();
		Serial.println("Recebido: [" + comando + "]");

		// Alternar modo de cor
		if (comando == "cor" && millis() - ultimaMudancaModo > debounceTempo) {
			modoBranco = !modoBranco;
			ultimaMudancaModo = millis();
		}

		// Iniciar animação correspondente ao comando
		for (int i = 0; i < NUM_STRIPS; i++) {
			String esperado = "LED" + String(i + 1);
			if (comando == esperado && !animationRunning[i]) {
				animationRunning[i] = true;
				position[i] = 0;
			}
		}
	}

	// Lê a distância do sensor e define o brightness
	long distance = readDistanceCM();
	int brightness = map(constrain(distance, 5, 100), 5, 100, 0, 255);

	// Atualizar animações
	for (int i = 0; i < NUM_STRIPS; i++) {
		if (animationRunning[i]) {
			if (millis() - ultimoUpdate[i] >= intervaloAnimacao) {
				ultimoUpdate[i] = millis();

				strips[i]->clear();

				for (int j = 0; j < 3; j++) {
					int ledIndex = position[i] + j;
					if (ledIndex < NUM_LEDS) {
						uint32_t color;
						if (modoBranco) {
							color = strips[i]->Color(255, 255, 255);
						} else {
							switch (i) {
								case 0: color = strips[i]->Color(255, 0, 0); break;
								case 1: color = strips[i]->Color(255, 80, 0); break;
								case 2: color = strips[i]->Color(255, 255, 0); break;
								case 3: color = strips[i]->Color(0, 255, 0); break;
								case 4: color = strips[i]->Color(0, 0, 255); break;
								case 5: color = strips[i]->Color(128, 0, 255); break;
								default: color = strips[i]->Color(255, 255, 255); break;
							}
						}
						strips[i]->setPixelColor(ledIndex, color);
					}
				}

				strips[i]->setBrightness(brightness);
				strips[i]->show();
				position[i]++;

				if (position[i] > NUM_LEDS) {
					animationRunning[i] = false;
				}
			}
		}
	}
}
