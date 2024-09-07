#include <WiFi.h>

// Replace with your network credentials
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

void setup() {
  Serial.begin(115200);
  
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // Process the form data
            if (header.indexOf("GET /submit?") >= 0) {
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
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
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

