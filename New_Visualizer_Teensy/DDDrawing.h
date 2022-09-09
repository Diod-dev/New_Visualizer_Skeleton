// DDDrawing.h
// This file contains functions used by DDAnimations.h to ultimately write color data to the 'leds', 'leds2', and 'leds3' arrays
// TODO: add more common drawing functions

void drawCircle(double centerX, double centerY, int hue, double thickness, double radii) {
  if (radii < 16) {
    // for all the rows...
    for (int i = 0; i < width; i++) {
      // for all the columns...
      for (int j = 0; j < height; j++) {
        // Find the distance between the led at (i,j) and the point (centerX, centerY)
        int dist = sqrt( pow(i - centerX, 2) + pow(j - centerY, 2) );
        // if the distance we just calculated is within a certain range (thickness)...
        if (dist > radii - thickness/2 && dist < radii + thickness/2)
          // Apply color to this LED
          leds3[ coords[i][j] ] += CHSV(radii*10, 255, 255);
      }
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
