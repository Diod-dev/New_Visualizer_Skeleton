/*
  This is a blank project with everything in place ready to become a music-visualizer on Diod.design's PCB.
*/

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))
#include <Arduino.h>
// These OCTOWS2811 library lines must be before #include FastLED
#define USE_OCTOWS2811
#include <OctoWS2811.h>
#include <FastLED.h>
// Statistic.h library is used for music analytics
#include <Statistic.h>
// For the 2 buttons on the PCB.
#include <Button.h>
#define buttonPin1 15
#define buttonPin2 16
Button button1(buttonPin1);
Button button2(buttonPin2);

// "mult" is the multiplier that can be adjusted over WiFi
double mult = 1.00;
// "DDAudio.h" is Diod.design's music-analytic code.
#include "DDAudio.h"

// NUM_LEDS is the total number of LEDs you have on 1 strip, if you're only using 1 LED output (1 LED strip/string)
// ** If you're using multiple LED outputs, NUM_LEDS is not used when "LEDS.addLeds..." is called in setup() **
// ** If you're using multiple LED outputs, use NUM_LEDS_PER_STRIP and NUM_STRIPS **
#define NUM_LEDS 256
#define NUM_LEDS_PER_STRIP 256
// NUM_STRIPS controls how many of the OCTOWS2811 output pins will drive LEDs, in this order : 2, 14, 7, 8, 6, 20, 21, 5
#define NUM_STRIPS 3

// "leds" is the LED data that is actually pushed out to the LEDs
CRGB leds[NUM_STRIPS * NUM_LEDS_PER_STRIP];
// leds2 is populated by non-beat-reactive patterns
CRGB leds2[NUM_STRIPS * NUM_LEDS_PER_STRIP];
// leds3 is populated by beat-reactive patterns
CRGB leds3[NUM_STRIPS * NUM_LEDS_PER_STRIP];
// 'leds2' and 'leds3' are blended in to 'leds'. If there is a beat present in the music, 'leds' essentially equals 'leds3'. If not, 'leds' = 'leds2'

// if LEDs are arranged as a matrix, put the number of rows in 'height' and number of columns in 'width'
const int height = 16;
const int width = 16;
int coords[width][height];

#include "DDGradients.h"
CRGBPalette16 currentPalette = normal_p; // for setting the first/default palette

// Variables used in setting patterns and brightness (can be modified over WiFi)
int patternMode = 1;
int patternModeOld;
int ambPattern;
int MVPattern;
int masterBrightness = 5;

#include "DDDrawing.h" // Must be above Animatinos
#include "DDAnimations.h"
#include "DDESPcomm.h"
#include "DDPatternLists.h"

// The setup function is only run once, as soon as the Teensy is powered on
void setup()
{
	// Useful to see if the Teensy crashes during setup
	Serial.println("Beginning Setup");

	// Initialize the buttons
	button1.begin();
	button2.begin();

	// This function in 'DDAudio.h' sets the input (aux/mic), and configures the audio adapter
	audioSetup();

	Serial.begin(115200);
	Serial1.begin(115200); // Used for ESP communication to the Teensy's RX1/TX1 pins
	delay(100);

	LEDS.addLeds<OCTOWS2811>(leds, NUM_LEDS); // Initalize LEDs if only 1 strip is used
	//LEDS.addLeds<OCTOWS2811>(leds, NUM_LEDS_PER_STRIP); // Initialize LEDs if more than 1 strip is used

	FastLED.clear(true); // clear all LEDs and push to strip (makes it all black)
	delay(1000); // make it easier on the eyes by waiting a second before they turn back on

	// Limit power draw of LEDs. 5V and 10,000 mA
	FastLED.setMaxPowerInVoltsAndMilliamps(5, 10000);

	// If there's a mapping function used to set (x,y) or (x,y,z) coordinates for each LED pixel, put here:
	// mapXY() is a function that sets an LED # to every x,y coordinate on a matrix.
	mapXY();

	// Writes to the serial monitor that the setup completed.
	Serial.println("Setup Complete!");
}

void loop()
{
	// Sets masterBrightness over all of LEDs. Updated through Wifi
	LEDS.setBrightness(masterBrightness);

	// Listen for messages from ESP-32 and do things with them
	talkToESP();

	// Music Analysis code
	musicAnalytics();

	// This is the function that makes it all work. When testing new patterns or functions, comment out normalOperation() and just put it below.
	//normalOperation();

	// Just display spectrum analyzer (not placed in leds2 or leds3 yet)
	spectrumAnalyzer(true);

	// Just display falling music (not placed in leds2 or leds3 yet)
	//fallingMusic();

	// Just display fullSparkles (in leds2, so use blend2(255) to copy it from leds2[] to leds[]
	//fullSparkles();
	//blend2(255);

	// Just display circles to beat ()
	//drawCircles();
	//blend3(255);
	

	// * Diagnostic functions if you need to test something *
	//printSpectrum(); // prints the whole spectrum. Useful to see if music is being input correctly.
	//testingInputButtons(); // prints whether the buttons are pressed or not
	//for (int i = 0; i < NUM_LEDS; i++) leds[i] = CHSV(0, 255, 100);
	//Serial.println("working");

	// This sets the maximum FPS to 120, which is plenty high. Without this, some minor code changes make the LEDs go haywire. Unsure why.
	FastLED.setMaxRefreshRate(120);
	// This actually writes LED data to LED strip. Without this line, the LEDs will not light up.
	FastLED.show();
}

void normalOperation() {
	switch (patternMode) {
	case 0:
		LEDS.clear(true);
		break;
	case 1:
		autoMusicVisualizing();
		break;
	case 2:
		staticMusicVizPattern();
		break;
	case 3:
		ambientPattern();
		break;
	case 4:
		solidColor();
		break;
	}

}

// This function automatically mixes leds2 and leds3 into leds as the song's beat comes in and drops out
void autoMusicVisualizing() {
	// A local variable to remember the last 
	static int mixAmountOld;
	mixAmountOld = mixAmount;

	// While a beat is present (mixAmount = 0), change the color palette used in non-beat pattern
	// "mixAmountOld != 255" is used so it only changes the color palette once
	if (mixAmount == 255 && mixAmountOld != 255) {
		currentPaletteNumber = random8(gGradientPaletteCount); // generate random number
		currentPalette = gGradientPalettes[currentPaletteNumber]; // Use that random number to assign a color palette

		nextMusicWithNoBeatPattern(); // chooses new non-beat pattern
	}

	// While no beat is present (mixAmount = 255), change the beat-dependent pattern
	// "mixAmountOld != 0" is used so it only chooses a new pattern once
	if (mixAmount == 0 && mixAmountOld != 0) 
		nextLowBeatPattern(); // chooses new beat-dependent pattern
	
	// When non-beat patterns are not color-specific, their look can be changed by having colors scroll through.
	// This randomly chooses whether to scroll the colors or keep them still
	//if (mixAmount == 255 && mixAmountOld != 255) changingHue = random8(2);
	
	// This is where the music-visualizing patterns are written to leds2 and leds3
	// Or if there is silence, populate leds with an ambient pattern

	if (silence) {
		ambientPatterns[currentAmbPatternNumber]();
	}
	else {
		// If there's a constant beat, populate leds3 with a constant beat pattern
		// TODO: make this another pattern list, and make it fade in
		if (constBeat)
			drawCircles();
		// else, populate leds3 with a non-constant beat pattern
		else
			lowBeatPatterns[currentLowBeatPatternNumber]();

		// Populate leds2 with non-beat pattern
		musicWithNoBeatPatterns[musicWithNoBeatPatternNumber]();
	}

	// blends leds3 in to leds. If a beat is present, mixAmount = 255 and leds = leds3.
	blend3(mixAmount);
	// blends leds2 in to leds. If a beat is NOT present, mixAmount = 0, and leds = leds2.
	blend2((mixAmount - 255) * -1);
	//blend2(255);
} // end autoMusicVisualizing()

// 1 music-visualizing pattern is chosen to be displayed over WiFi
void staticMusicVizPattern() {
	switch (MVPattern) {
	case 1:
		placeHolder();
		break;
	}
}

// 1 ambient pattern is chosen to be displayed over WiFi
void ambientPattern() {
	switch (ambPattern) {
	case 1:
		placeHolder();
		break;
	}
}

// 1 solid color is chosen to be displayed over Wifi
void solidColor() {
	for (int i = 0; i < NUM_LEDS; i++)
		leds[i] = CHSV(solidHue, solidSat, solidVal);
}

void testingInputButtons() {
	Serial.print(button1.read());
	Serial.print("\t");
	Serial.println(button2.read());
}

// Mapping function for an XY matrix where the LED path snakes back and fourth
void mapXY() {
	static int index = 0; // Keeps track of which LED is next

	for (int i = 0; i < height; i++) { // On every row...
		for (int j = 0; j < width; j++) { // For every column...
			if (i % 2 == 0) { // If it's going towards the right
				coords[j][i] = index; // The LED we're on is applied to that (x,y) coordinate
				index++; // move to next LED
			}
			else { // If it's going towards the left...
				coords[width - 1 - j][i] = index;
				index++;
			}
		}
	}
}
