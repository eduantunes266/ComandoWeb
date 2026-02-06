#include <Arduino.h>
#include <IRremote.h>

const int PIN_RECETOR = 15;  
const int PIN_LED_AZUL = 2;  

void setup() {
  Serial.begin(115200);
  
  IrReceiver.begin(PIN_RECETOR, ENABLE_LED_FEEDBACK);
  pinMode(PIN_LED_AZUL, OUTPUT);
  
  Serial.println("--- PRONTO! Aponte o comando MEO ---");
}

void loop() {
  if (IrReceiver.decode()) {
    digitalWrite(PIN_LED_AZUL, HIGH);
    
    
    if (IrReceiver.decodedIRData.decodedRawData != 0) {
      Serial.print("BOTAO: 0x");
      Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
    }

    delay(100); 
    
    digitalWrite(PIN_LED_AZUL, LOW);
    
    IrReceiver.resume(); 
  }
}