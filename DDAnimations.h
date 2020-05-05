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
	addSparkles(0, NUM_LEDS, 14, .1, -50, 200, false);

	int fade = (fadeVals[lowAveBin] + fadeVals[midAveBin] + fadeVals[highAveBin]) / 3;
	fade = map(fade, 0, 255, 0, 30) * -1 + 255;
	fadeAll(254);

	//Serial.println(fade);
	//Serial.print("\t");
	//Serial.print(fadeVals[lowAveBin]);
	//Serial.print("\t");
	//Serial.print(fadeVals[midAveBin]);
	//Serial.print("\t");
	//Serial.println(fadeVals[highAveBin]);
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

// fallingColorCreation() creates the lights for fallingMusic().
void fallingColorCreation(int freq) {
	static int fallingBrightness;
	static int threshold = 20;
	if (spectrumValue[freq] > threshold) {
		// constrain value between 0 and average + 3*stDev, map it to 0 - 255
		fallingBrightness = map(constrain(spectrumValue[freq] - 100, 0, average[freq] + 3 * stDev[freq]), 0, average[freq] + 3 * stDev[freq], 0, 255);
		fallingBrightness = constrain(fallingBrightness, 0, 255);
		//TODO: Make this work off of palettes
		leds[coords[map(freq, 0, 15, 0, width - 1)][height - 1]] += CHSV(map(freq, 0, 15, 0, 220), 255, fallingBrightness);
	}
}

void fallingMusic() {

	// Fade the top row of LEDs instead of turning them off so if they aren't moved down, they don't blink away really quick
	for (int i = 0; i < width; i++)
		leds[coords[i][height - 1]].nscale8(170); // pretty fast fade for only the top row

	// For every FFT bin, map the brightness of the light to the spectrumValue
	// TODO: Figure out why this for loop makes the LEDs all jittery and broken
	//for (int i = 0; i < 14; i++) 
	//	fallingColorCreation(i);

	fallingColorCreation(0);
	fallingColorCreation(1);
	fallingColorCreation(2);
	fallingColorCreation(3);
	fallingColorCreation(4);
	fallingColorCreation(5);
	fallingColorCreation(6);
	fallingColorCreation(7);
	fallingColorCreation(8);
	fallingColorCreation(9);
	fallingColorCreation(10);
	fallingColorCreation(11);
	fallingColorCreation(12);
	fallingColorCreation(13);
	fallingColorCreation(14);
	fallingColorCreation(15);

	// Move all of the lights down
	EVERY_N_MILLIS(50) { // this slows the speed at which the lights fall
		for (int i = 0; i < height - 1; i++) { // for all the rows except the top one
			for (int j = 0; j < width; j++) { // for all the columns
				leds[coords[j][i]] = leds[coords[j][i + 1]]; // set the LED equal to the one above it
			}
		}
	}
}

//// These next 2 functions are for XY arrays that are larger than 10x10
//// This function sets bar lengths and top dot locations
int barLength[16] = { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 }; 
int topDot[16] = { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 };
void spectrumAnalyzerBars(int freq, int pillar, bool vert) {
	int preBarLength;
	if (vert) preBarLength = constrain(map(spectrumValue[freq], 50, 600, 0, height - 2), 0, height - 2);
	else preBarLength = constrain(map(spectrumValue[freq], 50, 600, 0, width / 2 + 1), 0, (width / 2 - 1)); // '-1' levaes room for topDot

	if (preBarLength > barLength[pillar]) {
		barLength[pillar] = preBarLength;
		if (barLength[pillar] > topDot[pillar])
			topDot[pillar] = barLength[pillar] + 1;
	}
}

//// This code works when the matrix is a square
//// These spectrum analyzer bars can go in the classic vertical direction (dir = true), or out from the middle (dir = false)
void spectrumAnalyzer(bool dir) {
	static int hue;
	hue++;
	// Fade
	fadeAll(200);

	static int barDropTimer;
	static int dotDropTimer;

	for (int i = 0; i < 16; i++) {
		if (i != 8) // Don't ask me why this works, but if the LEDs are glitching out, removing the 8th bar fixed it... 
			spectrumAnalyzerBars(i, i, dir);
	}

	// Using Every_N_Mllis for these totally fucked up the LEDs
	if (barDropTimer > 5) {
		for (int i = 0; i < numFFTBins; i++) {
			if (barLength[i] > 1)
				barLength[i]--;
		}
		barDropTimer = 0;
	}
	barDropTimer++;

	// Making the dots fall
	if (dotDropTimer > 15) {
		for (int i = 0; i < numFFTBins; i++) {
			if (topDot[i] > 1)
				topDot[i]--;
		}
		dotDropTimer = 0;
	}
	dotDropTimer++;

	// Draw bars and dots
	if (dir) { // for vertical bars
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < barLength[i]; j++) {
				//leds[coords[i][j]] = CHSV(i * (230 / numFFTBins), 255, 250); // rainbow goes left to right
				leds[coords[i][j]] = CHSV(j * (230 / numFFTBins), 255, 250); // rainbow goes bottom to top
				leds[coords[i][topDot[i]]] = CHSV(0, 0, 150);
			}
		}
	}
	else { // for horizontal bars (if these are glitchingo out, comment out all the code just above this if/else that makes the dots fall, no idea why this fixes it...
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < barLength[i]; j++) {
				// To see different colors, a couple options are taking out 'hue' and changing 'j' to 'i'
				leds[coords[(width/2) +     j][i]] = CHSV(hue + j * (230 / height), 255, 250); // bars to the right
				leds[coords[(width/2) - 1 - j][i]] = CHSV(hue + j * (230 / height), 255, 250); // bars to the left
				// These 2 lines draw the white dots to the left and right, but I think it looks better without them
				//leds[coords[(width / 2 ) + topDot[i] - 1][i]] = CHSV(0, 0, 150); // dots to the right
				//leds[coords[(width / 2 ) - topDot[i]][i]] = CHSV(0, 0, 150); // dots to the left
			}
		}
	}
}

void placeHolder() {

}
