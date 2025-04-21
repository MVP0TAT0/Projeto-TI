#include <Adafruit_NeoPixel.h>  // Biblioteca para controlar neopixel
#include <SoftwareSerial.h>     // Biblioteca para melhor controlo de serial communication

#define LED_PIN 5     // Fita LED
#define NUM_LEDS 8    // Número de Leds da fita
#define LED_NORMAL 4  // LED normal no pino 4 (para simular outra fita que nao temos ainda)
#define TRIG_PIN 2    // Trigger ultrassonico
#define ECHO_PIN 3    // Echo ultrassonico

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


SoftwareSerial mySerial(8, 7);  // RX, TX
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

bool animationRunning = false;  // Verifica se a animação está a correr
int position = 0;               // Posição dos leds da animação

void setup() {
  pinMode(LED_NORMAL, OUTPUT);  // LED normal
  digitalWrite(LED_NORMAL, LOW);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  strip.begin();
  strip.setBrightness(26);  // Brilho default
  strip.show();

  mySerial.begin(9600);
  Serial.begin(9600);
}

void loop() {
  if (mySerial.available()) {
    String comando = mySerial.readStringUntil('\n');
    comando.trim();
    Serial.println("Recebido: [" + comando + "]");  // Debug para ver se está o Escravo está a receber os dados do Mestre

    if (comando == "NEO" && !animationRunning) {
      animationRunning = true;
      position = 0;
    } else if (comando == "LED") {
      digitalWrite(LED_NORMAL, HIGH);
      delay(10);  // LED acende por 0.5s
      digitalWrite(LED_NORMAL, LOW);
    }
  }

  if (animationRunning) {
    strip.clear();

    for (int i = 0; i < 3; i++) {
      int ledIndex = position + i;
      if (ledIndex < NUM_LEDS) {
        strip.setPixelColor(ledIndex, strip.Color(255, 0, 155));
      }
    }

    // Medir distância e ajustar brilho
    long distance = readDistanceCM();
    Serial.print("Distância: ");
    Serial.print(distance);
    Serial.println(" cm");
    int brightness = map(constrain(distance, 5, 100), 5, 100, 0, 255);  // Mapear 5-100cm → 10-255 brilho
    strip.setBrightness(brightness);

    strip.show();
    delay(50);
    position++;

    if (position > NUM_LEDS) {
      animationRunning = false;
    }
  }
}
