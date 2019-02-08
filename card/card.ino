
#include "TFT9341.h"
#include <string.h>

#define CLD 3               // Card status pin 
#define RDT 4               // Data pin

volatile int buffer[400];   // Buffer for data
volatile int buffer_counter = 0; // Buffer counter
volatile int bit = 0;       // global bit
char cardData[40];          // holds card info


void setup() {
    Serial.begin(4800); 
    pinMode(13,INPUT);
    /* The interrupts are key to reliable reading of the clock and data feed */
    attachInterrupt(0, []() -> void { bit = !bit; } , CHANGE); // flips the global bit
    attachInterrupt(1, []() -> void { buffer[buffer_counter++] = bit; }, FALLING); // Writes the bit to the buffer
    lcd.begin();
    lcd.clrscr(BLUE);
    lcd.setFont(4);
}
 
void loop(){
    while(digitalRead(CLD) == LOW){
        decode();
        buffer_counter = 0;
    }         
}
 
// prints the buffer
void printBuffer(){
    Serial.print("Data:");
    for (int j = 0; j < 200; j++) {
        Serial.println(buffer[j]);
    }
    Serial.println("");
}
 
int getStartSentinal() {
    int j;
    int queue[5];
    int sentinal = 0;
 
    for (j = 0; j < 400; j++) {
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
    int thisByte[5];
    int charCount = 0;
    
    for (int j = sentinal, i = 0; j < 400 - sentinal; j++) {
        thisByte[i] = buffer[j];
        i++;
        if (i % 5 == 0) {
            i = 0;
            if ( (thisByte[0]| thisByte[1] | thisByte[2] | thisByte[3] | thisByte[4]) == 0) {
                break;
            }
            cardData[charCount++] = decodeByte(thisByte);
        }
    }
    printBuffer(cardData)
    StrContains();
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

void StrContains()
{
    if (strstr(cardData, ";382") != NULL) { 
        lcd.gotoxy(10,10);
        lcd.println((char*)"Working");
        delay(1000);
    }
    digitalWrite(13,LOW);
}
