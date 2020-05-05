// DDDrawing.h
// This file contains functions used by DDAnimations.h to ultimately write color data to the 'leds', 'leds2', and 'leds3' arrays
// TODO: add more common drawing functions

// How to use the following addSparkles() function:
// - pick a section of leds that you want the sparkles to show up in, between "sparkStart" and "sparkEnd"
// - pick an FFT bin that you want the sparkles to react to, "freq"
// - the "multiplierAdjustment" and "additionAdjustment" variables change how many sparkles appear
//   - "additionAdjustment" stops low volume noise from creating sparkles. If the FFT bin value is 20, and additionAdjustment is -50, then no sparkles show
// - "hue" chooses the color of the sparkles
// - if you want sparkles to be white, as in a color that cannot be choosen with a hue, put true for that argument
int maxNumOfSparkles = 25; // The default maximum number of sparkles you'd like to appear if there's a very high beat. Usually ~75% of total
void addSparkles(int sparkStart, int sparkEnd, uint8_t freq, double multiplierAdjustment, int additionAdjustment, int hue, bool white) {
	// If sparkles are being added to react to music, do this:
	static int numOfSparkles;
	static int tempVar;
	int sparkleBrightness;

	// If the freq argument is a valid FFT bin, 0 - 15
	if (freq > 0 && freq < 15) {
		// Calculate number of sparkles to show up depending on FFT data
		numOfSparkles = spectrumValue[freq] + additionAdjustment; // first, apply Addition Adjustment
		numOfSparkles = constrain(numOfSparkles, 0, 500); // Put a ceiling on how high it can go if there's a very high peak
		sparkleBrightness = map(numOfSparkles, 0, 500, 0, 255); // Map this value to the brightness. If it's at the ceiling, LEDs will be brightest
		numOfSparkles = map(numOfSparkles, 0, 500, 0, maxNumOfSparkles); // Scale back how many sparkles will show up. Depends on how big the section of LEDs is
		numOfSparkles = numOfSparkles * multiplierAdjustment; // Apply multiplier adjustment

		// Use the number of sparkles calculated above to run a 'for' loop and apply color to that many LEDs
		for (int i = 0; i < numOfSparkles; i++) {
			int pos = random16(sparkStart, sparkEnd);
			leds2[pos] = ColorFromPalette(currentPalette, hue, sparkleBrightness);
		}
	}
	// Else, sparkles are being added as an ambient effect...
	else {
		EVERY_N_MILLISECONDS(additionAdjustment) { // additionAdjustmnet can be used to control how many sparkles appear in ambient effect
			int pos = random16(sparkStart, sparkEnd);
			leds[pos] = CHSV(hue, 255, freq); // 'freq' can be used to set the brightness since it'll almost definitely won't be in 0 - 15
			if (white) leds[pos] = CHSV(0, 0, freq); 
		}
	}
}

// Draws a circle with a given 'thickness' and radius 'r', with a center at (x,y), in a certain CHSV 'color'
void propagateCircle(CHSV color, double x, double y, double r, int thickness) {
	static int sdx, edx;
	static int sdy, edy;

	// This dx/dy stuff is not necessary for small arrays, but for a 3D cube with hundreds more LEDs to light, it can make
	// a huge difference in efficiency if the for loops go through unnecessary loops, so the beginning and ending points of 
	// the for loops are calculated based off of the radius being input, not just 0 to width, and 0 to height.
	// check to make sure it's contained in bounds of LEDs
	sdx = 0; // start of x values
	edx = width; // end of x values (10 is a place holder)
	sdy = 0; // start of y values
	edy = height; // end of y values (10 is a place holder)

	for (int dx = sdx; dx < edx; dx++) { // From start of x values, to the end of x values...
		for (int dy = sdy; dy < edy; dy++) { // From start of y values, to the end of y values...
			if (abs(sqrt(pow(x - dx, 2) + pow(y - dy, 2)) - r) < thickness) 
				leds3[coords[dx][dy]] += color;
		}
	}
}

// Blends leds2 in to leds in a certain amount. If mixAmount = 255, leds = leds2. If mixAmount = 0, none of leds2 will be in leds
void blend2(int mixAmount) {
	for (int i = 0; i < NUM_LEDS; i++) 
		nblend(leds[i], leds2[i], mixAmount);
}
// Blends leds3 in to leds in a certain amount. If mixAmount = 255, leds = leds3. If mixAmount = 0, none of leds3 will be in leds
void blend3(int mixAmount) {
	for (int i = 0; i < NUM_LEDS; i++)
		nblend(leds[i], leds3[i], mixAmount);
}

// Fades all the LEDs at speed 'fadeVar'
void fadeAll(int fadeVar) {
	for (int i = 0; i < NUM_LEDS; i++) leds[i].nscale8(fadeVar);
}
void fadeAll2(int fadeVar) {
	for (int i = 0; i < NUM_LEDS; i++) leds2[i].nscale8(fadeVar);
}
void fadeAll3(int fadeVar) {
	for (int i = 0; i < NUM_LEDS; i++) leds3[i].nscale8(fadeVar);
}

// Fades the leds between arguments 'start' and 'stop' and speed 'fadeVar'
void fadeRange(int fadeVar, int start, int stop) {
	for (int i = start; i < stop; i++) leds[i].nscale8(fadeVar);
}
void fadeRange2(int fadeVar, int start, int stop) {
	for (int i = start; i < stop; i++) leds2[i].nscale8(fadeVar);
}
void fadeRange3(int fadeVar, int start, int stop) {
	for (int i = start; i < stop; i++) leds3[i].nscale8(fadeVar);
}
