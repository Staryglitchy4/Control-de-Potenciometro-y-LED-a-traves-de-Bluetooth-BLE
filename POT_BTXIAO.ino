#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// Definir el servicio y la característica BLE
BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;
BLEService *pService = NULL;

// Definir el UUID para el servicio y la característica
#define SERVICE_UUID        "12345678-1234-1234-1234-123456789012"
#define CHARACTERISTIC_UUID "87654321-4321-4321-4321-210987654321"

// Pin del potenciómetro
const int potPin = A0; 

// Pin del LED
const int ledPin = 6; // Cambia este valor al pin donde tengas conectado el LED

//String value="";

// Callback para manejar los eventos BLE
class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    String value = pCharacteristic->getValue();

    if (value.length() > 0) {
      Serial.print("Comando recibido: ");
      Serial.println(value.c_str());

      if (value == "S") {
        digitalWrite(ledPin, HIGH); // Encender LED
        Serial.println("LED encendido.");
      } 
      else if (value == "N") {
        digitalWrite(ledPin, LOW);  // Apagar LED
        Serial.println("LED apagado.");
      }
    }
  }
};

void setup() {
  // Inicializar el monitor serial
  Serial.begin(115200);

  // Inicializar el pin del potenciómetro
  pinMode(potPin, INPUT);

  // Inicializar el pin del LED
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);  // Asegurarse que el LED esté apagado al inicio

  // Iniciar el BLE
  BLEDevice::init("ESP32-TILICO");
  pServer = BLEDevice::createServer();

  // Crear el servicio BLE
  pService = pServer->createService(SERVICE_UUID);

  // Crear la característica BLE
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );

  // Establecer el callback para recibir comandos
  pCharacteristic->setCallbacks(new MyCallbacks());

  // Iniciar el servicio
  pService->start();

  // Empezar la publicidad BLE
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();

  Serial.println("ESP32 BLE iniciado. Conéctate desde nRF Connect.");
}

void loop() {
  // Leer el valor del potenciómetro (entre 0 y 4095)
  int potValue = analogRead(potPin);

  // Escalar el valor a un rango más adecuado (por ejemplo, de 0 a 3.3 V)
  float scaledValue = map(potValue, 0, 4095, 0, 3300) / 1000.0; // Convertido a voltios

  // Convertir el valor a cadena de texto para enviar
  char potValueStr[8];
  dtostrf(scaledValue, 4, 2, potValueStr); // Conversión flotante a string con 2 decimales

  // Enviar el valor como una notificación
  pCharacteristic->setValue(potValueStr);
  pCharacteristic->notify();  // Notificar a los dispositivos conectados

  // Mostrar el valor en el monitor serial
  Serial.print("Valor del potenciómetro enviado: ");
  Serial.println(scaledValue);

  // Esperar 100 ms antes de la siguiente lectura y envío
  delay(100);
}
