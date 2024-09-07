#include <LoRa.h>
#include <SPI.h>
 
#define ss 5
#define rst 14
#define dio0 2
 
void setup() 
{
  Serial.begin(115200);
  while (!Serial);
  Serial.println("LoRa Receiver");
 
  LoRa.setPins(ss, rst, dio0);    
 
  while (!LoRa.begin(433E6))     
  {
    Serial.println(".");
    delay(500);
  }
  LoRa.setSyncWord(0xA5);
  Serial.println("LoRa Initializing OK!");
}
 
void loop() 
{
  int packetSize = LoRa.parsePacket();    
  if (packetSize) 
  {
    
    Serial.print("Received packet '");
 
    while (LoRa.available())             
    {
      String LoRaData = LoRa.readString();
      Serial.print(LoRaData); 
    }      
    Serial.println(LoRa.packetRssi());
  }
}