#pragma once

#define CLD 3               // Card status pin 
#define RDT 4 

typedef struct mag_card {
    int charCount;
    bool global_bit;
    int buffer_counter;
    char buffer[400];
    char data[40];
} mag_card;

#define CLD 3               // Card status pin
#define RDT 4               // Data pin
int reading = 2;            // Reading status
volatile int buffer[400];   // Buffer for data
volatile int i = 0;         // Buffer counter
volatile int bit = 0;       // global bit
char cardData[40];          // holds card info
int charCount = 0;          // counter for info
int DEBUG = 0;
 


void kalil() {
  while (1) {
      while(digitalRead(CLD) == LOW){
        decode();
        reading = 0;
        i = 0;
        int l;
        for (l = 0; l < 40; l = l + 1) {
          cardData[l] = '\n';
        }
        charCount = 0;
     }
  }
}
 
// prints the buffer
void printBuffer(){
  int j;
  for (j = 0; j < 200; j = j + 1) {
    Serial.println(buffer[j]);
  }
}
 
int getStartSentinal(){
  int j;
  int queue[5];
  int sentinal = 0;
 
  for (j = 0; j < 400; j = j + 1) {
    queue[4] = queue[3];
    queue[3] = queue[2];
    queue[2] = queue[1];
    queue[1] = queue[0];
    queue[0] = buffer[j];
 
    if (queue[0] == 0 & queue[1] == 1 & queue[2] == 0 & queue[3] == 1 & queue[4] == 1) {
      sentinal = j - 4;
      break;
    }
  }
  return sentinal;
}
 
void decode() {
  int sentinal = getStartSentinal();
  int j;
  int i = 0;
  int k = 0;
  int thisByte[5];
 
  for (j = sentinal; j < 400 - sentinal; j = j + 1) {
    thisByte[i] = buffer[j];
    i++;
    if (i % 5 == 0) {
      i = 0;
      if ((thisByte[0] | thisByte[1] | thisByte[2] | thisByte[3] | thisByte[4]) == 0) {
        break;
      }
      cardData[charCount++] = decodeByte(thisByte);
    }
  }
 
  Serial.print("Stripe_Data:");
  for (k = 0; k < charCount; k = k + 1) {
    Serial.print(cardData[k]);
  }
  Serial.println("");
}

char decodeByte(int thisByte[]) {
      char dict[16] = {'0', '1', '2', '3', '4', '5', '6', '7',
                      '8', '9', ':', ';', '<', '=', '>', '?' };
      byte data = 0;

      for (byte i = 0; i < 4; i++) {
          if (thisByte[i]) data += (1 << i); 
      } 

      return dict[data];
} 
