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

void logoffMusic() {
  for(int thisNote = 0; thisNote < 7; thisNote++) {
    int noteduration = 1000 / winmusic2[thisNote][1];
    tone(6, winmusic2[thisNote][0], noteduration);
    int pauseBetweenNotes = noteduration * 1.30;
    delay(pauseBetweenNotes);
    noTone(6);
  }
}
