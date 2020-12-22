#include "Pitches.h"

int winmusic1[8][2] = {
  {NOTE_DS6, 4}, {NOTE_DS5, 6}, {NOTE_AS5, 2}, {NOTE_FS5, 3}, {NOTE_DS6, 4}, {NOTE_AS5, 3}, {NOTE_DS5, 3}
};

int winmusic2[8][2] = {
  {NOTE_GS6, 5}, {NOTE_DS6, 6}, {NOTE_GS5, 5}, {NOTE_FS5, 2}, {NOTE_DS5, 3}, {NOTE_AS5, 3}, {NOTE_FS5, 2}
};

void logonMusic() {
  for(int thisNote = 0; thisNote < 7; thisNote++) {
    int noteduration = 1000 / winmusic1[thisNote][1];
    tone(6, winmusic1[thisNote][0], noteduration);
    int pauseBetweenNotes = noteduration * 1.30;
    delay(pauseBetweenNotes);
    noTone(6);
  }
}

int errormusic[6][2] = {
  {NOTE_C7, 3}, {NOTE_FS5, 2}
};

int notifymusic[6][2] = {
  {NOTE_B5, 5}, {NOTE_DS7, 6}, {NOTE_G6, 5}, {NOTE_GS7, 6}
};

void startMusic() {
  for(int thisNote = 0; thisNote < 4; thisNote++) {
    int noteduration = 1000 / notifymusic[thisNote][1];
    tone(6, notifymusic[thisNote][0], noteduration);
    int pauseBetweenNotes = noteduration * 1.30;
    delay(pauseBetweenNotes);
    noTone(6);
  }
}

void stopMusic() {
  for(int thisNote = 3; thisNote >= 0; thisNote--) {
    int noteduration = 1000 / notifymusic[thisNote][1];
    tone(6, notifymusic[thisNote][0], noteduration);
    int pauseBetweenNotes = noteduration * 1.30;
    delay(pauseBetweenNotes);
    noTone(6);
  }
}
void errorMusic() {
  for(int thisNote = 0; thisNote < 2; thisNote++) {
    int noteduration = 1000 / errormusic[thisNote][1];
    tone(6, errormusic[thisNote][0], noteduration);
    int pauseBetweenNotes = noteduration * 1.30;
    delay(pauseBetweenNotes);
    noTone(6);
  }
}
