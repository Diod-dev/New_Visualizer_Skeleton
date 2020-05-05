// DDAudio.h
// This file contains all of the music-analytics (FFT, beat detection, beat timing, silence detection)

// PJRC's audio library
#include <Audio.h>

// Generated on PJRC's audio configuration tool
// GUItool: begin automatically generated code
AudioInputI2S i2s1; //xy=219.1999969482422,183
AudioMixer4 mixer1; //xy=403.1999969482422,203.1999969482422
AudioAnalyzeFFT1024 fft1024; //xy=547.1999969482422,239
AudioConnection patchCord1(i2s1, 0, mixer1, 0);
AudioConnection patchCord2(i2s1, 1, mixer1, 1);
AudioConnection patchCord3(mixer1, fft1024);
AudioControlSGTL5000 audioShield; //xy=446.1999969482422,317
// GUItool: end automatically generated code

// These 2 lines hardcore an input mode, LINEIN (aux) or MIC. Only 1 can be uncommented.
const int myInput = AUDIO_INPUT_LINEIN;
//const int myInput = AUDIO_INPUT_MIC;
//int myInput;

// AUDIO DETECTION VARIABLES
// Number of FFT bins we are populating
const int numFFTBins = 16;
// Raw FFT data
int spectrumValue[numFFTBins];
int spectrumValueOld[numFFTBins];
// Running averages in the FFT bins
double average[numFFTBins];
// Running standard deviations in the FFT bins
double stDev[numFFTBins];
// TODO: implement total volume data. 
int volume, volumeAverage;
Statistic vol;
// fadeValues are used to control how fast LEDs fade that are colored by the FFT data directly.
// The higher the FFT data (the louder the sound is), the higher the fadeValue (the faster the light fades).
int fadeVals[numFFTBins];
// freq is an array of values that we can apply the 'Statistic' library to
Statistic freq[numFFTBins];
// beatDetected monitors whether a beat has been detected. 2 = new beat detected. 1 = old beat hasn't gone down yet. 0 = no beat detected
uint8_t beatDetected[numFFTBins] = {};
// BeatTimer measures the amount of time between detected beats
elapsedMillis beatTimer[numFFTBins] = {};
// BeatIntervals are where beatTimer data is stored
int beatInterval[numFFTBins];
int beatIntervalOld[numFFTBins];
// constantBeatCounter keeps track of how many beats have occurred with a constant time between them
int constantBeatCounter[numFFTBins];
// TODO: utilize the peaks of the beats to have more accurate peak detection and utilize for beat-dependent patterns
int beatPeak[numFFTBins];
int beatPeakOld[numFFTBins];
// The higher the activity in the FFT bin, the higher the score
int binScore[numFFTBins];
// Which FFT bins in the low frequencies, mid frequencies, and high frequencies have the the strongest beats within the song.
int lowBeatBin, midBeatBin, highBeatBin;
// Which FFT bins in the low frequencies, mid frequencies, and high frequencies have the highest averages
int lowAveBin, midAveBin, highAveBin;
// How long has silence been detected
elapsedMillis silenceDuration;
// How long has song been going on for. Useful for DJ sets that don't have silence between songs. This keeps the song data clearing every 5 mins
elapsedMillis songDuration;
bool silence = false;
// Constant beat detected variables
bool constBeat = false;
int maxConstBeat = 0;
uint8_t constBeatBin;
// mixAmount bounces between 0 and 255 depending on whether a strong beat is detected. 255 = beat detcted, 0 = no beat detected
int mixAmount;

void audioSetup() {
	// TODO: add way for input to be chosen either by button or over WiFi somehow.
	// Input can only be changed when Teensy is turned on / rebooted.
  delay(2000);

  // Must be holding both buttons to make it AUX input. This can be changed depending on what you want your default input to be.
  //if (button1.pressed()) {
    //delay(1000);
    //if (button2.pressed()) {
    //  myInput == AUDIO_INPUT_LINEIN;
      //Serial.println("Input is Aux");
    //}
 // }
  //else {
    //  myInput = AUDIO_INPUT_MIC;
      //Serial.println("Input is Mic");
  //}

	// AUDIO STUFF
	AudioMemory(12);
	audioShield.enable();
	audioShield.inputSelect(myInput);

	// configure the mixer to equally add left & right
	mixer1.gain(0, 0.5);
	mixer1.gain(1, 0.5);
}

// Clears the running mean and st dev. Used mostly between songs when silence is detected.
void clearStats() {
	for (int i = 0; i < numFFTBins; i++) {
		freq[i].clear();
		beatPeak[i] = 0;
		binScore[i] = 0;
		lowBeatBin = 2;
		midBeatBin = 8;
		highBeatBin = 12;
	}
}

void detectSilence() {
	uint8_t silenceCounter = 0;
	// If FFT bin data is very low, consider it as silence in the bin
	// reset the statistics because it's likely the song ended
	for (int i = 0; i < numFFTBins; i++) {
		if (spectrumValue[i] < 10) { // 10 is an arbitrary number but it means it is basically silent. Some audio sources have noise in higher FFT bins.
			silenceCounter++;
		}
		else {
			break; // stop counting silent FFT bins
		}
	}
	// If the song is 5 minutes long, reset stats.
	if (songDuration > 300000) {
		songDuration = 0;
		clearStats();
	}

	// If we've counted over 11 bins that were silent, then clear stats, set silence = true, and set songDuration = 0.
	if (silenceCounter > 11) {
		clearStats();
		silence = true;
		songDuration = 0;
	}
	else { // else (if silence isn't detected), set silence = false, silenceDuration = 0
		silence = false;
		silenceDuration = 0;
	}
}

void fillStats() {
	// Save old FFT data in spectrumValueOld
	for (int i = 0; i < numFFTBins; i++)
		spectrumValueOld[i] = spectrumValue[i];

	//Serial.println(mult);
	spectrumValue[0] = mult * fft1024.read(0) * 1000; // multiply by 1000 to make them integers
	spectrumValue[1] = mult * fft1024.read(1) * 1000;
	spectrumValue[2] = mult * fft1024.read(2, 3) * 1000;
	spectrumValue[3] = mult * fft1024.read(4, 6) * 1000;
	spectrumValue[4] = mult * fft1024.read(7, 10) * 1000;
	spectrumValue[5] = mult * fft1024.read(11, 15) * 1000;
	spectrumValue[6] = mult * fft1024.read(16, 22) * 1000;
	spectrumValue[7] = mult * fft1024.read(23, 32) * 1000;
	spectrumValue[8] = mult * fft1024.read(33, 46) * 1000;
	spectrumValue[9] = mult * fft1024.read(47, 66) * 1000;
	spectrumValue[10] = mult * fft1024.read(67, 93) * 1000;
	spectrumValue[11] = mult * fft1024.read(94, 131) * 1000;
	spectrumValue[12] = mult * fft1024.read(132, 184) * 1000;
	spectrumValue[13] = mult * fft1024.read(185, 257) * 1000;
	spectrumValue[14] = mult * fft1024.read(258, 359) * 1000;
	spectrumValue[15] = mult * fft1024.read(360, 511) * 1000;

	// reset volume variable
	volume = 0;
	// Update the average and standard deviation of each FFT bin value
	for (int i = 0; i < numFFTBins; i++) {
		freq[i].add(spectrumValue[i]);
		average[i] = freq[i].average();
		stDev[i] = freq[i].pop_stdev();
		volume = volume + spectrumValue[i]; // add all the values to find the volume
	}

	// See if there is silence
	detectSilence();
}

// Measure the time between beats
void beatTiming(int i) {
	if (beatDetected[i] == 2) { // if a new beat has been detected
		if (beatTimer[i] < 200) // but it is less than 200ms from the last one
			beatDetected[i] = 1; // cancel the beat
		else { // if it's more than 200ms from the last one
			beatInterval[i] = beatTimer[i];  // A real beat has been detected. Save the time since last beat.
			// TODO: could use absolute value below (Wasn't working for unknown reason)
			if ((beatIntervalOld[i] - beatInterval[i]) < 30 && (beatIntervalOld[i] - beatInterval[i]) > -30) // If the time between the new interval and old interval is less than 30 ms 
				constantBeatCounter[i]++;

			beatTimer[i] = 0; // reset beat timer
			beatIntervalOld[i] = beatInterval[i]; // save new beat interval as old beat interval
		}
	}
	if (constantBeatCounter[i] > 0 && beatTimer[i] > beatInterval[i] + 50) constantBeatCounter[i] = 0; // clears beat counter when more time than the beat has passed

	// These print statements will print the constant beat counter of each bin
	//if (i == numFFTBins - 1) Serial.println(constantBeatCounter[i]);
	//else {
	//	Serial.print(constantBeatCounter[i]);
	//	Serial.print("\t");
	//}

	// NOTE: remember this beatTiming function is run within a for loop in musicAnalytics()
	if ( i < 4) { // for the 4 lowest frequency FFT bins
		if (constantBeatCounter[i] > maxConstBeat) { // if this FFT bin's constantBeatCounter is greater than the current highest constant beat detected (within 4 lowest bins)...
			maxConstBeat = constantBeatCounter[i]; // the new maxConstBeat is this bin's constantBeatCounter
			constBeatBin = i; // This bin currently has the maxConstBeat
		}
	}
	if (maxConstBeat > 10) // If we've reached over 10 beats with a constant time interval...
		constBeat = true; // Go in to a constBeat pattern ( used in normalOperations() )
	else 
		constBeat = false;
}

void beatDetection(int i) {
	// 0 = no beat detected
	// 1 = old beat hasn't dropped / reset yet
	// 2 = new beat detected
	// Peak detection is performed here. FFT value must be greater than the average+(2.3*st.dev) and greater than a threshold of 100, which is pretty quiet
	if (spectrumValue[i] > average[i] + 2.3 * stDev[i] && spectrumValue[i] > 100) {
		if (beatDetected[i] == 2) // If it's already 2, then the beat has already been detected...
			beatDetected[i] = 1; // so drop the value to 1.
		if (beatDetected[i] == 0) // If 0, then this is a new beat...
			beatDetected[i] = 2; // so make the value 2.
		// if it's == 1, it stays 1. no code is needed.
	}
	// This is where 1's get reset to 0. This prevents multiple beats being triggered from 1 beat, or when they aren't well defined
	else { // if beat is not detected...
		if (beatDetected[i] == 1) { // and it's value is 1...
			if (spectrumValue[i] <= average[i]) // and it has dropped below the running average of that FFT bin...
				beatDetected[i] = 0; // reset to 0
		}
	}
}
 
// scoreBins figures out which FFT bins have the best beats to visualize.
void scoreBins(int i) {
	// If there's a constant beat above 5 counts, increase score by 2
	if (constantBeatCounter[i] > 5)
		binScore[i] += 2;

	// If there's a beat detected, increase score by 1
	if (beatDetected[i] == 2)
		binScore[i] += 1;

	// If an FFT bin's score is high and it doesn't have a constant beat, lower that score
	if (binScore[i] > 300 && constantBeatCounter[i] < 2)
		binScore[i] -= 2;

	// These print statements will print the score of each bin
	//if (i == numFFTBins - 1)
	//	Serial.println(binScores[i]);
	//else {
	//	Serial.print(binScores[i]);
	//	Serial.print("\t");
	//}
}

void musicAnalytics() {
	// Write FFT data to spectrumValue[ ] array
	fillStats();

	maxConstBeat = 0; // reset this before it is set within beatTiming().
	// All the deep analytics happen from the functions in this for loop
	for (int i = 0; i < numFFTBins; i++) {
		beatDetection(i);
		beatTiming(i);
		//printDetectedBeats(i); // diagnostic function, uncomment to see if beats are being detected.
		scoreBins(i);

		// Fill Fade Values
		if (spectrumValue[i] > average[i])
			fadeVals[i] += (spectrumValue[i] - average[i]) / 2;
		else
			//fadeVals[i] -= (average[i] - spectrumValue[i]) / 2;
			fadeVals[i] -= 5;

		fadeVals[i] = constrain(fadeVals[i], 0, 255);
	}

	// This for loop checks the lowest 4 bins, mid 4 bins, high 4 bins. (Some bins are not counted b/c they're in between. Might need fixing)
	for (int i = 0; i < 4; i++) {
		// Find the beats in the low, mid, and high ranges
		if (binScore[i] > binScore[lowBeatBin])
			lowBeatBin = i;
		if (binScore[i + 5] > binScore[midBeatBin]) // binScore[4] is not measured
			midBeatBin = i + 5;
		if (binScore[i + 9] > binScore[highBeatBin])
			highBeatBin = i + 9;

		// Find highest averages in the low, mid, and high ranges
		if (average[i] > average[lowAveBin])
			lowAveBin = i;
		if (average[i + 5] > average[midAveBin])
			midAveBin = i + 5;
		if (average[i + 9] > average[highAveBin])
			highAveBin = i + 9;
	}

	// This local variable is what moves the mixAmount variable
	static int mixAmountInfluencer;

	// If a beat is detected or there's currently a constant beat over 8 counts, increase mixAmount
	if (beatDetected[lowBeatBin] == 2 || constantBeatCounter[lowBeatBin] > 8)
		mixAmountInfluencer += 5;

	// Otherwise, it is constantly decreasing
	EVERY_N_MILLIS(150)
		mixAmountInfluencer -= 1;

	mixAmountInfluencer = constrain(mixAmountInfluencer, -10, 10);
	mixAmount = constrain(mixAmount + mixAmountInfluencer, 0, 255);
	//Serial.print(mixAmountInfluencer);
	//Serial.print("\t");
	//Serial.println(mixAmount);
}

void printBeatBins() {
	// Prints all the beat bins
	//Serial.print(lowBeatBin);
	//Serial.print("\t");
	//Serial.print(binScore[lowBeatBin]);
	//Serial.print("\t");
	//Serial.print(midBeatBin);
	//Serial.print("\t");
	//Serial.print(binScore[midBeatBin]);
	//Serial.print("\t");
	//Serial.print(highBeatBin);
	//Serial.print("\t");
	//Serial.println(binScore[highBeatBin]);
}

void printNumber(float n) {
  if (n >= 0.004) {
    Serial.print(n, 3);
    Serial.print(" ");
  }
  else {
    //  Serial.print("   -  "); // don't print "0.00"
    Serial.print("   0  "); // print 0 for excel purposes
  }
}

void printSpectrum() {
  ////// UNCOMMENT THEse LINES TO PRINT THE FFT WHILE PLAYING A SONG
	if (fft1024.available()) {
		// each time new FFT data is available
		// print to the Arduino Serial Monitor
		//Serial.print("FFT: ");
		printNumber(fft1024.read(0) * 1000);
		printNumber(fft1024.read(1) * 1000);
		printNumber(fft1024.read(2, 3) * 1000);
		printNumber(fft1024.read(4, 6) * 1000);
		printNumber(fft1024.read(7, 10) * 1000);
		printNumber(fft1024.read(11, 15) * 1000);
		printNumber(fft1024.read(16, 22) * 1000);
		printNumber(fft1024.read(23, 32) * 1000);
		printNumber(fft1024.read(33, 46) * 1000);
		printNumber(fft1024.read(47, 66) * 1000);
		printNumber(fft1024.read(67, 93) * 1000);
		printNumber(fft1024.read(94, 131) * 1000);
		printNumber(fft1024.read(132, 184) * 1000);
		printNumber(fft1024.read(185, 257) * 1000);
		printNumber(fft1024.read(258, 359) * 1000);
		printNumber(fft1024.read(360, 511) * 1000);
		Serial.println();
	}
}

// Used for diagnostics / watching whether a song is having beats detected
void printDetectedBeats(int i) {
	// These print statements will print the beat status of each bin
	if (i == numFFTBins - 1) Serial.println(beatDetected[i]);
	else {
		Serial.print(beatDetected[i]);
		Serial.print("\t");
	}
}
