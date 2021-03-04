# New_Visualizer_Skeleton
This repository has everything required to start a new music-visualizer project, and is designed for our hardware, whch is available for purchase on our Tindie store, at https://www.tindie.com/stores/diod_design/. You don't really need our hardware though. Visit pjrc.com, learn how to stack the audio adapter on top of the Teensy, learn how to input audio through either aux or mic, and then look up their OCTOWS2811 library to see which pins to connect to LED strips and how.

This project is set up for a 16x16 LED matrix

Features of this project:

AUDIO ANALYTICS
 - The DDAudio.h file and the musicAnalytics() function (called in loop() ) handles all of the FFT, beat detection and timing, and a couple other things. With this, you can create animations where the beat triggers certain things (fireworks, sweeps, etc), and animations that just react to the FFT data stored in spectrumValue[] (spectrumAnalyzer, sparkles, etc).
 LED ANIMATIONS
 - Animations can be sorted in to lists in DDPatternLists.h depending on whether they are ambient/non-reactive, beat reactive, general FFT visualizing, or animations you want to display when there's a constant low-frequency beat present in the music.
 
 SMOOTH ANIMATION BLENDING
 - Beat reactive patterns populate leds3[], general FFT animations populate leds2[], and the blend functions blend2() and blend3() in DDDrawing.h blend them in to leds[] according to how present a low-frequency beat is in the music. When there's a beat present and a beat-reactive animation is displaying, a new general FFT animation will be choosen to populate leds2[], so that when it fades back, it'll be a new pattern.
 
AUX or MIC input
 - You can hard-code your input at the top of DDAudio.h if you won't be switching between them. Or you can use the buttons on the PCB to choose your input when the Teensy is first powered on.
 
WiFi CONTROL PANEL HOSTED ON THE ESP32
 - The "NEW_VISUALIZER_ESP32" folder has the .ino file to be uploaded to the ESP32, and the 'data' folder to be uploaded through SPIFFS. This control panel has everything in place for on/off, brightness, audio gain, pattern selection, and any custom color can be displayed.
 - The ESP32's TX2/RX2 pins are connected to the Teensy's RX1/TX1 pins, so they can perform serial communication.
 - After every input on the WiFi page, the ESP32 will send a message in the format <x###> (b200, m2, a4, o000 are examples)
 - The DDESPcomm.h file contains talkToESP() (called in loop() ) and will handle everything the Teensy does when it receives these messages.

ESP-32 code for hosting a webpage control panel has been added! Hopefully the comments in the .ino file can explain how everything works well enough. The 'data' folder gets uploaded to ESP32 through SPIFFS. The website randomnerdtutorials.com has great ESP32 webserver tutorials, which this is basd on.

More patterns and LED functions coming soon!

More info on how to use this code coming soon!

Have any questions, join us in the Discord! https://discord.gg/AECN24q
