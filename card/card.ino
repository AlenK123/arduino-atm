
#include "TFT9341.h"

/*
 * Magnetic Stripe Reader
 * by Stephan King http://www.kingsdesign.com
 *
 * Reads a magnetic stripe.
 *
 */

#define CLD 3               // Card status pin 
#define RDT 4               // Data pin
int reading = 2;            // Reading status
volatile int buffer[400];   // Buffer for data
volatile int i = 0;         // Buffer counter
volatile int bit = 0;       // global bit
char cardData[40];          // holds card info
int charCount = 0;          // counter for info
int DEBUG = 0;


void setup() {
    Serial.begin(9600); 
    pinMode(13,INPUT);
    /* The interrupts are key to reliable reading of the clock and data feed */
    attachInterrupt(0, []() -> void { bit = !bit; } , CHANGE); // flips the global bit
    attachInterrupt(1, []() -> void { buffer[i++] = bit; }, FALLING); // Writes the bit to the buffer
    lcd.begin();
    lcd.clrscr(BLUE);
    lcd.setFont(4);
}
 
void loop(){
 
    // Active when card present
    while(digitalRead(CLD) == LOW){
        reading = 1;
    }         
 
    // Active when read is complete
    // Reset the buffer
    if(reading == 1) {    
        decode();
        reading = 0;
        i = 0;
 
        for (int l = 0; l < 40; l = l + 1) {
            cardData[l] = '\n';
        }
 
        charCount = 0;
    }
}
 
// prints the buffer
void printBuffer(){
    for (int j = 0; j < 200; j = j + 1) {
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
 
        if (DEBUG == 1) {
            Serial.print(queue[0]);
            Serial.print(queue[1]);
            Serial.print(queue[2]);
            Serial.print(queue[3]);
            Serial.println(queue[4]);
        }
 
        if (queue[0] == 0 & queue[1] == 1 & queue[2] == 0 & queue[3] == 1 & queue[4] == 1) {
            sentinal = j - 4;
            break;
        }
    }
 
    if (DEBUG == 1) {
        Serial.print("sentinal:");
        Serial.println(sentinal);
        Serial.println("");
    }
 
    return sentinal;
}
 
void decode() {
    int sentinal = getStartSentinal();
    int i = 0;
    int thisByte[5];
 
    for (int j = sentinal; j < 400 - sentinal; j = j + 1) {
        thisByte[i] = buffer[j];
        i++;
        if (i % 5 == 0) {
            i = 0;
            if ( (thisByte[0]| thisByte[1] | thisByte[2] | thisByte[3] | thisByte[4]) == 0) {
                break;
            }
            printMyByte(thisByte);
        }
    }
 
    Serial.print("Stripe_Data:");
    for (int k = 0; k < charCount; k = k + 1) {
        Serial.print(cardData[k]);
    }
    Serial.println("");
    StrContains();
}
 
void printMyByte(int thisByte[]) {
   if (DEBUG == 1) {
        for (int i = 0; i < 5; i = i + 1) {
                Serial.print(thisByte[i]);
        }
        Serial.print("\t");
        Serial.print(decodeByte(thisByte));
        Serial.println("");
    }

    cardData[charCount++] = decodeByte(thisByte);
}
void StrContains()
{
    if (strstr(cardData, ";4995") != NULL) { 
        lcd.gotoxy(10,10);
        lcd.println((char*)"Working");
        delay(1000);
    }
    digitalWrite(13,LOW);
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
