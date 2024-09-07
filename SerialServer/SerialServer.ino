#include <LoRa.h>
#include <SPI.h>

#define ss 5
#define rst 14
#define dio0 2

void setup() 
{
  Serial.begin(9600);
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
    String name = "Name: ";
    String age = "Age: ";
    String area = "Area: ";
    String needs = "Needs: ";

    String LoRaData;
    while (LoRa.available())             
    {
      LoRaData += (char)LoRa.read();
    }
    
    int firstComma = LoRaData.indexOf(',');
    int secondComma = LoRaData.indexOf(',', firstComma + 1);
    int thirdComma = LoRaData.indexOf(',', secondComma + 1);

    if (firstComma != -1) {
      name += LoRaData.substring(0, firstComma);
    }
    if (secondComma != -1) {
      age += LoRaData.substring(firstComma + 1, secondComma);
    }
    if (thirdComma != -1) {
      area += LoRaData.substring(secondComma + 1, thirdComma);
      needs += LoRaData.substring(thirdComma + 1);
    }

    Serial.println(name);
    Serial.println(age);
    Serial.println(area);
    Serial.println(needs);
  }
}
