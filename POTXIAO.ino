// Definir el pin del potenciómetro
const int potPin = A0; // Asegúrate de que esté conectado a un pin analógico

void setup() {
  // Inicializar el monitor serial
  Serial.begin(115200);

  // Esperar a que el puerto serial esté listo
  while (!Serial) {
    delay(10); // Esperar a que el puerto esté disponible
  }
}

void loop() {
  // Leer el valor analógico del potenciómetro
  int potValue = analogRead(potPin);

  // Imprimir el valor en el monitor serial
  float potValue1 = map(potValue, 0, 4095, 0, 3300);
  potValue1/=1000;
  //Serial.print("Valor del potenciómetro en voltaje: ");
  Serial.println(potValue1);

  // Pausar un momento antes de la siguiente lectura
  delay(500); // Ajusta el retardo si deseas lecturas más rápidas o lentas
}
