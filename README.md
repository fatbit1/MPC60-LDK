# MPC60-LDK
AKAI MPC60 KEYBOARD LOOP and DELAY
  This project uses this Arduino MIDILibrary: https://github.com/FortySevenEffects/arduino_midi_library
  
  The following features are supported by KLD.
  1) CHROMATIC KEYS: Mapping of an MPC60 sound to an external chromatic keyboard. Chords and polyphony are supported.
  2) LOOP/REPETITION: Play the selected sound as a rudimentary variable lenght loop.
  3) DECAY: Decrease the sound velocity at each repetition.
  
  Shopping list:
    1) Arduino UNO
    2) MIDI shield with INPUT and OUTPUT
    3) 2 Midi cable
    4) USB Cable
    5) 5V USB Charger or 9V PSU for Arduino
    6) External MIDI Keyboard (Tested on a Emu EMAX I. All 61 keys MIDI keyboards with pitchbend and right wheel should work with no changes)
    7) AKAI MPC60/MPC60II. Should work with MPC3000 or other MPCs, too, but most likely, there will be no use case for these other models.
    
   Sketch loading and preliminary steps:
    1) Attach the MIDI shield to Arduino UNO. Check if shield and Arduino pins match as some MIDI shields do not have a standard form factor.
    2) Switch the ON/OFF RX(S") switch to OFF ( otherwise Sketchloading will fail with an error)
    3) Plug the USB cable to a computer running the Arduino IDE
    4) Select the right Serial port for programming (Tools/port/... from Arduino IDE)
    5) Open the present file and run Sketch/Upload
    6) Detach the USB cable when completed
    7) Switch the ON/OFF RX(S2) switch to ON 
    
   Connections:
    1) Connect one of the MPC60 MIDI inputs to the MPC60 translator OUT
    2) Connect the external keyboard MIDI Out to MPC60 translator 
    3) Connect Arduino UNO to USB charger/ 9V PSU

    User guide
    1) Map an MPC60 sound on an external chromatic keyboard (on MIDI Keys 72 to 90 C to Gb in the next octave)
     a) On MPC60: go to "Sound + 3" - Assign to note variation slider - pitch.
     b) Press a keys on the left side of the external keyboard (MIDI notes 36 to 71)
     Play the mapped sound on MIDI keys 72 to 90
     Repeat both steps a) and b) to select another sound.
    2) Loop/Repetition
       While keeping a key pressed, Move the right wheel to change the loop length. 
       If loop length is longer than sound duration, this will result in retriggering the same sound
    3) Decay
       While keeping a key pressed and the right wheel on a non-zero position, move the left wheel.
       Positions close to mid/0 produce sharp decays, positions closer to lowest point produce longer decays. 
*/
