#include <WiFi.h>
#include <LoRa.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define ss 5
#define rst 14
#define dio0 2

const char* ssid     = "ESP32-Access-Point";
const char* password = "123456789";

WiFiServer server(80);

String header;

String name = "";
String age = "";
String area = "";
String needs = "";

String statusMessage = "-";

unsigned long lastUpdateTime = 0;
bool messageSent = false;

void setup() 
{
  Serial.begin(115200); 
  while (!Serial);

  // Initialize LoRa
  Serial.println("LoRa Sender");
  LoRa.setPins(ss, rst, dio0);    
  
  while (!LoRa.begin(433E6))     // 433E6 - Asia, 866E6 - Europe, 915E6 - North America
  {
    Serial.println(".");
    delay(500);
  }
  LoRa.setSyncWord(0xA5);
  Serial.println("LoRa Initializing OK!");

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  Serial.print("Setting AP (Access Point)...");
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  updateOLEDStatus();
  
  server.begin();
}

void updateOLEDStatus() {
  IPAddress IP = WiFi.softAPIP();
  
  display.clearDisplay();
  
  display.setTextSize(2);  
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);  
  display.println("VeeConn");

  display.drawRect(0, 30, SCREEN_WIDTH, 30, SSD1306_WHITE);  
  display.setTextSize(1); 
  display.setCursor(5, 35);  
  display.print("IP: ");
  display.println(IP);

  display.setCursor(5, 50);  
  display.print("Status: ");
  display.println(statusMessage);

  display.display();
}

void loop() 
{
  WiFiClient client = server.available();   

  if (client) 
  {                             
    Serial.println("New Client.");         
    String currentLine = "";                

    while (client.connected()) 
    {            
      if (client.available()) 
      {            
        char c = client.read();             
        Serial.write(c);                    
        header += c;
        
        if (c == '\n') 
        {                    
          if (currentLine.length() == 0) 
          {
         
            if (header.indexOf("GET /submit?") >= 0) 
            {
              int nameStart = header.indexOf("name=") + 5;
              int nameEnd = header.indexOf("&", nameStart);
              name = header.substring(nameStart, nameEnd);

              int ageStart = header.indexOf("age=") + 4;
              int ageEnd = header.indexOf("&", ageStart);
              age = header.substring(ageStart, ageEnd);

              int areaStart = header.indexOf("area=") + 5;
              int areaEnd = header.indexOf("&", areaStart);
              area = header.substring(areaStart, areaEnd);

              int needsStart = header.indexOf("needs=") + 6;
              int needsEnd = header.indexOf(" ", needsStart);
              needs = header.substring(needsStart, needsEnd);

              Serial.println("Form Data:");
              Serial.println("Name: " + name);
              Serial.println("Age: " + age);
              Serial.println("Area: " + area);
              Serial.println("Needs: " + needs);

              LoRa.beginPacket();
              LoRa.print("Name: ");
              LoRa.print(name);
              LoRa.print(", Age: ");
              LoRa.print(age);
              LoRa.print(", Area: ");
              LoRa.print(area);
              LoRa.print(", Needs: ");
              LoRa.print(needs);
              LoRa.endPacket();

              statusMessage = "Message sent!";
              messageSent = true;
              lastUpdateTime = millis();  
              updateOLEDStatus();  
            }

            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".inputField { padding: 12px 20px; margin: 8px 0; display: inline-block; border: 1px solid #ccc; box-sizing: border-box;}");
            client.println(".button { background-color: #4CAF50; color: white; padding: 14px 20px; margin: 8px 0; border: none; cursor: pointer; width: 100%;}");
            client.println(".button:hover { opacity: 0.8; }</style></head>");
            
            client.println("<body><h1>ESP32 Form</h1>");
            client.println("<form action=\"/submit\" method=\"GET\">");
            client.println("Name: <input type=\"text\" name=\"name\" class=\"inputField\"><br>");
            client.println("Age: <input type=\"number\" name=\"age\" class=\"inputField\"><br>");
            client.println("Area: <input type=\"text\" name=\"area\" class=\"inputField\"><br>");
            client.println("Needs: <input type=\"text\" name=\"needs\" class=\"inputField\"><br>");
            client.println("<input type=\"submit\" value=\"Submit\" class=\"button\">");
            client.println("</form>");
            client.println("</body></html>");
            
            client.println();
            break;
          } 
          else 
          {
            currentLine = "";
          }
        } 
        else if (c != '\r') 
        {  
          currentLine += c;      
        }
      }
    }
    
   
    header = "";

   
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }

 
  if (messageSent && millis() - lastUpdateTime > 5000) {
    statusMessage = "-";
    messageSent = false;
    updateOLEDStatus();
  }
}
