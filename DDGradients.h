// DDGradients.h
// There are lots of common gradients that FastLED uses, but they aren't included here because they have "darker" spots in the palettes
// These palettes do not have any darker regions

// COLOR PALLETTE STUFF
uint8_t currentPaletteNumber;
TBlendType currentBlending;

DEFINE_GRADIENT_PALETTE(Blue_Cyan_Yellow_gp) {
		0, 0, 0, 255,
		63, 0, 55, 255,
		127, 0, 255, 255,
		191, 42, 255, 45,
		255, 255, 255, 0
};


DEFINE_GRADIENT_PALETTE(usa_p) {
		0, 255, 0, 0,   //red
		128, 255, 255, 255,   //white
		255, 0, 0, 255   // blue
};

DEFINE_GRADIENT_PALETTE(redBlueRed_p) {
		0, 255, 0, 0,		  //red
		128, 0, 0, 255,		  // blue
		255, 255, 0, 0,		  //red
};

DEFINE_GRADIENT_PALETTE(redGreenRed_p) {
		0, 255, 0, 0,		  //red
		128, 0, 255, 0,		  // blue
		255, 255, 0, 0,		  //red
};

DEFINE_GRADIENT_PALETTE(purpleGreenPurple_p) {
		0, 255, 0, 255,		  //red
		128, 0, 255, 0,		  // blue
		255, 255, 0, 255,		  //red
};

DEFINE_GRADIENT_PALETTE(yellowPurpleYellow_p) {
		0, 255, 255, 0,		  //yellow
		128, 255, 0, 255,		  // purple
		255, 255, 255, 0,		  // yellow
};

DEFINE_GRADIENT_PALETTE(orangeRed_p) {
		0, 255, 160, 0,		  // orange
		255, 255, 0, 0,		  // red
};

DEFINE_GRADIENT_PALETTE(blueGreen_p) {
	    0, 0, 0, 255,		  // blue
		255, 0, 255, 0,		  // green
};

DEFINE_GRADIENT_PALETTE(blueWhite_p) {
		0, 0, 0, 170,		  // blue
		255, 170, 170, 170,
};

DEFINE_GRADIENT_PALETTE(justWhite_p) {
		0, 255, 255, 255,		  // blue
		255, 255, 255, 255,
};

DEFINE_GRADIENT_PALETTE(normal_p) {
	0, 255, 0, 0,
	42, 128, 128, 0,
	85, 0, 255, 0,
	128, 0, 128, 128,
	171, 0, 0, 255,
	213, 128, 0, 128,
	255, 255, 0, 0
};


// This list of color palettes can be called upon in the main code
// The order only matters if you'd like to scroll through them sequentially
// Otherwise, they can be called by their number on this list: 0, 1, 2, etc..
const TProgmemRGBGradientPalettePtr gGradientPalettes[] = {
	usa_p,
	redBlueRed_p,
	purpleGreenPurple_p,
	orangeRed_p,
	yellowPurpleYellow_p,
	Blue_Cyan_Yellow_gp,
	normal_p,
	blueGreen_p
 };

// Count of how many gradients are defined:
const uint8_t gGradientPaletteCount = 
sizeof(gGradientPalettes) / sizeof(TProgmemRGBGradientPalettePtr);
