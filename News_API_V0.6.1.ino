/*
 News API Display V0.6.1
 Robert James Hastings
 Github CountZero1066
   ###############################################################
   #                      News API Display                       #
   #                                                             #
   #      Intended for use with the ESPIRIFF Systems ESP8266     #
   #      and the AZDelivery MAX7219 LED Dot Matrix Display.     #
   #      Calls the NewsAPI.org API and extracts news            #
   #      headlines and scrolls them accross the display.        # 
   #                                                             #
   ###############################################################
	Version Detail:
            -First commit 
*/
//-------------------------------------------------------------------------------------------
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 12 // 12 MAX7219 units
#define CLK_PIN 14 // or SCK
#define DATA_PIN 13 // or MOSI
#define CS_PIN 15 // or SS

MD_Parola myDisplay = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);// Initialize MD_Parola object for LED display

DynamicJsonDocument jsonBuffer(1024); // Buffer for JSON data
// WiFi credentials
const char* ssid = "your SSID";
const char* password = "your Password";
const char* host = "newsapi.org";
const int port = 443;
const char* apiPath = "/v2/top-headlines?country=us&apiKey=your API key"; //insert your API key here

const int MAX_TITLES = 20;// Maximum number of news titles to display
String titles[MAX_TITLES];// Array to store news titles
int numTitles = 0; // Counter for the number of titles retrieved
//-------------------------------------------------------------------------------------------
//Setup
//-------------------------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(100);
  myDisplay.begin(); // Initialize LED display
  myDisplay.setIntensity(5); // Set display intensity
  myDisplay.displayClear(); // Clear display
  myDisplay.setTextAlignment(PA_CENTER); // Set text alignment
  myDisplay.setInvert(false); // Set display inversion
  
  myDisplay.print("News API Display");// Print startup message on LED display
  delay(1000);
  connectToWiFi(); // Connect to WiFi network
}
//-------------------------------------------------------------------------------------------
//Main Loop
//-------------------------------------------------------------------------------------------
void loop() {
  String apiResponse = sendApiRequest(); // Send API request and get response
  parseApiResponse(apiResponse); // Parse API response to extract news titles
  printTitlesToSerial(); // Print parsed titles to serial monitor for debugging
  // Display titles on LED display
 for(int i = 0; i < numTitles; i++){
  Display_Scroll_Text(titles[i], 40);
 }
  delay(5000); 
}
//-------------------------------------------------------------------------------------------
//Connect to AP
//-------------------------------------------------------------------------------------------
void connectToWiFi() {
  Serial.println();
  Serial.println("Connecting to WiFi...");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected!");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
//-------------------------------------------------------------------------------------------
//Send API Request and return response to caller
//-------------------------------------------------------------------------------------------
String sendApiRequest() {
  WiFiClientSecure client; // Initialize a secure WiFi client for HTTPS connection

  Serial.print("Resolving host: ");
  Serial.println(host);

  // Resolve the hostname to an IP address
  IPAddress serverIP;
  if (!WiFi.hostByName(host, serverIP)) { // Resolve the hostname to IP address
    Serial.println("Failed to resolve host!"); // Print error if resolution fails
    return "";
  }

  Serial.print("Server IP address: ");
  Serial.println(serverIP); // Print resolved IP address

  Serial.print("Connecting to ");
  Serial.println(host);

  // Disable certificate verification (not recommended for production use)
  client.setInsecure();

  if (!client.connect(host, port)) { // Establish connection to the host
    Serial.print("Connection failed! Error code: ");
    Serial.println(client.getLastSSLError()); // Print error code if connection fails
    return "";
  }

  // Request the API path
  String url = apiPath; // Construct the complete URL
  Serial.print("Requesting URL: ");
  Serial.println(url); // Print requested URL

  // Send HTTP GET request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: ESP8266\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("Request sent"); // Print confirmation message

  // Wait for the response headers
  while (client.connected() && !client.available()) {
    delay(100); // Wait for response headers to become available
  }

  // Read and process the response in smaller chunks
  const size_t bufferSize = 1024;
  char buffer[bufferSize];
  String response = "";
  while (client.available()) {
    size_t bytesRead = client.readBytes(buffer, bufferSize); // Read response data into buffer
    response += String(buffer); // Append buffer contents to response string
    memset(buffer, 0, bufferSize); // Clear buffer for next iteration
  }

  // Print the received response
  Serial.println("Received response:");
  Serial.println(response); // Print received response

  Serial.println("Closing connection");
  client.stop(); // Close connection with the server

  return response; // Return the received response
}
//-------------------------------------------------------------------------------------------
//Extract and parse JSON
//-------------------------------------------------------------------------------------------
void parseApiResponse(String apiResponse) {
  // Find the position of the start and end of the JSON data
  int startPos = apiResponse.indexOf("{\"status\":\"ok\"");
  int endPos = apiResponse.lastIndexOf("}]}") + 3; // Include the closing bracket and brace
  
  // Check if startPos and endPos are valid
  if (startPos == -1 || endPos == -1 || startPos >= endPos) {
    Serial.println("Invalid JSON data in the response");
    return;
  }

  // Extract the JSON string
  String jsonString = apiResponse.substring(startPos, endPos);

  // Parse the JSON response
  DynamicJsonDocument jsonBuffer(apiResponse.length() + 100); // Adjust the size according to your response
  DeserializationError error = deserializeJson(jsonBuffer, jsonString);

  // Check for parsing errors
  if (error) {
    Serial.print("Failed to parse JSON: ");
    Serial.println(error.c_str());
    Serial.println("JSON Response:");
    Serial.println(jsonString);
    return;
  }

  // Extract and store titles
  numTitles = 0; // Clear existing titles
  JsonArray articles = jsonBuffer["articles"];
  for (JsonObject article : articles) {
    const char* title = article["title"];
    // Check if the array index is within bounds
    if (numTitles < MAX_TITLES) {
      titles[numTitles++] = title; // Store the title in the array
    }
  }
}

//-------------------------------------------------------------------------------------------
//Display text to MAX7219 LED Dot Matrix Display
//-------------------------------------------------------------------------------------------
void Display_Scroll_Text(String text_to_scroll, int timedelay) {
  const char* x = text_to_scroll.c_str();
  myDisplay.displayClear();
  delay(50);
  myDisplay.displayText(x, PA_CENTER, 100, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
  int charlen = text_to_scroll.length() * timedelay;
  for (int i = 0; i <= charlen + 48; ++i) {
    delay(10);
    if (myDisplay.displayAnimate()) {
      myDisplay.displayReset();
    }
    delay(50);
  }
}
//-------------------------------------------------------------------------------------------
//Print parsed JSON contents to serial (used in debugging)
//-------------------------------------------------------------------------------------------
void printTitlesToSerial() {
  Serial.println("Titles stored in the array:");
  for (int i = 0; i < numTitles; i++) {
    Serial.print("Title ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.println(titles[i]);
  }
}
