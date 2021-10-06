#include <NewPingESP8266.h>


#define TRIGGER_PIN  0  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     4  // Arduino pin tied to echo pin on the ultrasonic sensor.

long duracao = 0;
float distancia = 0;
float leitura, conversao1, conversao2, conversao3;
float aux;

NewPingESP8266 sonar(TRIGGER_PIN, ECHO_PIN); // NewPing setup of pins and maximum distance.


float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
 return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void setup() {
  Serial.begin(9600); // Open serial monitor at 115200 baud to see ping results.
}

void loop() {
  delay(1000); // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
   digitalWrite(TRIGGER_PIN, LOW);    // Limpa trigPin
  delayMicroseconds(2);          // aguarda 2 microsegundos
 
  digitalWrite(TRIGGER_PIN, HIGH);   // Seta trigPin HIGH aguarda 
  delayMicroseconds(10);         // aguada 10 microsegundos
  digitalWrite(TRIGGER_PIN, LOW);    // Seta trigPin LOW 
   
  // Leitura do echoPin, retorna a onda de som em microsegundos
  duracao = pulseIn(ECHO_PIN, HIGH);
  distancia= duracao*0.034/2;
//  distancia = distancia*10; // converter cm para mm
  
//  leitura = sonar.ping_cm();
 conversao1 = distancia*227,17; // volume(de um cone cortado) é dado em cm cúbico
 conversao2 = (conversao1/1000);
conversao3 = (mapFloat(conversao2, 0, 3.6, 3.6, 0));// converte 0 ate 3.6L

 Serial.print("Litros: ");
 Serial.println(conversao3); // Send ping, get distance in cm and print result (0 = outside set distance range)
  
 Serial.print("c2: ");
 Serial.println(conversao2); // Send ping, get distance in cm and print result (0 = outside set distance range)

 Serial.print("c1: ");
Serial.println(conversao1); // Send ping, get distance in cm and print result (0 = outside set distance range)


    Serial.print("leitura: ");
  Serial.println(distancia); // Send ping, get distance in cm and print result (0 = outside set distance range)
  
}
