#include <SPI.h>
#include "RF24.h"

// Pins definieren für die Ultraschallsensoren
#define TRIGGER_PIN1 2
#define ECHO_PIN1 3
#define TRIGGER_PIN2 4
#define ECHO_PIN2 5
#define TRIGGER_PIN3 6
#define ECHO_PIN3 7

// Hardware-Konfiguration des nRF24L01 Funkmodul auf den SPI-Bus mit Pins 9 & 10
RF24 radio(9, 10);

// Adressen für das Empfänger nRF24L01 Modulen
byte node_B_address[6] = "NodeB";

// Struktur für die Daten, die übertragen werden sollen
struct DataPacket {
  int distance1;
  int distance2;
  int distance3;
};

// Klasse für die Ultraschallsensor
class UltrasonicSensor {
  private:
    int triggerPin;
    int echoPin;

  public:
    UltrasonicSensor(int trigger, int echo) {
      triggerPin = trigger;
      echoPin = echo;
      pinMode(triggerPin, OUTPUT);
      pinMode(echoPin, INPUT);
    }
    // Methode zur Messung der Entfernung in Zentimetern
    int measureDistance() {
      // Senden Sie einen Ultraschallimpuls
      digitalWrite(triggerPin, LOW);
      delayMicroseconds(2);
      digitalWrite(triggerPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(triggerPin, LOW);

      // Messen der die Zeit, die der Impuls benötigt, um zurückzukommen
      long duration = pulseIn(echoPin, HIGH);

      // Berechnen der Entfernung (in cm) basierend auf der Geschwindigkeit des Schalls
      int distance = duration / 29.15 / 2;
        if (distance > 400) {
        distance = 400;
      } 
      return distance;
    }
};

// RFTransmitter Klasse
class RFTransmitter {
  private:
    RF24& radio;
    byte* address;
    
  public:
    RFTransmitter(RF24& radio, byte* address) : radio(radio), address(address) {}

    // Methode zum Senden der gemessenen Entfernungen
    void sendDistances(int distance1, int distance2, int distance3) {
      // Erstelle ein Datenpaket mit den Entfernungen
      DataPacket data = {distance1, distance2, distance3};
      // Daten über das Funkmodul senden
      radio.write(&data, sizeof(DataPacket));
    }
};

// Globale Variablen für die Sensoren und den Funk
UltrasonicSensor sensor1(TRIGGER_PIN1, ECHO_PIN1);
UltrasonicSensor sensor2(TRIGGER_PIN2, ECHO_PIN2);
UltrasonicSensor sensor3(TRIGGER_PIN3, ECHO_PIN3);
RFTransmitter rfTransmitter(radio, node_B_address);

void setup() {
  // Initialisiere das Funkmodul
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.openWritingPipe(node_B_address);
  radio.stopListening(); // Modul in den Sendemodus versetzen
}

void loop() {
  // Messen der Entfernung für jeden Sensor
  int distance1 = sensor1.measureDistance();
  int distance2 = sensor2.measureDistance();
  int distance3 = sensor3.measureDistance();


  // Senden der Entfernungen über das Funkmodul
  rfTransmitter.sendDistances(distance1, distance2, distance3);

}
