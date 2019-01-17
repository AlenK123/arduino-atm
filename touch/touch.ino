#include "spiUnit.h"
#include "TFT9341.h"
#include "touch.h"
#include "Button.h"

#include <string.h>


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
	static byte pos = 0;
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


byte option_screen_touch(Button * btn_list) {
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

byte wd_option_screen(size_t balance) {
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

  byte sum =  option_screen_touch(btn_list);
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


void setup() {	
	Serial.begin(9600);
	lcd.begin();
	lcdtouch.begin();
	lcd.clrscr (BLACK);
	char code[5] = "____";
	//Serial.println(wd_option_screen(50));
  /*
	button_screen(code);
  Serial.print(code);
  */
}


void loop() {
}
