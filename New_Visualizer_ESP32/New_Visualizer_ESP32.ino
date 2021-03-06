/*********
  For anyone needing explanations, randomnerdtutorials.com is great for ESP-32 tutorials. I used them to create this.
*********/

// Import required libraries
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"

// Replace with your network credentials if connecting to a network
// Replace with whatever credentials you want if creating an Access Point
const char* ssid = "ESP32";
const char* password = "123456789"; // must be at least 8 characters

// Set LED GPIO
const int ledPin = 2;
// Stores LED state
String productState;
String productStateOld = "Choose a mode";
uint8_t currentBrightness;
int currentMode;
String currentPattern;
String inputAudioMultiplier;

String inputHue = "0";
String inputSat = "255";
String inputVal = "255";
String inputMasterBrightness;
int sendMode;
String sendModeStr;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// This function creates the text on the webpage that shows up after an input.
String processor(const String& var) {
	if (var == "INPUTMASTERBRIGHTNESS") {
		return inputMasterBrightness;
	}
	if (var == "CURRENTPATTERN") {
		if (currentMode == 0) {
			productStateOld = productState;
			productState = "OFF";
		}
		if (currentMode == 1) 
			productState = productStateOld;
		if (currentMode > 4 && currentMode < 17)
			productState = "Music Visualizing Pattern " + String(currentMode - 4);
		if (currentMode == 2 || currentMode == 3 || currentMode == 4)
			//productState = "A Solid Color";
			productState = currentPattern;
		if (currentMode > 16 && currentMode < 23)
			productState = "Ambient Pattern " + String(currentMode - 16);
		if (currentMode == 98)
			productState = "Automatic Pattern Switching";
		return productState;
	}
	if (var == "INPUTAUDIOCONTROL") {
		return inputAudioMultiplier;
	}
	return String();
}

void setup() {
	// Serial port for debugging purposes
	Serial.begin(115200); // Serial monitor output
	Serial2.begin(115200); // TX2/RX2 pins that are connected to Teensy TX1/RX1 pins
	pinMode(ledPin, OUTPUT);

	// Initialize SPIFFS (SPI FORMAT FILE SYSTEM makes it possible to upload .html, .css, .jpg/.png files to memory
	if (!SPIFFS.begin(true)) {
		Serial.println("An Error has occurred while mounting SPIFFS");
		return;
	}

   Serial.println("Running wifi setup");
	// CONNECTING TO EXISTING WIFI NETWORK
	// Connect to Wi-Fi 
//	WiFi.begin(ssid, password); // connect to existing WiFi
//	while (WiFi.status() != WL_CONNECTED) { // While trying to connect
//		delay(1000);
//		Serial.println("Connecting to WiFi.."); // print this statement
//	}
//	// Print ESP32 Local IP Address
//	Serial.println(WiFi.localIP()); // If connected, print the IP address

	// CREATING A SOFT ACCESS POINT
	// Create Wi-Fi
	WiFi.softAP(ssid, password);	// For creating a network / access point

	//// For creating a network
	IPAddress IP = WiFi.softAPIP(); // Create access point
	Serial.print("AP IP address: "); 
	Serial.println(IP); // print IP address

	// Route for root / web page
	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(SPIFFS, "/index.html", String(), false, processor);
	});

	// Route to load style.css file
	server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(SPIFFS, "/style.css", "text/css");
	});

	// Route to load the hsvGraphic file
	//server.on("/hsvGraphic", HTTP_GET, [](AsyncWebServerRequest *request) {
	server.on("/hsvGraphic", HTTP_GET, [](AsyncWebServerRequest *request) {
		//request->send(SPIFFS, "images/hsvGraphic.png", "image/png");
		request->send(SPIFFS, "/images/hsvChart.png", "image/png");
	});

	// Route to load solidcolor page
	server.on("/solidcolor", HTTP_GET, [](AsyncWebServerRequest *request) {
		sendRequest(2);
		currentPattern = "Solid Color (hue, sat, brightness): ( " + inputHue + " , " + inputSat + " , " + inputVal + " )";
		request->send(SPIFFS, "/index.html", String(), false, processor);
	});

	// Route when "on" button is pressed. Also sets GPIO to HIGH (blue light turns on)
	server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request) {
		digitalWrite(ledPin, HIGH);
		//Serial2.write('o');
		sendRequest(1);
		request->send(SPIFFS, "/index.html", String(), false, processor);
	});

	// Route when "off" button is pressed. Also sets GPIO to LOW (blue light turns off)
	server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request) {
		digitalWrite(ledPin, LOW);
		//Serial2.write('f');
		sendRequest(0);
		request->send(SPIFFS, "/index.html", String(), false, processor);
	});
	
	// Send a GET request for the master brightness (when a brightness value is submitted)
	server.on("/masterBrightness", HTTP_GET, [](AsyncWebServerRequest *request) {
		if (request->hasParam("masterBrightnessInput")) {
			inputMasterBrightness = request->getParam("masterBrightnessInput")->value();
		}
		sendRequest(23);
		request->send(SPIFFS, "/index.html", String(), false, processor);
	});

	// Send a GET request for the audio sensitivity control
	server.on("/audioSensitivity", HTTP_GET, [](AsyncWebServerRequest *request) {
		if (request->hasParam("audioSensitivityInput")) {
			inputAudioMultiplier = request->getParam("audioSensitivityInput")->value();
		}
		//Serial.println(currentBrightness);
		sendRequest(99);
		request->send(SPIFFS, "/index.html", String(), false, processor);
	});

	// Send a GET request for the hue
	server.on("/hue", HTTP_GET, [](AsyncWebServerRequest *request) {
		if (request->hasParam("hueInput")) {
			inputHue = request->getParam("hueInput")->value();
		}
		Serial.println(inputHue);
		currentPattern = "Solid Color (hue, sat, brightness): ( " + inputHue + " , " + inputSat + " , " + inputVal + " )";
		sendRequest(2);
		request->send(SPIFFS, "/index.html", String(), false, processor);
	});

	// Send a GET request for the Saturation
	server.on("/sat", HTTP_GET, [](AsyncWebServerRequest *request) {
		if (request->hasParam("satInput")) {
			inputSat = request->getParam("satInput")->value();
		}
		Serial.println(inputSat);
		currentPattern = "Solid Color (hue, sat, brightness): ( " + inputHue + " , " + inputSat + " , " + inputVal + " )";
		sendRequest(3);
		request->send(SPIFFS, "/index.html", String(), false);
	});

	// Send a GET request for the Brightness
	server.on("/val", HTTP_GET, [](AsyncWebServerRequest *request) {
		if (request->hasParam("valInput")) {
			inputVal = request->getParam("valInput")->value();
		}
		Serial.println(inputVal);
		currentPattern = "Solid Color (hue, sat, brightness): ( " + inputHue + " , " + inputSat + " , " + inputVal + " )";
		sendRequest(4);
		request->send(SPIFFS, "/index.html", String(), false);
	});

	// MUSIC VIZUALIZING PATTERN BUTTONS
  // Request 98 makes the patterns automatically switching
	server.on("/mvpat0", HTTP_GET, [](AsyncWebServerRequest *request) {
		sendRequest(98);
		request->send(SPIFFS, "/index.html", String(), false, processor);
	});
	server.on("/mvpat", HTTP_GET, [](AsyncWebServerRequest *request) {
    int pattern = request->getParam("pattern")->value().toInt();
		sendRequest((4 + pattern));
		request->send(SPIFFS, "/index.html", String(), false, processor);
	});
 
	// AMBIENT PATTERN BUTTONS
	server.on("/ambpat", HTTP_GET, [](AsyncWebServerRequest *request) {
    int pattern = request->getParam("pattern")->value().toInt();
		sendRequest((16 + pattern));
		request->send(SPIFFS, "/index.html", String(), false, processor);
	});

	// Start server
	server.begin();

  Serial.printf("\n\nSetup Complete\n");
}

void loop() {

}

int updateCurrentBrightness() {
	currentBrightness = inputMasterBrightness.toInt();
	return currentBrightness;
}

void sendRequest(int req) {
  Serial.printf("sendRequest(%i)\n", req);
	switch (req) {
	case 0:
		currentMode = req;
    sendCommand('f', 0);
		break;
	case 1:
		currentMode = req;
    sendCommand('o', 0);
		break;
	case 2:
		currentMode = req;
    sendCommand('s', inputHue.toInt());
		break;
	case 3:
		currentMode = req;
    sendCommand('t', inputSat.toInt());
		break;
	case 4:
		currentMode = req;
    sendCommand('t', inputVal.toInt());
		break;
	case 5 ... 16:
		sendMVMode(req);
		break;
	case 17 ... 22:
		sendAmbMode(req);
		break;
	case 23:
		//currentMode = req;
    sendCommand('z', inputMasterBrightness.toInt());
		break;
	case 98:
		currentMode = req;
    sendCommand('x', 0);
		break;
	case 99:
    sendCommand('y', inputAudioMultiplier.toInt());
		break;
	}
}

void sendMVMode(int req) {
  Serial.printf("sendMVMode(%i)\n", req);
	currentMode = req;
	sendMode = currentMode - 4;
	sendCommand('m', sendMode);
}

void sendAmbMode(int req) {
  Serial.printf("sendAmbMode(%i)\n", req);
	currentMode = req;
	sendMode = currentMode - 16;
  sendCommand('a', sendMode);
}

void sendCommand(char command, int value) {
  Serial.printf("<%c%03d>\n", command, value);  
  Serial2.printf("<%c%03d>\n", command, value);  
}
