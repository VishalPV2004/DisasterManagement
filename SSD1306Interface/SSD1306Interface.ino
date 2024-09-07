#include <WiFi.h>
#include <LoRa.h>
#include <SPI.h>

#define ss 5
#define rst 14
#define dio0 2

// Wi-Fi credentials
const char* ssid     = "ESP32-Access-Point";
const char* password = "123456789";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Variables to store the form data
String name = "";
String age = "";
String area = "";
String needs = "";

void setup() 
{
  Serial.begin(115200); 
  while (!Serial);

  // Initialize LoRa
  Serial.println("LoRa Sender");
  LoRa.setPins(ss, rst, dio0);    // Setup LoRa transceiver module
  
  while (!LoRa.begin(433E6))     // 433E6 - Asia, 866E6 - Europe, 915E6 - North America
  {
    Serial.println(".");
    delay(500);
  }
  LoRa.setSyncWord(0xA5);
  Serial.println("LoRa Initializing OK!");

  // Initialize Wi-Fi
  Serial.print("Setting AP (Access Point)...");
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  server.begin();
}

void loop() 
{
  WiFiClient client = server.available();   // Listen for incoming clients

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
            // Process the form data
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

              // Print the form data to the serial monitor
              Serial.println("Form Data:");
              Serial.println("Name: " + name);
              Serial.println("Age: " + age);
              Serial.println("Area: " + area);
              Serial.println("Needs: " + needs);

              // Send the form data via LoRa
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
            }

            // HTTP headers
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // Display the HTML form
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
            
            // The HTTP response ends with another blank line
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
    
    // Clear the header variable
    header = "";
    
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
