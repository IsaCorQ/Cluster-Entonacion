#include <LedControl.h>

// Pines de conexión del MAX7219
const int DIN_PIN = 12;
const int CLK_PIN = 10;
const int CS_PIN = 11;
int ledPin = 13; 

// Pin del buzzer
const int BUZZER_PIN = 9;

// Crear objeto LedControl
LedControl lc = LedControl(DIN_PIN, CLK_PIN, CS_PIN, 1);

// Definición de patrones de letras
byte A[8] = {
  B00111000,
  B01000100,
  B01000100,
  B01111100,
  B01000100,
  B01000100,
  B01000100,
  B00000000
};

byte G[8] = {
  B00111100,
  B01000010,
  B00000010,
  B01110010,
  B01000010,
  B01000010,
  B00111100,
  B00000000
};

byte E[8] = {
  B01111110,
  B00000010,
  B00000010,
  B00111110,
  B00000010,
  B00000010,
  B01111110,
  B00000000
};

byte S[8] = {
  B00111100,
  B01000010,
  B00000010,
  B00111100,
  B01000000,
  B01000010,
  B00111100,
  B00000000
};

byte M[8] = {
  B01000010,
  B01100110,
  B01011010,
  B01000010,
  B01000010,
  B01000010,
  B01000010,
  B00000000
};

// Definir frecuencias para sonidos
const int FREQ_DU = 200;  // frecuencia más grave
const int FREQ_DA = 500;  // frecuencia más aguda
const int DURATION = 500; // duración de cada sonido en milisegundos (2 segundos)

void setup() {
  Serial.begin(9600); // Match the baud rate with your PC program
  while (!Serial) {
    // Wait for the Serial port to initialize
  }
  Serial.println("Arduino Ready");
  // Inicializar el MAX7219
  lc.shutdown(0, false);     // Activar el dispositivo
  lc.setIntensity(0, 8);     // Establecer brillo (0-15)
  lc.clearDisplay(0);        // Limpiar la pantalla

  pinMode(ledPin, OUTPUT);

  // Configurar pin del buzzer
  pinMode(BUZZER_PIN, OUTPUT);
}

void playSequenceWithSpaces(int* sequence, int length) {
  for (int i = 0; i < length; i++) {
    // Tocar nota
    tone(BUZZER_PIN, sequence[i], DURATION);
    delay(DURATION);
    noTone(BUZZER_PIN);
    
    // Espacio en silencio
    delay(DURATION);
  }
}

void loop() {
  if (Serial.available() > 0) {
    char received = Serial.read(); // Read the received character
    Serial.print("Received: ");
    Serial.println(received);     // Echo it back to the sender
    
    if (received == 'a') {
      displayLetter(A);
      Serial.println("Mostrando letra A");
      digitalWrite(ledPin, HIGH);
      int sequenceA[] = {FREQ_DU, FREQ_DU, FREQ_DU, FREQ_DA};
      playSequenceWithSpaces(sequenceA, 4);
      delay(1000); // Added delay to ensure proper timing
    } else if (received == 'g') {
      displayLetter(G);
      Serial.println("Mostrando letra G");
      int sequenceG[] = {FREQ_DU, FREQ_DU, FREQ_DA, FREQ_DU};
      playSequenceWithSpaces(sequenceG, 4);
      delay(1000); // Added delay to ensure proper timing
    } else if (received == 'e') {
      displayLetter(E);
      Serial.println("Mostrando letra E");
      int sequenceE[] = {FREQ_DU, FREQ_DA, FREQ_DU, FREQ_DU};
      playSequenceWithSpaces(sequenceE, 4);
      delay(1000); // Added delay to ensure proper timing
    } else if (received == 's') {
      displayLetter(S);
      Serial.println("Mostrando letra S");
      int sequenceS[] = {FREQ_DA, FREQ_DU, FREQ_DU, FREQ_DU};
      playSequenceWithSpaces(sequenceS, 4);
      delay(1000); // Added delay to ensure proper timing
    } else if (received == 'm') {
      displayLetter(M);
      Serial.println("Mostrando letra M");
      int sequenceM[] = {FREQ_DA};
      playSequenceWithSpaces(sequenceM, 1);
      delay(1000); // Added delay to ensure proper timing
    }
  }
}

// Función para mostrar una letra
void displayLetter(byte letter[]) {
  for (int i = 0; i < 8; i++) {
    lc.setRow(0, i, letter[i]);
  }
}
