//This is a comment :)
int buttonPin[4] = { 33, 34, 35, 37 };
int ledPin[4] = { 2, 5, 8, 10 };
int indicatorLed[3] = { 22, 21, 20 };

int upButton = 31;
boolean upButtonState = LOW;
boolean upLastButtonState = LOW;
int downButton = 32;
boolean downButtonState = LOW;
boolean downLastButtonState = LOW;
int currentSequence = 0;
int totalSteps = 3;

int tempo = 0;
int currentStep = 0;
unsigned long lastStepTime = 0;
boolean lastButtonState[4] = { LOW, LOW, LOW, LOW };
boolean buttonState[4] = { LOW, LOW, LOW, LOW };
boolean stepState[3][4] = {
  { HIGH, LOW, LOW, LOW },
  { LOW, HIGH, LOW, LOW },
  { LOW, LOW, HIGH, HIGH }
};

int note[4] = { 50, 54, 58, 62 };
int i = 0;

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < 4; i++) {
    pinMode(buttonPin[i], INPUT);
    pinMode(ledPin[i], OUTPUT);
  }
  pinMode(upButton, INPUT);
  pinMode(downButton, INPUT);
  for (int i = 0; i < 3; i++) {
    pinMode(indicatorLed[i], OUTPUT);
  }
}

void loop() {

  channelSelect();
  checkButtons();
  setLeds();
  sequence();
}

void channelSelect() {
  Serial.println(currentSequence);

  upLastButtonState = upButtonState;
  upButtonState = digitalRead(upButton); //checks up button

  downLastButtonState = downButtonState;
  downButtonState = digitalRead(downButton); //checks down button


  if (upButtonState == HIGH && upLastButtonState == LOW) {
    currentSequence = currentSequence + 1;
    if (currentSequence == totalSteps) { //if its on the last step
      currentSequence = 0;                        //go back to the first step
    }
  }

  if (downButtonState == HIGH && downLastButtonState == LOW) {
    currentSequence = currentSequence - 1;
    if (currentSequence < 0) {                  //if its on the first step
      currentSequence = (totalSteps - 1);                        //go back to the total step
    }
  }

  for (int  k = 0; k < 3; k++) {    // switches LED based on step of pushbuttons
    if (k == currentSequence) {
      digitalWrite(indicatorLed[k], HIGH);
    } else {
      digitalWrite(indicatorLed[k], LOW);
    }
  }
}

void sequence() {

  tempo = analogRead(A9); //potentiometer sets tempo of sequencer

  if (millis() > lastStepTime + tempo) {   //if its time to go to the next step...
    currentStep = currentStep + 1;         //increment to the next step
    if (currentStep > 3) {
      currentStep = 0;
    }
    sendMIDI(); // send midi note

    lastStepTime = millis();               //set lastStepTime to the current time
  }
}

void sendMIDI() {
  for (int i = 0; i < 4; i++) {
    if ((i == currentStep) && (stepState[0][i] == HIGH)) {
      usbMIDI.sendNoteOff(60, 0, 1);
      usbMIDI.sendNoteOn(60, 127, 1);
    }
    else if ((i == currentStep) && (stepState[1][i] == HIGH)) {
      usbMIDI.sendNoteOff(64, 0, 1);
      usbMIDI.sendNoteOn(64, 127, 1);
    }
    else if ((i == currentStep) && (stepState[2][i] == HIGH)) {
      usbMIDI.sendNoteOff(67, 0, 1);
      usbMIDI.sendNoteOn(67, 127, 1);
    }
  }
}


void checkButtons() {

  for (int i = 0; i < 4; i++) {
    lastButtonState[i] = buttonState[i];
    buttonState[i] = digitalRead(buttonPin[i]);

    if (buttonState[i] == HIGH && lastButtonState[i] == LOW) {
      Serial.println(i);
      if (stepState[currentSequence][i] == LOW) { //currentSequence of channel switcher corresponds to array selected
        stepState[currentSequence][i] = HIGH;
      } else if (stepState[currentSequence][i] == HIGH) {
        stepState[currentSequence][i] = LOW;
      }
      Serial.println(stepState[currentSequence][i]);
    }
  }
}

void setLeds() {
  for (int i = 0; i < 4; i++) {
    if (i == currentStep) {
      analogWrite(ledPin[i], 150);  // High brightness
    } else if (stepState[currentSequence][i] == HIGH) { //currentSequence of channel switcher corresponds to array selected
      analogWrite(ledPin[i], 20);  // medium brightness
    } else {
      analogWrite(ledPin[i], 0);  // Off
    }
  }
}



