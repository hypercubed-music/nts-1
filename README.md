# nts1
Developing stuff for Korg nts-1

## osc-808
A simple 808-style bass oscilator
 - Shape: Pitch decay time
 - Alt (shift-shape): Phase distortion
 - Parameter 1: Drive
 - Parameter 2: Pitch attack time
 
## chords-osc
A 12-voice chord oscillator.
This oscillator automatically chooses major or minor chords depending on the key setting. It is able to make fifths, triads, sevenths, and suspended chords.
Pressing a note outside of a key plays a suspended chord.
 - Shape: Quantized chord key; 12 values from C to B
 - Alt (shift-shape): Number of notes in the chord, from 1 to 4. Note that the fifth is added before the third.
 - Parameter 1: Wave type; choose from saw, square, and sine
 - Parameter 2: Detune
 
## distort-mod
A simple distort/clip mod effect
 - Shape = type of clipping: softclip, hardclip, wrap, or fold.
 - Alt (shift-shape): Distortion depth
