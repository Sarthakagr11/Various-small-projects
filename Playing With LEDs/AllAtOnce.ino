#include <ESP8266WiFi.h>

// Replace with your network credentials
const char* ssid = "SARTHAK's S23 FE";
const char* password = "Sarthak13";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;
// Declare the pins to which the LEDs are connected
const int greenled = 5;
const int redled = 4;
const int blueled = 14;  // Third LED pin

String ledState = "off";  // State of all LEDs

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);
  // Set the pinmode of the pins to which the LEDs are connected and turn them low to prevent fluctuations
  pinMode(greenled, OUTPUT);
  pinMode(redled, OUTPUT);
  pinMode(blueled, OUTPUT);
  digitalWrite(greenled, LOW);
  digitalWrite(redled, LOW);
  digitalWrite(blueled, LOW);
  
  // Connect to access point
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP()); // IP address to be entered into the browser
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    String currentLine = "";                // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // Loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // If there's bytes to read from the client,
        char c = client.read();             // Read a byte, then
        Serial.write(c);                    // Print it out the serial monitor
        header += c;
        if (c == '\n') {                    // If the byte is a newline character
          // If the current line is blank, you got two newline characters in a row.
          // That's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // Toggle all LEDs on/off
            if (header.indexOf("GET /toggle") >= 0) {
              if (ledState == "off") {
                Serial.println("Turning all LEDs on");
                ledState = "on";
                digitalWrite(greenled, HIGH);
                digitalWrite(redled, HIGH);
                digitalWrite(blueled, HIGH);
              } else {
                Serial.println("Turning all LEDs off");
                ledState = "off";
                digitalWrite(greenled, LOW);
                digitalWrite(redled, LOW);
                digitalWrite(blueled, LOW);
              }
            }
       
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>Heet's IoT Portal</h1>");
            
            // Display current state, and a single ON/OFF button for all LEDs
            client.println("<p>LEDs - State " + ledState + "</p>");
            client.println("<p><a href=\"/toggle\"><button class=\"button\">TOGGLE</button></a></p>");
            
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else {  // if you got a newline, then clear currentLine
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
