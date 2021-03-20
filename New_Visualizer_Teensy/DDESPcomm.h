// DDESPcomm.h
// This file, uploaded on the Teensy, handles all incoming messages from ESP-32
// Reads and acts on incoming messages from ESP - 32, in the format of '< (letter) (3 digit number) >'. For exmaple, <b200>

// Variables for solid color
uint8_t solidHue = 0;
uint8_t solidSat = 255;
uint8_t solidVal = 255;

const byte numChars = 10;
char receivedChars[10];

// TODO: use 'categories' instead of letters. 
// variables to hold parsed data
//char category[1] = { 0 };
//int instruction = 0;

bool newCommand = false;

int audioMultiplier; // temp input variable before recording this input in variable 'mult'

void doThingsWithCommands() {

  char tempChars[3];

  char command = receivedChars[0];
  for (int i = 0; i < 3; i++) { // Record the next 3 characters (numbers) to a holding string 'tempChars'
    tempChars[i] = receivedChars[i + 1];
  }
  int commandValue = atoi(tempChars); // convert 'tempChars' to integer.
  Serial.printf("New command [%c] value[%i]\n", command, commandValue);

  switch (command) {
    case 'a': // Chooses an ambient pattern and displays it
      ambPattern = commandValue;
      patternMode = 3; // Used in normalOperation()
      Serial.println(ambPattern);
      break;

    case 'b':  // Sets the brightness when displaying a solid color
      patternMode = 4; // Used in normalOperation() to display a solid color
      solidVal = commandValue;
      break;

    case 'f': // The off button is set, last pattern mode is saved.
      patternModeOld = patternMode;
      patternMode = 0; // Used in normalOperation() to turn all LEDs off
      break;

    case 'm': // Chooses a music-visualizing pattern and displays it
      MVPattern = commandValue;
      patternMode = 2; // Used in normalOperation() to display a single music-visualizing pattern
      Serial.println(patternMode);
      break;

    case 'o':
      Serial.println(patternModeOld);
      patternMode = patternModeOld; // Used in normalOperation(), returns to last patternMode
      break;

    case 's':
      patternMode = 4; // Used in normalOperation() to display a solid color
      solidHue = commandValue;
      break;

    case 't':
      patternMode = 4; // Used in normalOperation() to display a solid color
      solidSat = commandValue;
      break;

    case 'x':
      patternMode = 1; // Used in normalOperation() to enter mode of changing music-visualization patterns to the music
      break;

    case 'y':
      audioMultiplier = commandValue;
      mult = (double)audioMultiplier / 100;
      break;

    case 'z':
      masterBrightness = commandValue;
      break;

    default:
      Serial.printf("Unknown input %c\n", command);
      break;
  }
}

// Reads incoming message from ESP-32, in the format of '< (letter) (3 digit number) >'. For exmaple, (<b200>)
void recWithStartEndMarkers() {
	static boolean recInProgress = false; // is a message currently being received?
	static byte ndx = 0; // index variable as receivedChars[] array populates
	char startMarker = '<'; // the first character sent by ESP-32 in a message
	char endMarker = '>'; // the last character sent by ESP-32 in a message
	char rc; // rc = each character as it comes in

	// While serial1 data is available and newCommand == false (the endMarker '>' hasn't been received yet)
	while (Serial1.available() && newCommand == false) {
		rc = Serial1.read(); // rc = new incoming character
		if (recInProgress == true) {
			if (rc != endMarker) {
				receivedChars[ndx] = rc;
				ndx++;
				//if (ndx >= numChars) // THESE 2 LINES FUCK UP THE LEDS
				//	ndx = numChars - 1;
			}
			else {
				receivedChars[ndx] = '\0';
				recInProgress = false;
				ndx = 0;
				newCommand = true;
			}
		}
		else if (rc == startMarker)
			recInProgress = true;
		else {}
	}
}

void parseData() {
	if (newCommand == true) {
		for (int i = 0; i < 4; i++) 
			Serial.print(receivedChars[i]);
		
		Serial.println();

		doThingsWithCommands();

		newCommand = false;
	}
}

void talkToESP() {
	recWithStartEndMarkers(); // constantly looking out for new messages
	parseData(); // after a message is received, act on it
}