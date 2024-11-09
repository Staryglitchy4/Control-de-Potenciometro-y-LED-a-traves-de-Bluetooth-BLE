// Definir el pin del LED
const int ledPin = 6;  // Utilizando el pin D2 para el LED

void setup() {
  // Inicializar el pin del LED como salida
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin,LOW);

  // Inicializar el monitor serial
  Serial.begin(115200);
  
  // Mensaje inicial en el monitor serial
  Serial.println("Envía 'S' para encender el LED o 'N' para apagarlo.");
}

void loop() {
  // Comprobar si hay datos disponibles en el monitor serial
  if (Serial.available() > 0) {
    // Leer el caracter enviado desde el monitor serial
    char command = Serial.read();

    // Verificar el comando recibido
    if (command == 'S') {
      // Encender el LED
      digitalWrite(ledPin, HIGH);
      Serial.println("LED encendido");
    } 
    else if (command == 'N') {
      // Apagar el LED
      digitalWrite(ledPin, LOW);
      Serial.println("LED apagado");
    }
  }
}
