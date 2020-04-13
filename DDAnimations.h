// DDAnimations.h
// This file contains all of the music-visualizing patterns and non-music-visualizing patterns, unless you want to create a new header file for those.

// 
void fullSparkles() {
	maxNumOfSparkles = (height*width) / 2; // for a matrix
	//brightness = 255;
	addSparkles(0, NUM_LEDS, 2, .06, -50, 0, false);
	addSparkles(0, NUM_LEDS, 3, .1, -50, 35, false);
	addSparkles(0, NUM_LEDS, 4, .1, -50, 70, false);
	addSparkles(0, NUM_LEDS, 6, .1, -50, 105, false);
	addSparkles(0, NUM_LEDS, 9, .1, -50, 180, false);
	addSparkles(0, NUM_LEDS, 12, .1, -50, 200, false);

	int fade = (fadeVals[lowAveBin] + fadeVals[midAveBin] + fadeVals[highAveBin]) / 3;
	fade = map(fade, 0, 255, 1, 7) * -1 + 255;
	//Serial.println(noFadeFading);
	fadeAll2(fade);
	Serial.println(fade);

	/*Serial.print(fadeVals[lowAveBin]);
	Serial.print("\t");
	Serial.print(fadeVals[midAveBin]);
	Serial.print("\t");
	Serial.println(fadeVals[highAveBin]);*/
}

// Explodes a new circle on beats in the low frequency
void drawCircles() {
	// These arrays allow for a maximum of 5 circles to be drawn at once
	// TODO: create a struct for them
	static int radIndex = 0; // used to cycle through circles
	static double radius[5]; // radii of circles
	static int hues[5]; // allows each circle to have a different hue
	static double xCenter[5]; // center of circle on x-axis
	static double yCenter[5]; // center of circle on y-axis
	static int thickness = 1; // thickness of the circles
	static int delayCounter = 0; // how slow to increase radii of circles

	if (beatDetected[lowBeatBin] == 2) { // if a beat is detected in the lowBeatBin...
		//EVERY_N_MILLIS(5000) { // This timer can be used instead of beat detection to test this pattern
		radius[radIndex] = 0; // set radius of circle = 0
		hues[radIndex] = random8(); // set a random hue
		xCenter[radIndex] = random8(1, width - 1); // choose xCenter, 1 away from the outside
		yCenter[radIndex] = random8(1, height - 1); // choose yCenter, 1 away from the inside
		radIndex++; // increase the index by 1, so the next beat will change another circle's properties
	}
	if (radIndex == 5) radIndex = 0; // if index = 5 (6th value), reset to 0

	delayCounter++; // increase delay counter
	if (delayCounter > 5) { // every 5 cycles through this code...
		for (int i = 0; i < 5; i++) { // for all the possible circles...
			if (radius[i] < 25) { // if the radius is less than 25 (to prevent circles from growing infinitely big) ...
				radius[i] += 1; // increase by 1
				propagateCircle(CHSV(hues[i], 255, 200), xCenter[i], yCenter[i], radius[i], thickness); // draw the circle on LEDs
			}
		}
		delayCounter = 0; // reset delay counter
	}

	// fade all the LEDs in leds3
	fadeAll3(245);
}

void placeHolder() {

}