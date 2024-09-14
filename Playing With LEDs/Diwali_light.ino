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

bool cycleLeds = false;  // State to start or stop LED cycling
bool ledOff = true;      // To track if LEDs are currently off

// Timing for cycling LEDs
unsigned long previousMillis = 0;
const long interval = 1000;  // 1-second interval
int currentLed = 0;          // To keep track of which LED to turn on

// Current time for client connection timeout
unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000;  // Timeout for client connection (2 seconds)

void setup() {
  Serial.begin(115200);
  // Set the pinmode of the pins to which the LEDs are connected and turn them low to start
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
    String currentLine = "";                // Make a string to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // Loop while the client is connected
      currentTime = millis();
      if (client.available()) {             // If there's bytes to read from the client,
        char c = client.read();             // Read a byte, then
        Serial.write(c);                    // Print it out to the serial monitor
        header += c;
        if (c == '\n') {                    // If the byte is a newline character
          // If the current line is blank, two newlines were received in a row.
          // That's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g., HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // Toggle the LED cycling when the button is pressed
            if (header.indexOf("GET /toggle") >= 0) {
              if (cycleLeds == false) {
                Serial.println("Cycling LEDs on");
                cycleLeds = true;   // Start cycling the LEDs
                ledOff = false;     // LEDs are not off
              } else {
                Serial.println("Turning all LEDs off");
                cycleLeds = false;  // Stop cycling LEDs
                turnAllLedsOff();   // Ensure all LEDs are turned off
                ledOff = true;      // LEDs are off now
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
            client.println("<body><h1>Heet's Diwali IoT Portal</h1>");
            
            // Display the toggle button
        
            // client.println("<p>LEDs - State " + ledState + "</p>");
            client.println("<p><a href=\"/toggle\"><button class=\"button\">TOGGLE</button></a></p>");
            
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else {  // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // If you got anything else but a carriage return character,
          currentLine += c;      // Add it to the end of the currentLine
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

  // Handle the cycling of LEDs
  if (cycleLeds) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      // Save the last time an LED was changed
      previousMillis = currentMillis;

      // Turn off all LEDs before turning on the next one
      turnAllLedsOff();
      
      // Move to the next LED
      if (currentLed == 0) {
        digitalWrite(greenled, HIGH);
      } else if (currentLed == 1) {
        digitalWrite(redled, HIGH);
      } else if (currentLed == 2) {
        digitalWrite(blueled, HIGH);
      }
      
      // Cycle through the 3 LEDs
      currentLed = (currentLed + 1) % 3;
    }
  }
}

// Function to turn off all LEDs
void turnAllLedsOff() {
  digitalWrite(greenled, LOW);
  digitalWrite(redled, LOW);
  digitalWrite(blueled, LOW);
}
