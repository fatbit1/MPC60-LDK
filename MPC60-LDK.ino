/**MPC60 KLD - MPC60 KEYBOARD LOOP and DELAY
  This project uses this Arduino MIDILibrary: https://github.com/FortySevenEffects/arduino_midi_library
  The following features are supported by KLD.
  1) CHROMATIC KEYS: Mapping of an MPC60 sound to an external chromatic keyboard. Chords and polyphony are supported.
  2) LOOP/REPETITION: Play the selected sound as a variable lenght loop.
  3) DECAY: Decrease the sound velocity at each repetition.
  
  Shopping list:
    1) Arduino UNO
    2) MIDI shield with INPUT and OUTPUT
    3) 2 Midi cable
    4) USB Cable
    5) 5V USB Charger or 9V PSU for Arduino
    6) External MIDI Keyboard (Tested on a Emu EMAX I. All 61 keys MIDI keyboards with pitchbend and right wheel should work with no changes)
    7) MPC60/MPC60II. Should work the same with MPC3000 or other MPCs but most likely, there is no use case for these other models.
   Sketch loading a preliminary steps:
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
    1) Mapping of an MPC60 sound on an external chromatic keyboard (MIDI Keys 72 to 90 C to Gb in the next octave)
     a) On MPC60: go to "Sound + 3" - Assign to note variation slider - pitch menu
     b) Press a keys on the left side of the external keyboard (MIDI notes 36 to 71)
     Play the mapped sound on MIDI keys 72 to 90
     Repeat both steps a) and b) to select another sound.
    2) Loop/Repetition
       While keeping a key pressed, Move the right wheel to change the loop lenght. 
       If loop lenght is longer than sound duration, this will result in retriggering the same sound
    3) Decay
       While keeping a key pressed and the right wheel on a non-zero position, move the left wheel.
       Positions close to mid/0 produce sharp decays, positions closer to lowest point produce longer decays. 
*/


#include <MIDI.h>
//CC received from Rightwheel 
//( left wheel is pitchbend. It uses a comnination of 128*128 values and has a separate handler)
#define RIGHTWHEEL 1
//First MIDI note on the keyboard to be used as Chromatic keyboard for MPC60
#define BASEKEY 72
//First MIDI note on the MPC60 (36 -SNARE1 -PAD2)
#define MPCBASENOTE 36
//Definition of a base unit in milliseconds for loop repetition
#define MINDELAY 20
// CC assignd to note variation slider in MPC60 (default = 20)
#define NOTEVARCC 20
//MIDI channel used to send messages to MPC60
#define CHANNEL_MIDI_DEVICE 1


// note parameters: mpc notenumber, velocity, decay.
// MPC60 note variation sliders admits values from -120 to 60. 
// Not all values generate a pitch change.
unsigned int gnote[19];
unsigned int gvelocity[19];
unsigned int gdecay[19];
unsigned int decay=0;
//Initial value for the MPC note that has to be mapped on the chromatic keyboard. 
//Different notes can be selected by pressing keys on the left side of the keyboard,
//while on MPC menu SOUND +3 (Assing Note Var Slider)
unsigned int targetnote = MPCBASENOTE;

//Clock handling:
long currenttime;
long gdelaytime [19] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
long lasttime [19] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
long delaytime = 0;



// Create a 'MIDI' object using MySettings bound to Serial1. Set MIDI baud rate on Serial interface.
struct MidiBaudRate : public midi::DefaultSettings
{
    static const long BaudRate = 31250;
};
MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial, MIDI, MidiBaudRate);


// Initialize MIDI 
void setup() {
  // MIDI listening to device channel
  MIDI.begin(CHANNEL_MIDI_DEVICE);
  // we don't won't to echo all incoming data without filtering or translating!
  MIDI.setThruFilterMode(midi::Thru::Off);
  
//Accept, Note ON/OFF, Control Change and Pitch Bend MIDI messages.
  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.setHandleControlChange(handleControlChange);
  MIDI.setHandlePitchBend(handlePitchBend);
}


// Read MIDI inputs and trigger note repetitions, if loop is non-zero

void loop() { 
  MIDI.read();
  for (int i = 0; i <=18; i++) 
  {
  currenttime=millis();
  if (((currenttime - lasttime[i]) >= gdelaytime[i]) and (gvelocity[i]>0) and gdelaytime[i]>0)
    { 
    sendnote(BASEKEY+i,gvelocity[i]);
   } 
  }
}


//React to note ON messages
void handleNoteOn(byte channel, byte note, byte velocity) 
 { 
   if((note>=BASEKEY) and (note<=BASEKEY+18))  
    {
     int i=note-BASEKEY;
     gnote[i]=note;
     gdelaytime[i]=delaytime;
     gvelocity[i]=velocity;
     gdecay[i]=decay;     
    }
   sendnote(note,velocity);
}

//React to note OFF messages
void handleNoteOff(byte channel, byte note, byte velocity) 
 { 
   if((note>=BASEKEY) and (note<=BASEKEY+18)) 
   gvelocity[note - BASEKEY]=0;
  }

//React to MIDI Control change from Right Wheel and transform them in delay time.
void handleControlChange(byte channel, byte number, byte value) 
  {
   if(number==RIGHTWHEEL) 
              delaytime = value * MINDELAY;
  }

//React to PitchBend messages (lower side only)
void   handlePitchBend (byte channel, int bend)
{     
        if (bend>=0) 
             {
             decay=0;
             }
        else  
             {
             decay = 64 + (bend/128); 
             }           
}



//Send MIDI messages to MPC60
void sendnote(unsigned int playnote, unsigned int playvelocity) 
   { 
     //If note falls in the MPC60 mappd range
   if((playnote >=BASEKEY) and (playnote <=(BASEKEY+18)))
    {
     int i = playnote - BASEKEY;
     // set timers for delay calculation
     lasttime[i]=millis();
     //   Send a MIDI CC value as MPC60 note variation slider, corresponding to the semitone of they key pressed in the external keyboard
     //   Note variation slider has a default range of -120 and 60 and accepts 0-127 MIDI values. Not all values correspond to a different semitone. 
     //   Semitone changes happen in steps of 7. Send a MIDI note on message immediately after the semitone is set.
     MIDI.sendControlChange(NOTEVARCC, (7*i)+1, CHANNEL_MIDI_DEVICE);
     MIDI.sendNoteOn(targetnote, playvelocity, CHANNEL_MIDI_DEVICE); 

    //Subtract decay values to note velocity for the next note play
     if( gdecay[i] > 0)
      if(gvelocity[i] <= gdecay[i]) gvelocity[i]=0;
      else gvelocity[i] = gvelocity[i] - gdecay[i];
    }

   //If a note is outside of the MPC60 chromatic keyboard range, just send it to MPC and use it as the next mapped note 
   // Sound + 3 - Assigned note variation slider on MPC60 is required, otherwise the same semitone will be played across the mapped keys on the right.
   if(playnote >=MPCBASENOTE and (playnote <(BASEKEY)) )
    {
     MIDI.sendNoteOn(playnote, playvelocity, CHANNEL_MIDI_DEVICE);
     targetnote=playnote;
    }
}
