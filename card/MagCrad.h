#pragma once

#include <string.h>
#include "User.h"

#define CLD 3               // Card status pin 
#define RDT 4               // Data pin

typedef unsigned char byte;
void attachInterrupt(...);
int digitalRead(...);
#define CHANGE 1
#define FALLING 2
#define LOW 0

class MagCard {
private:
    bool global_bit;
    int buffer_counter;
    char buffer[400];
    char data[40];

public:
    MagCard() {
        this->global_bit = false;
        this->buffer_counter = 0;
        /* The interrupts are key to reliable reading of the clock and data feed */
        attachInterrupt(0, [this]() -> void { global_bit = !global_bit; } , CHANGE); // flips the global bit
        attachInterrupt(1, [this]() -> void { buffer[buffer_counter++] = global_bit; }, FALLING); // Writes the bit to the buffer
    
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
 
    char decodeByte(int thisByte[]) {
        char dict[16] = {'0', '1', '2', '3', '4', '5', '6', '7',
                        '8', '9', ':', ';', '<', '=', '>', '?' };
        byte data = 0;

        for (byte i = 0; i < 4; i++) {
            if (thisByte[i]) data += (1 << i); 
        } 

        return dict[data];
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
                this->data[charCount++] = this->decodeByte(thisByte);
            }
        }
    }

    user * find_rfid(user ** users, char * rfid, size_t n) {
        for (int i = 0; i < n; i++) {
            if (strstr(rfid, users[i]->get_rfid()) != nullptr) {
                return users[i];
            }
        }
        return nullptr;
    } 

    user * load_rfid(user ** users, char * rfid, size_t n) {
        user * u = nullptr;
        while ((u = find_rfid(users, rfid, n)) == nullptr) {
            while(digitalRead(CLD) == LOW) {
                decode();
                buffer_counter = 0;
            }
        }
        return u;
    }

}