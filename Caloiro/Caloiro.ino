// ---------------------
// Bibliotecas
// ---------------------
#include <Adafruit_NeoPixel.h>  // Biblioteca para controlar neopixel
#include <SoftwareSerial.h>     // Biblioteca para melhor controlo de serial communication

// ---------------------
// Definições de pinos
// ---------------------
#define LED_PIN 5   // Fita LED
#define LED_PIN2 4  // Fita LED 2
#define NUM_LEDS 5  // Número de leds das fitas
#define TRIG_PIN 2  // Trigger ultrassonico
#define ECHO_PIN 3  // Echo ultrassonico

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

// ---------------------
// Inicializações globais
// ---------------------
SoftwareSerial mySerial(8, 7);                                       // Iniciar a Serial Communication via SoftwareSerial com pinos digitais
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);    // Iniciar a fita LED
Adafruit_NeoPixel strip2(NUM_LEDS, LED_PIN2, NEO_GRB + NEO_KHZ800);  // Segunda fita LED


// Variáveis de animação dos LEDs
bool animationRunning = false;  // Verifica se a animação está a correr
int position = 0;               // Posição dos leds da animação

// ---------------------
// Setup
// ---------------------
void setup() {

  // Sensor ultrassónico
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Fitas LED
  strip.begin();
  strip.setBrightness(26);  // Brilho default
  strip.show();

  strip2.begin();
  strip2.setBrightness(26);
  strip2.show();

  // Serial Communication
  mySerial.begin(9600);
  Serial.begin(9600);
}

// ---------------------
// Loop
// ---------------------
void loop() {

  // Se receber o comando do Mestre, executa este código
  if (mySerial.available()) {
    String comando = mySerial.readStringUntil('\n');
    comando.trim();
    Serial.println("Recebido: [" + comando + "]");  // Debug para ver se está o Escravo está a receber os dados do Mestre

    // Se recebe o comando NEO (neopixel) e a animação não estiver a correr, executa este código
    if (comando == "LED1" && !animationRunning) {
      animationRunning = true;
      position = 0;
    } else if (comando == "LED2" && !animationRunning) {  // Se receber o comando LED do outro botão, acende o LED normal
      animationRunning = true;
      position = 5;
    }
  }

  // Se a animação LED estiver a correr, executa o seguinte
  if (animationRunning) {
    strip.clear();  // Limpa os LEDs

    // Acende os 3 primeiros LEDs
    for (int i = 0; i < 3; i++) {
      int ledIndex = position + i;
      if (ledIndex < NUM_LEDS) {
        strip.setPixelColor(ledIndex, strip.Color(255, 0, 155));  // Cor rosa
      }
    }

    // Medir distância e ajustar brilho
    long distance = readDistanceCM();

    // Debugging
    Serial.print("Distância: ");
    Serial.print(distance);
    Serial.println(" cm");

    //Mapear 5-100cm → 10-255 brilho
    int brightness = map(constrain(distance, 5, 100), 5, 100, 0, 255);
    strip.setBrightness(brightness);

    // Atualiza os LEDs
    strip.show();
    delay(50);
    position++;

    // Termina a animação quando atinge o fim da fita
    if (position > NUM_LEDS) {
      animationRunning = false;
    }
  }
}
