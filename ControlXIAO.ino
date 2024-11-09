#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// Definir los servicios y características BLE
BLEServer *pServer = NULL;
BLECharacteristic *pPotCharacteristic = NULL;
BLECharacteristic *pLedCharacteristic = NULL;
BLEService *pService = NULL;

// Definir UUIDs para el servicio y las características
#define SERVICE_UUID          "12345678-1234-1234-1234-123456789012"
#define POT_CHARACTERISTIC_UUID "87654321-4321-4321-4321-210987654321"
#define LED_CHARACTERISTIC_UUID "87654321-4321-4321-4321-210987654322"

// Pines del potenciómetro y el LED
const int potPin = A0; 
const int ledPin = 6; // Cambia este valor al pin donde tengas conectado el LED

// Umbral en voltios para activar el LED
const float THRESHOLD_VOLTAGE = 2.1;

// Variables para almacenar el estado anterior del LED
bool ledState = false;

// Función para actualizar el estado del LED y notificar el cambio
void updateLedState(bool state) {
  ledState = state;
  digitalWrite(ledPin, state ? HIGH : LOW);

  // Notificar el estado del LED
  String ledStateStr = state ? "ON" : "OFF";
  pLedCharacteristic->setValue(ledStateStr.c_str());
  pLedCharacteristic->notify();

  Serial.print("LED ");
  Serial.println(ledStateStr);
}

void setup() {
  // Inicializar el monitor serial
  Serial.begin(115200);

  // Inicializar el pin del potenciómetro y el LED
  pinMode(potPin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);  // Asegurarse de que el LED esté apagado al inicio

  // Iniciar el BLE
  BLEDevice::init("ESP32-TILICO");
  pServer = BLEDevice::createServer();

  // Crear el servicio BLE
  pService = pServer->createService(SERVICE_UUID);

  // Crear la característica para el valor del potenciómetro
  pPotCharacteristic = pService->createCharacteristic(
                        POT_CHARACTERISTIC_UUID,
                        BLECharacteristic::PROPERTY_READ |
                        BLECharacteristic::PROPERTY_NOTIFY
                      );

  // Crear la característica para el estado del LED
  pLedCharacteristic = pService->createCharacteristic(
                        LED_CHARACTERISTIC_UUID,
                        BLECharacteristic::PROPERTY_READ |
                        BLECharacteristic::PROPERTY_NOTIFY
                      );

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

  // Escalar el valor a voltios (0 a 3.3 V)
  float scaledValue = map(potValue, 0, 4095, 0, 3300) / 1000.0;

  // Convertir el valor a cadena de texto para enviar
  char potValueStr[8];
  dtostrf(scaledValue, 4, 2, potValueStr); // Conversión flotante a string con 2 decimales

  // Enviar el valor del potenciómetro como notificación
  pPotCharacteristic->setValue(potValueStr);
  pPotCharacteristic->notify();  // Notificar a los dispositivos conectados

  // Mostrar el valor en el monitor serial
  Serial.print("Valor del potenciómetro enviado: ");
  Serial.println(scaledValue);

  // Comprobar si el voltaje supera el umbral
  if (scaledValue > THRESHOLD_VOLTAGE && !ledState) {
    updateLedState(true); // Encender el LED y notificar
  } else if (scaledValue <= THRESHOLD_VOLTAGE && ledState) {
    updateLedState(false); // Apagar el LED y notificar
  }

  // Esperar 100 ms antes de la siguiente lectura y envío
  delay(100);
}
