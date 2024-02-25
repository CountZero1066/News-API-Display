# News-API-Display
Designed for use with the ESP8266 microcontroller and the MAX7219 LED Dot Matrix Display. It leverages the NewsAPI.org API to fetch news headlines and scrolls them across the LED display.

## Features:
- Display News Headlines: Fetches top headlines from NewsAPI.org and displays them on the LED Dot Matrix Display.
- Scalable Design: Can handle up to 12 MAX7219 LED Dot Matrix Display units for larger displays.
- WiFi Connectivity: Connects to a WiFi network to fetch news headlines over the internet.
- Dynamic Text Scrolling: Scrolls news headlines horizontally on the LED display for easy readability.
- Debugging Support: Provides serial output for debugging, including printing fetched headlines.
  
## Components:
- ESP8266 microcontroller
- MAX7219 LED Dot Matrix Display
- NewsAPI.org API
  
## Dependencies:
- MD_Parola library for LED Dot Matrix Display control
- ArduinoJson library for parsing JSON responses
- ESP8266WiFi library for WiFi connectivity

## API Key and Certificate

To use this project, you'll need to obtain an API key from [NewsAPI.org](https://newsapi.org/) and include it in the code. Follow these steps:

#### 1. Sign up for a free or paid account on NewsAPI.org.
#### 2. Once logged in, navigate to your account settings to find your API key.
#### 3. Copy your API key and paste it into the `apiPath` variable in the code.

Additionally, if your ESP8266 device requires a certificate for secure communication (e.g., for HTTPS requests), make sure to include it in the appropriate part of your code.
## Adapting the Code for Other APIs

This project can be easily adapted to work with other APIs by modifying the API endpoint, request parameters, and response parsing logic. Follow these steps to customize the code for different APIs:

#### 1. Identify the API
Choose the API you want to integrate with. Ensure that the API provides the data you want to display on the LED Dot Matrix Display.

#### 2. Update API Endpoint
Modify the `apiPath` variable in the code to point to the endpoint of the new API. This is typically the URL that the API provides for making requests.

#### 3. Adjust Request Parameters
Review the documentation of the new API to understand its request parameters. Modify the request (e.g., query parameters, headers) in the `sendApiRequest()` function to match the requirements of the new API.

#### 4. Parse API Response
Update the response parsing logic in the `parseApiResponse()` function to extract the relevant data fields from the API response. This may involve accessing different JSON keys or fields depending on the structure of the response.

#### 5. Ensure Data Compatibility
Make sure that the data format returned by the new API matches the expectations of the code for proper parsing and display. If necessary, adjust the parsing logic to handle different data formats.

By following these steps, you can effectively adapt the code to work with different APIs, expanding its functionality and versatility to suit your needs.
