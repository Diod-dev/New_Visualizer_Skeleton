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
	server.on("/mvpat1", HTTP_GET, [](AsyncWebServerRequest *request) {
		sendRequest(5);
		request->send(SPIFFS, "/index.html", String(), false, processor);
	});
	server.on("/mvpat2", HTTP_GET, [](AsyncWebServerRequest *request) {
		sendRequest(6);
		request->send(SPIFFS, "/index.html", String(), false, processor);
	});
	server.on("/mvpat3", HTTP_GET, [](AsyncWebServerRequest *request) {
		sendRequest(7);
		request->send(SPIFFS, "/index.html", String(), false, processor);
	});
	server.on("/mvpat4", HTTP_GET, [](AsyncWebServerRequest *request) {
		sendRequest(8);
		request->send(SPIFFS, "/index.html", String(), false, processor);
	});
	server.on("/mvpat5", HTTP_GET, [](AsyncWebServerRequest *request) {
		sendRequest(9);
		request->send(SPIFFS, "/index.html", String(), false, processor);
	});
	server.on("/mvpat6", HTTP_GET, [](AsyncWebServerRequest *request) {
		sendRequest(10);
		request->send(SPIFFS, "/index.html", String(), false, processor);
	});
	server.on("/mvpat7", HTTP_GET, [](AsyncWebServerRequest *request) {
		sendRequest(11);
		request->send(SPIFFS, "/index.html", String(), false, processor);
	});
	server.on("/mvpat8", HTTP_GET, [](AsyncWebServerRequest *request) {
		sendRequest(12);
		request->send(SPIFFS, "/index.html", String(), false, processor);
	});
	server.on("/mvpat9", HTTP_GET, [](AsyncWebServerRequest *request) {
		sendRequest(13);
		request->send(SPIFFS, "/index.html", String(), false, processor);
	});
	server.on("/mvpat10", HTTP_GET, [](AsyncWebServerRequest *request) {
		sendRequest(14);
		request->send(SPIFFS, "/index.html", String(), false, processor);
	});
	server.on("/mvpat11", HTTP_GET, [](AsyncWebServerRequest *request) {
		sendRequest(15);
		request->send(SPIFFS, "/index.html", String(), false, processor);
	});
	server.on("/mvpat12", HTTP_GET, [](AsyncWebServerRequest *request) {
		sendRequest(16);
		request->send(SPIFFS, "/index.html", String(), false, processor);
	});
	// AMBIENT PATTERN BUTTONS
	server.on("/ambpat1", HTTP_GET, [](AsyncWebServerRequest *request) {
		sendRequest(17);
		request->send(SPIFFS, "/index.html", String(), false, processor);
	});
	server.on("/ambpat2", HTTP_GET, [](AsyncWebServerRequest *request) {
		sendRequest(18);
		request->send(SPIFFS, "/index.html", String(), false, processor);
	});
	server.on("/ambpat3", HTTP_GET, [](AsyncWebServerRequest *request) {
		sendRequest(19);
		request->send(SPIFFS, "/index.html", String(), false, processor);
	});
	server.on("/ambpat4", HTTP_GET, [](AsyncWebServerRequest *request) {
		sendRequest(20);
		request->send(SPIFFS, "/index.html", String(), false, processor);
	});
	server.on("/ambpat5", HTTP_GET, [](AsyncWebServerRequest *request) {
		sendRequest(21);
		request->send(SPIFFS, "/index.html", String(), false, processor);
	});
	server.on("/ambpat6", HTTP_GET, [](AsyncWebServerRequest *request) {
		sendRequest(22);
		request->send(SPIFFS, "/index.html", String(), false, processor);
	});

	// Start server
	server.begin();
}

void loop() {

}

int updateCurrentBrightness() {
	currentBrightness = inputMasterBrightness.toInt();
	return currentBrightness;
}

void sendRequest(int req) {
	switch (req) {
	case 0:
		currentMode = req;
		Serial2.write('<');
		Serial2.write('f');
		Serial2.write('0');
		Serial2.write('0');
		Serial2.write('0');
		Serial2.write('>');
		break;
	case 1:
		currentMode = req;
		Serial2.write('<');
		Serial2.write('o');
		Serial2.write('0');
		Serial2.write('0');
		Serial2.write('0');
		Serial2.write('>');
		break;
	case 2:
		currentMode = req;
		Serial2.write('<');
		Serial2.write('s');
		for (int i = 0; i < inputHue.length(); i++) {
			Serial2.write(inputHue[i]);
		}
		Serial2.write('>');
		break;
	case 3:
		currentMode = req;
		Serial2.write('<');
		Serial2.write('t');
		for (int i = 0; i < inputSat.length(); i++) {
			Serial2.write(inputSat[i]);
		}
		Serial2.write('>');
		break;
	case 4:
		currentMode = req;
		Serial2.write('<');
		Serial2.write('b');
		for (int i = 0; i < inputVal.length(); i++) {
			Serial2.write(inputVal[i]);
		}
		Serial2.write('>');
		break;
	case 5:
		currentMode = req;
		sendMVMode(req);
		break;
	case 6:
		currentMode = req;
		sendMVMode(req);
		break;
	case 7:
		currentMode = req;
		sendMVMode(req);
		break;
	case 8:
		currentMode = req;
		sendMVMode(req);
		break;
	case 9:
		currentMode = req;
		sendMVMode(req);
		break;
	case 10:
		currentMode = req;
		sendMVMode(req);
		break;
	case 11:
		currentMode = req;
		sendMVMode(req);
		break;
	case 12:
		currentMode = req;
		sendMVMode(req);
		break;
	case 13:
		currentMode = req;
		sendMVMode(req);
		break;
	case 14:
		currentMode = req;
		sendMVMode(req);
		break;
	case 15:
		currentMode = req;
		sendMVMode(req);
		break;
	case 16:
		currentMode = req;
		sendMVMode(req);
		break;
	case 17:
		currentMode = req;
		sendAmbMode(req);
		break;
	case 18:
		currentMode = req;
		sendAmbMode(req);
		break;
	case 19:
		currentMode = req;
		sendAmbMode(req);
		break;
	case 20:
		currentMode = req;
		sendAmbMode(req);
		break;
	case 21:
		currentMode = req;
		sendAmbMode(req);
		break;
	case 22:
		currentMode = req;
		sendAmbMode(req);
		break;
	case 23:
		//currentMode = req;
		Serial2.write('<');
		Serial2.write('z');
		for (int i = 0; i < inputMasterBrightness.length(); i++) {
			Serial2.write(inputMasterBrightness[i]);
		}
		Serial2.write('>');
		break;
	case 98:
		currentMode = req;
		Serial2.write('<');
		Serial2.write('x');
		Serial2.write('0');
		Serial2.write('0');
		Serial2.write('0');
		Serial2.write('>');
		break;
	case 99:
		Serial2.write('<');
		Serial2.write('y');
		for (int i = 0; i < inputAudioMultiplier.length(); i++) {
			Serial2.write(inputAudioMultiplier[i]);
		}
		Serial2.write('>');
		break;
	}
}

void sendMVMode(int req) {
	currentMode = req;
	Serial2.write('<');
	Serial2.write('m');
	sendMode = currentMode - 4;
	sendModeStr = (String)sendMode;
	for (int i = 0; i < sendModeStr.length(); i++) {
		Serial2.write(sendModeStr[i]);
	}
	Serial2.write('>');
}

void sendAmbMode(int req) {
	currentMode = req;
	Serial2.write('<');
	Serial2.write('a');
	sendMode = currentMode - 16;
	sendModeStr = (String)sendMode;
	for (int i = 0; i < sendModeStr.length(); i++) {
		Serial2.write(sendModeStr[i]);
	}
	Serial2.write('>');
}
