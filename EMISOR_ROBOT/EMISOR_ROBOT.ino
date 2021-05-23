#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(9, 10); // CE, CSN
const byte address[6] = "00001";
char datos[4];
void setup() {
  radio.begin();
    Serial.begin(115200); 
 
//Abrimos un canal de escritura
 
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
}
void loop() {
  
 
 if (Serial.available()){
  
  datos[0] = Serial.read();
   

     
    
  Serial.flush();
  //cargamos los datos en la variable datos[]
 //enviamos los datos
 bool ok = radio.write(datos, sizeof(datos));
}

}
