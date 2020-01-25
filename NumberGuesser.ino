/* @file Number Chooser
|| @version 1.0
|| @author Athavan Theivakulasingham
||
|| @description
|| | You need to guess the randomized number between 0 and 9.
|| | The user has 3 tries.
|| #
*/

#include <Keypad.h>

// notes for tone
#define NOTE_C4  262
#define NOTE_G3  196
#define NOTE_A3  220
#define NOTE_B3  247

const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
{'1','2','3','A'},
{'4','5','6','B'},
{'7','8','9','C'},
{'*','0','#','D'}
};
byte rowPins[ROWS] = {2,3,4,5}; //connect to the row pinouts of the kpd
byte colPins[COLS] = {6,7,8,9}; //connect to the column pinouts of the kpd

Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

// notes in the melody:
int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

// pins
int retryButtonPin = 11;
int threeHeartsPin = 19;
int twoHeartsPin = 18;
int oneHeartPin = 17;
int lowerPin = 12;
int higherPin = 13;
int tonePin = 10;

int buttonState; 
int hearts = 3;
char currentRandomNumber; // saved as char to simplify comparison

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
int lastButtonState = LOW;   // the previous reading from the input pin

void setup() {
    pinMode(retryButtonPin, INPUT);
    startNewGame();
}


void loop() {
  // define hearts LED
  if(hearts >= 3) {
    digitalWrite(threeHeartsPin, HIGH);
  } else {
    digitalWrite(threeHeartsPin, LOW);
  }
  
  if(hearts >= 2) {
    digitalWrite(twoHeartsPin, HIGH);
  }else {
    digitalWrite(twoHeartsPin, LOW);
  }
  
  if(hearts >= 1) {
    digitalWrite(oneHeartPin, HIGH);
  }else {
    digitalWrite(oneHeartPin, LOW);
  }

  char pressedKey = kpd.getKey();

  // check if a key from KeyPad was pressed and if there a enough hearts
  if (pressedKey && hearts > 0){  
    // compare pressed key with randomized value
    if(pressedKey == currentRandomNumber) {
      runSuccessMusic();
      hearts = 0;
    } else {
      // because of a wrong input, the user looses a heart
      hearts = hearts - 1;
      supportUserByLED(pressedKey - '0');
    }
  }

  // get value of retry button
  int reading = digitalRead(retryButtonPin);

  // if current button state is not equal to last button state
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      // when buttonState of retryButton is 1 (HIGH)
      if (buttonState == HIGH) {
        startNewGame();
        hearts = 3;
      } 
    }
  }
  lastButtonState = reading;
} 

void startNewGame() {
  // get random number between 0 - 9
  currentRandomNumber = (rand() % 10) + '0';
  digitalWrite(lowerPin, LOW);
  digitalWrite(higherPin, LOW);
}

/**
 * depending, if the user made an input which is 
 * lower or higher than the expected number, 
 * it shows a led to tell the user, that the 
 * randomized value is lower/higher
 */
void supportUserByLED(int value) {
  int castedValue = currentRandomNumber - '0';
   if(value > castedValue) {
      digitalWrite(lowerPin, HIGH);
      digitalWrite(higherPin, LOW);
   } else if(value  < castedValue) {
      digitalWrite(lowerPin, LOW);
      digitalWrite(higherPin, HIGH);
   }
}

void runSuccessMusic() {
  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 8; thisNote++) {

    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(tonePin, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(tonePin);
  }
}
