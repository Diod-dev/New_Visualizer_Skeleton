// DDPatternLists.h
// This file holds the lists of functions that can be chosen from during different song scenarios (beat present, no beat present, no music)

uint8_t musicWithNoBeatPatternNumber = 0;
uint8_t currentLowBeatPatternNumber = 0;
uint8_t currentSpectrumPatternNumber = 0;
uint8_t currentPatternNumber = 0;
uint8_t currentAmbPatternNumber = 0;
uint8_t musicWithConstBeatPatternNumber = 0;
uint8_t allPatternsNumber = 0;

// These are lists of functions that will be displayed during different types of music / no music.

// Ambient patterns for when no music is playing
typedef void(*ambientPatternList[])();
ambientPatternList ambientPatterns = {
	placeHolder
};

// Patterns for when there is no beat detected in the song
typedef void(*musicWithNoBeatPatternsList[])();
musicWithNoBeatPatternsList musicWithNoBeatPatterns = {
	fullSparkles,
	fallingMusic
};

// Patterns for when a beat is detected in the low frequency
typedef void(*lowBeatPatternList[])();
lowBeatPatternList lowBeatPatterns = {
	drawCircles
};

// Patterns for when a constant beat is detected
typedef void(*constBeatPatternList[])();
constBeatPatternList constBeatPatterns = {
	drawCircles
};

// A list of all the patterns
typedef void(*allPatternsList[])();
allPatternsList allPatterns = {
	drawCircles,
	fullSparkles,
	fallingMusic
};

// Cycle to the next pattern in 'AllPatterns'
void nextAllPatterns() {
	allPatternsNumber++;
	if (allPatternsNumber >= ARRAY_SIZE(allPatterns))
		allPatternsNumber = 0;
}

// These functions choose a random pattern within the lists above

void nextMusicWithConstBeatPattern() {
	// add one to the current pattern number, and wrap around at the end
	//musicWithNoBeatPatternNumber = (musicWithNoBeatPatternNumber + 1) % ARRAY_SIZE(musicWithNoBeatPatterns);
	musicWithConstBeatPatternNumber = random8(ARRAY_SIZE(constBeatPatterns));
}

void nextMusicWithNoBeatPattern() {
	// add one to the current pattern number, and wrap around at the end
	//musicWithNoBeatPatternNumber = (musicWithNoBeatPatternNumber + 1) % ARRAY_SIZE(musicWithNoBeatPatterns);
	musicWithNoBeatPatternNumber = random8(ARRAY_SIZE(musicWithNoBeatPatterns));
}

void nextAmbPattern() {
	//currentAmbPatternNumber = (currentAmbPatternNumber + 1) % ARRAY_SIZE(ambientPatterns);
	currentAmbPatternNumber = random8(ARRAY_SIZE(ambientPatterns));
}

void nextLowBeatPattern() {
	//currentLowBeatPatternNumber = (currentLowBeatPatternNumber + 1) % ARRAY_SIZE(lowBeatPatterns);
	currentLowBeatPatternNumber = random8(ARRAY_SIZE(lowBeatPatterns));
}