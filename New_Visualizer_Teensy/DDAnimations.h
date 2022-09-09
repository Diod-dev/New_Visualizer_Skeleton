// DDAnimations.h
// This file contains all of the music-visualizing patterns and non-music-visualizing patterns, unless you want to create a new header file for those.

// Sparkles to the whole frequency spectrum
void sparkles() {
  // For every other bin across the whole frequency spectrum (i+=2). This keeps there from being too many sparkles.
  for (int i = 0; i < numFFTBins; i+=2) {
    // Calculate the number of sparkles to add depending on the spectrumValue[] data. The -50 stops sparkles from appearing for quiet sound.
    int numOfSparkles = constrain(map(spectrumValue[i] - 50, 0, 500, 0, 10), 0, 10); // Put a ceiling on how high it can go if there's a very high peak
    // For as many sparkles that we need to add...
    for (int j = 0; j < numOfSparkles; j++) 
      // draw the sparkles at a random location, map the hue to the frequency bin
      leds2[ random8() ] = CHSV(i * 10, 255, 255);
  }

  // Fade
	fadeAll2(250);
}

// Animation where every beat in the low frequencies triggers a new firework
void fireworks() {
    // Initialize variables for this animation
    static double radii[10];
    static double radiiX[10], radiiY[10];
    static int index;

    // This is where new fireworks are created every time there's a beat
    // If beat detected, create a new circle
    if (beatDetected[lowBeatBin] == 2) {
        // Set the radius to 0
        radii[index] = 0;
        // Find a new random location for the center point
        radiiX[index] = random8(3, 13);
        radiiY[index] = random8(3, 13);
        // Increment the index variable so for the next beat detected, it resets the next radius in radii[]
        index++;
        // If index has been incremented above the size of our arrays, reset it at 0
        if (index == 10) index = 0;
    }

    // This is where the circles get drawn to the LEDs
    // For all the possible radii in our radii[] array
    for (int i = 0; i < 10; i++) {
        // Call our drawCircle() function
        drawCircle(radiiX[i], radiiY[i], 0, 2, radii[i]);
    }

    // This is where the fireworks grow
    // For all the potential radii...
    for (int i = 0; i < 10; i++) {
      // check to see if the radii are still low enough to be growing (if they're still on the matrix, they need to keep growing)
      if (radii[i] < 20)
         // If they are, add __ to the radius
         radii[i] += .2;
    }

  // Fade
  fadeAll3(200);
}

// Animation where the whole frequency range is visualized as colors falling across the matrix
void fallingMusic() {

  // This is where the light is drawn to the LEDs
  // For all the columns...
  for (int i = 0; i < width; i++) {
    // Calculate the brightness from the spectrumValue[] data
    int fallingBrightness = constrain(map(spectrumValue[i] - 50, 0, 600, 0, 255), 0, 255);
    // Draw that brightness to the column
    leds2[ coords[i][height - 1] ] += CHSV(map(i, 0, 15, 0, 220), 255, fallingBrightness);
  }

  // This is where all the lights are moved down
  // Every 50 milliseconds...
  EVERY_N_MILLIS(50) { // this slows the speed at which the lights fall
    // For every row on the matrix...  (except the top one)
    for (int i = 0; i < height - 1; i++) {
      // Go across each column...
      for (int j = 0; j < width; j++) {
        // Save the LED color from (x, y+1) to (x,y). This moves the color downwards
        leds2[coords[j][i]] = leds2[coords[j][i + 1]]; // set the LED equal to the one above it
      }
    }
    // Every time we move the LEDs down, fade the colors on the top row.
    for (int i = 0; i < width; i++)
      // Apply the .nscale8() fade from fadeAll() to just the top row
      leds2[coords[i][height - 1]].nscale8(170); 
  }
}

//// This code works when the matrix is a square
//// These spectrum analyzer bars can go in the classic vertical direction (dir = true), or out from the middle (dir = false)
void spectrumAnalyzer() {
  // Lenghts of all the spectrum analyzer bars
  static int barLength[16] = { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 }; 
  // Locations of the white dots above each bar
  static int topDot[16] = { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 };

  // This is where all the bar lengths are calculated and set
  // For all the FFT bins (numFFTBins = 16, which happens to be the number of columns on the 16x16 matrix)
  for (int i = 0; i < numFFTBins; i++) {
    // Calculate the bar's height by mapping that FFTbin's value to the matrix, while constraining it to keep it in bounds
    int tempBarLength = constrain(map(spectrumValue[i] - 50, 0, 600, 0, height - 2), 0, height - 2);
    // If the bar length just calculated is higher than the one currently drawn on the matrix...
    if (tempBarLength > barLength[i]) {
      // Save the new bar length to the column
      barLength[i] = tempBarLength;
      // If the new bar length is taller than the dot...
      if (barLength[i] > topDot[i])
        // move the dot to 1 higher than the new bar length
        topDot[i] = barLength[i] + 1;
    }
  }

  // This is where the bar lengths decreases
  // Every N milliseconds...
  EVERY_N_MILLIS(100) {
    // Go through all the bar lengths and...
    for (int i = 0; i < numFFTBins; i++) {
      // If they're greater than 1...
      if (barLength[i] > 1)
        // Decrement them
				barLength[i]--;
		}
	}

  // This is where the top dots decrease
  // Check comments above for decreasing bar lengths
  EVERY_N_MILLIS(200) {
		for (int i = 0; i < numFFTBins; i++) {
			if (topDot[i] > 1)
				topDot[i]--;
		}
	}

  // This is where the bars are drawn to the LEDs
  // For all the columns of the matrix...
	for (int i = 0; i < width; i++) {
    // On each column, for as tall as the bar is...
		for (int j = 0; j < barLength[i]; j++) {
      // Draw color to the bar, however long it is
			//leds[coords[i][j]] = CHSV(i * (230 / numFFTBins), 255, 250); // rainbow goes left to right
			leds2[coords[i][j]] = CHSV(j * (230 / numFFTBins), 255, 250); // rainbow goes bottom to top
			leds2[coords[i][topDot[i]]] = CHSV(0, 0, 150);
		}
	}

  // Fade
  fadeAll2(200);
}

void sweeps() {
  static int locations[10];
  static int index;
  // Initializing the location of the sweeps
  if (beatDetected[lowBeatBin] == 2) {
    locations[index] = 0;
    index++;
    if (index == 10) index = 0;
  }

  // Display the sweeps
  // For all the 10 potential sweep locations...
  for (int i = 0; i < 10; i++) { 
    // If the sweep location is less than the number of rows on the matrix, then draw it
    if (locations[i] < height) {
        // Draw the sweep at x = location[i] for all the columns on the matrix
        for (int j = 0; j < width; j++)
            leds3[ coords[ locations[i] ][j] ] = CHSV(locations[i] * 12, 255, 255);
    }
  }

  // Move the sweeps
  // Every 50 milliseconds...
  EVERY_N_MILLIS(50) {
    // for all the potential sweep locations,
    for (int i = 0; i < 10; i++) {
        // check to see if they are still on the matrix
        if (locations[i] < height)
            // if they are, increment them
            locations[i]++;
    }
  }

  // Fade all the LEDs
  fadeAll3(230);
}

void placeHolder() {

}
