#include "spiUnit.h"
#include "TFT9341.h"
#include "touch.h"
#include "Button.h"
#include <Servo.h>
#include <string.h>

void rotate_servo() {
  Servo S;
  lcd.clrscr(BLACK);
  S.attach(3);
  S.write(0);
  for (int i = 0; i < 181; i++) {
    S.write(i);
    delay(10);
  }
  S.write(0); 
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
  	delay(120); 
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
	char msg[25];
	sprintf(msg, "withdraw? Balance: %d", balance);
  char messages[3][5] = {"50$", "100$", "200$"};
  byte starty = 60;
  Button btn_list[3];
  printToLcd(point(0, 10), WHITE, BLACK, 2, (char*)"How much would like to");
  printToLcd(point(0, 30), WHITE, BLACK, 2, msg);
  
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

bool option_screen(char * _name) { 

  byte n = strlen("Welcome! ") + strlen(_name) + 1;
  char * _welcome = new char[(size_t)n];
  sprintf(_welcome, "Welcome %s!", _name);
  
  lcd.clrscr(BLACK);
  printToLcd(point(0, 10), WHITE, BLACK, 3, _welcome);
  delete[] _welcome;
  Button dip_btn(point(40, 60), 235, 50, 7, BLUE, 3, (char*)"View details");
  dip_btn.draw();
  Button wd_btn(point(40, 120), 235, 50, 7, BLUE, 3, (char*)"  Withdraw");
  wd_btn.draw();

  while (true) { 
    lcdtouch.readxy();
    size_t x = 320 - lcdtouch.readx();
    size_t y = 240 - lcdtouch.ready();
    
    if (dip_btn.pressed(x, y)) {
      return (bool)0;
    }
    else if (wd_btn.pressed(x, y)) {
      return (bool)1;
    }
  }
}

void get_RFID(char * RFID) {
  delay(1000);
  return;
}

void welcome_screen(char * RFID) {
  lcd.clrscr(BLACK);
  printToLcd(point(15, 20), WHITE, BLACK, 3, (char*)"Plese Enter Your");
  printToLcd(point(15, 50), WHITE, BLACK, 3, (char*)"Card");
  get_RFID(RFID);
}

void setup() {	
	Serial.begin(9600);
	lcd.begin();
	lcdtouch.begin();
	lcd.clrscr (BLACK);
}


void loop() {
  char code[5] = "____";
  char RFID[12] = "";
  byte sum = 0;
  welcome_screen(RFID);
  button_screen(code);
  option_screen((char*)"Alen");
  if ((sum = wd_screen(100)) != 0) {
    rotate_servo();
  }
}
