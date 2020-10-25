/*

  Melody

  Plays a melody

  circuit:

  - 8 ohm speaker on digital pin 8

  created 21 Jan 2010

  modified 30 Aug 2011

  by Tom Igoe

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Tone

*/

#include "pitches.h"

// notes in the melody:
int melody[] = {

  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {

  4, 8, 8, 4, 4, 4, 4, 4
};

int music[8][2] = {
  {NOTE_C4, 4}//, {NOTE_G3, 8}, {NOTE_G3, 8}, {NOTE_A3, 4}, {NOTE_G3, 4}, {0, 4}, {NOTE_B3, 4}, {NOTE_C4, 4}
};

int winmusic1[8][2] = {
  {NOTE_DS6, 4}, {NOTE_DS5, 6}, {NOTE_AS5, 2}, {NOTE_FS5, 3}, {NOTE_DS6, 4}, {NOTE_AS5, 3}, {NOTE_DS5, 3}
};

int winmusic2[8][2] = {
  {NOTE_GS6, 5}, {NOTE_DS6, 6}, {NOTE_GS5, 5}, {NOTE_FS5, 2}, {NOTE_DS5, 3}, {NOTE_AS5, 3}, {NOTE_FS5, 2}
};

void setup() {
 
}

void loop() {
  //play1();
  logonMusic();
  delay(5000);
  logoffMusic();
  delay(5000);
}

/*
void play() {
  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 8; thisNote++) {
    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(6, melody[thisNote], noteDuration);
    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(6);
  }
}*/

void play1() {
  for(int thisNote = 0; thisNote < (sizeof(music)/sizeof(music[thisNote])); thisNote++) {
    int noteduration = 1000 / music[thisNote][1];
    tone(6, music[thisNote][0], noteduration);
    int pauseBetweenNotes = noteduration * 1.30;
    delay(pauseBetweenNotes);
    noTone(6);
  }
}

void logonMusic() {
  for(int thisNote = 0; thisNote < 7; thisNote++) {
    int noteduration = 1000 / winmusic1[thisNote][1];
    tone(6, winmusic1[thisNote][0], noteduration);
    int pauseBetweenNotes = noteduration * 1.30;
    delay(pauseBetweenNotes);
    noTone(6);
  }
}

void logoffMusic() {
  for(int thisNote = 0; thisNote < 7; thisNote++) {
    int noteduration = 1000 / winmusic2[thisNote][1];
    tone(6, winmusic2[thisNote][0], noteduration);
    int pauseBetweenNotes = noteduration * 1.30;
    delay(pauseBetweenNotes);
    noTone(6);
  }
}
