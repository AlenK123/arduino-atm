 #include "spiUnit.h"
#include "TFT9341.h"
#include "touch.h"
#include "Button.h"
#include <Servo.h>
#include <string.h>

#define CLD 3                           // Card status pin
#define RDT 4                           // Data pin

int reading = 2;                        // Reading status
volatile int * buffer = new int[400];   // Buffer for data
volatile int i = 0;                     // Buffer counter
volatile int bit = 0;                   // global bit
char * cardData = new char[40];         // holds card info
int charCount = 0;                      // counter for info

char * users_data[3] = {(char*)";;42", (char*)";382", (char*)";111"};
char * users[3] = {(char*)"Noa", (char*)"Alen", (char*)"Hadar"};
char * users_PIN[3] = {(char*)"1234", (char*)"2341", (char*)"2332"};
unsigned short user_balance[3] = { 500, 250, 200 };



void setup() {
    pinMode(13,OUTPUT);
    /* The interrupts are key to reliable reading of the clock and data feed */
    attachInterrupt(0, []() -> void { bit = !bit; }, CHANGE);
    attachInterrupt(1, []() -> void { buffer[i++] = bit; }, FALLING);

    
     
    Serial.begin(4800);
    lcd.begin();
    lcdtouch.begin();
}
 
void loop(){
    lcd.clrscr(BLACK);
    char code[5] = "____";
    byte sum = 0;
    byte index = get_user();
    bool ex = false;

    delete[] buffer;
    delete[] cardData;
    
    button_screen(code);
    while (strcmp(code, users_PIN[index]) != 0) {
      strcpy(code, "____");
      printToLcd(point(80, 120), RED, WHITE, 3, (char*)"Incorrect");
      printToLcd(point(80, 150), RED, WHITE, 3, (char*)"try again");
      delay(1000);
      button_screen(code);
    }
    while (!ex) {
        switch ((byte)option_screen(users[index])) {
        case 0:
            lcd.clrscr(BLACK);
            details_screen(index);
            break;
        case 1:
            lcd.clrscr(BLACK);
            while ((sum = wd_screen(user_balance[index])) == 0);
            user_balance[index] -= sum;
            /* activate device */
            Servo * ser = new Servo;
            ser->attach(9);
            ser->write(180);
            ser->write(0);
            delete ser;
            exit_screen();
            ex = true;
            break;
        case 2:
            ex = true;
            break;
        default:
            break;
        }
    }
}

void printToLcd(point pos, size_t fg, size_t bg, size_t fs, char * msg){
  lcd.gotoxy(pos.x,pos.y);
  lcd.setColor(fg, bg);
  lcd.setFont(fs);
  lcd.print(msg);
}

void button_screen(char * code) {
  lcd.clrscr(BLACK);

  size_t startx = 0;
  size_t starty = 55;
  Button btn_list[4][3];
  char msg[12][4][2] =  {{"1", "2", "3"}, {"4", "5", "6"}, {"7", "8", "9"}, {"*", "0", "#"}};
  Button btn;

  printToLcd(point(20, 20), WHITE, BLACK, 2, (char*)"Enter your PIN code: ");

  for (int i = 0; i < 4; i ++){
    startx = 20; 
    for (int j = 0; j < 3; j++){
      btn = Button(point(startx, starty), msg[i][j]);
      btn.draw();
      btn_list[i][j] = btn;
      startx += 55;
    }
    starty += 45;
  }
  button_screen_touch(btn_list, code);
  
}


void button_screen_touch(Button btn_list[4][3], char * code){
  byte pos = 0;
  printToLcd(point(190, 60), WHITE, BLACK, 3, code);
  while (pos < 4) {
    lcdtouch.readxy();
    size_t x = 320 - lcdtouch.readx();
    size_t y = 240 - lcdtouch.ready();
    delay(140); 
    for (byte i = 0; i < 4; i++){
      for (byte j = 0; j < 3; j++){
          if (btn_list[i][j].pressed(x, y)) {
            code[pos] = btn_list[i][j].getMsg()[0];
            pos++;
            printToLcd(point(190, 60), WHITE, BLACK, 3, code);
        }
      }
    } 
  }
}


byte wd_screen_touch(Button * btn_list) {
  while (true) {
    lcdtouch.readxy();
    size_t x = 320 - lcdtouch.readx();
    size_t y = 240 - lcdtouch.ready();
    //delay(120); 
    for (byte i = 0; i < 3; i++){
       if (btn_list[i].pressed(x, y)) {
            return atoi(btn_list[i].getMsg());
       }
    } 
  }
}

byte wd_screen(size_t balance) {
  lcd.clrscr(BLACK);
  char msg[4];
  sprintf(msg, "%d", balance);
  char messages[3][5] = {"50$", "100$", "200$"};
  byte starty = 60;
  Button btn_list[3];
  printToLcd(point(0, 10), WHITE, BLACK, 2, (char*)"How much would like to");
  printToLcd(point(0, 30), WHITE, BLACK, 2, (char*)"withdraw? Balance: ");
  printToLcd(point(220, 30), WHITE, BLACK, 2, msg);
  
  for (byte i = 0; i < 3; i ++) {
    Button opt_btn(point(10, starty), 100, 40, 5, BLUE,  3, messages[i]);
    opt_btn.draw();
    starty += 45;
    btn_list[i] = opt_btn;
  }

  byte sum =  wd_screen_touch(btn_list);
  if ((size_t)sum > balance) {
    printToLcd(point(120, 60), WHITE, RED, 3, (char*)"You can\'t");
    printToLcd(point(120, 85), WHITE, RED, 3, (char*)"withdraw ");
    printToLcd(point(120, 110), WHITE, RED, 3, (char*)"sums larger");
    printToLcd(point(120, 135), WHITE, RED, 3, (char*)"than your");
    printToLcd(point(120, 160), WHITE, RED, 3, (char*)"balance. ");
    delay(2000);
    return 0;
  }
  
  return sum;
}

byte option_screen(char * _name) { 

  byte n = strlen("Welcome! ") + strlen(_name) + 1;
  char * _welcome = new char[(size_t)n];
  sprintf(_welcome, "Welcome %s!", _name);
  
  lcd.clrscr(BLACK);
  printToLcd(point(0, 10), WHITE, BLACK, 3, _welcome);
  delete[] _welcome;
  Button det_btn(point(40, 55), 235, 50, 7, BLUE, 3, (char*)"View details");
  det_btn.draw();
  Button wd_btn(point(40, 110), 235, 50, 7, BLUE, 3, (char*)"  Withdraw");
  wd_btn.draw();
  Button ex_btn(point(40, 165), 235, 50, 7, BLUE, 3, (char*)"   exit");
  ex_btn.draw();

  while (true) { 
    lcdtouch.readxy();
    size_t x = 320 - lcdtouch.readx();
    size_t y = 240 - lcdtouch.ready();
    
    if (det_btn.pressed(x, y)) {
      return 0;
    }
    else if (wd_btn.pressed(x, y)) {
      return 1;
    }
    else if (ex_btn.pressed(x, y)) {
      return 2;
    }
  }
}

void welcome_screen(void) {
  lcd.clrscr(BLACK);
  printToLcd(point(15, 20), WHITE, BLACK, 3, (char*)"Plese Enter Your");
  printToLcd(point(15, 50), WHITE, BLACK, 3, (char*)"Card");
}

void exit_screen(void) {
  lcd.clrscr(BLUE);
  printToLcd(point(15, 20), WHITE, BLUE, 4, (char*)"Thank You");
  printToLcd(point(15, 50), WHITE, BLUE, 4, (char*)"Goodbye");
  delay(1000);
}

void details_screen(byte index) {
  char balance[] = "4294967296";
  sprintf(balance, "%d", user_balance[index]);
  Button back_btn(point(10, 140), 100, 40, 5, BLUE,  3, (char*)"Back");
  lcd.clrscr(BLACK);
  printToLcd(point(15, 20), WHITE, BLACK, 3, (char*)"Name: ");
  printToLcd(point(110, 20), WHITE, BLACK, 3, users[index]);
  printToLcd(point(15, 50), WHITE, BLACK, 3, (char*)"balance: ");
  printToLcd(point(143, 50), WHITE, BLACK, 3, balance);
  printToLcd(point(15, 90), WHITE, BLACK, 2, (char*)"Project by Hadar");
  printToLcd(point(23, 120), WHITE, BLACK, 2, (char*)"Amar and Alen Peer");
  back_btn.draw();
  while (true) { 
    lcdtouch.readxy();
    size_t x = 320 - lcdtouch.readx();
    size_t y = 240 - lcdtouch.ready();  
    if (back_btn.pressed(x, y)) return;
  }
  return;
}

/*
 * The part of the database, needs to be changed;
 */

int search_db() {
    for (byte i = 0; i < 3; i++) {
        if (strstr(cardData, users_data[i]) != NULL) {
            return i;
        }
    }
    return -1;
}

int get_user() {
  int index = 0;
  welcome_screen();
  while (1) {   
      while(digitalRead(CLD) == LOW){
        decode(&index);
        reading = 0;
        i = 0;
        int l;
        for (l = 0; l < 40; l = l + 1) {
          cardData[l] = '\n';
        }
        if (index != -1) return index;
        charCount = 0;
     }
  }
}
 

/* 
 *  This is the part that controls the magnetic card reader part of the  project
 */

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
 
void decode(int * _index) {
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


  int index = 0;
  if ((index = search_db()) != -1) {
      //return index;
      Serial.println("#########################");
      *_index = index;
      return;
      Serial.println(index);
  }
  *_index = -1;
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
