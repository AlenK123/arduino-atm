#pragma once
#include "point.h"

class Button {
private:
	size_t w;
	size_t l;
	size_t r;
	size_t color;
	size_t fs;
	point middle;

	char * message;
	public:

	point pos;

	Button(point pos, size_t w, size_t l, size_t r, size_t color, size_t fs, char * message) {
		this->pos = pos;
		this->w = w;
		this->l = l;
		this->r = r;
		this->fs = fs;
		this->color = color;
		this->middle = point(pos.x + 15, pos.y + 10);
		this->message = message;
	} 

	Button(point pos, char * message) {
		this->pos = pos;
		this->w = 40;
		this->l = 40;
		this->r = 5;
		this->fs = 4;
		this->color = RED; //button background color
		this->message = message;

		this->middle = point(pos.x + 10, pos.y + 8);
	}

	Button() {

	}

	void operator=(const Button& b) {
		this->pos = b.pos;
		this->w = b.w;
		this->l = b.l;
		this->r = b.r;
		this->fs = b.fs;
		this->color = b.color;
		this->middle = b.middle;
		this->message = b.message;
	}

	bool pressed(size_t _x, size_t _y) {
		return ( (_x >= this->pos.x) && (_x <= (this->pos.x + this->w)) && ( (_y >= this->pos.y) && (_y <= (this->pos.y + this->l))) );
	}

	const char * getMsg() {
		return this->message;
	}

	void draw() {
		lcd.fillRoundRect(this->pos.x, this->pos.y, this->w, this->l, this->r, this->color);
		//button foreground, background
		lcd.setColor(WHITE, this->color);
		lcd.setFont(this->fs);
		lcd.gotoxy(this->middle.x, this->middle.y);
		lcd.print(this->message);
	}

	void draw(size_t _x, size_t _y) {
		lcd.fillRoundRect(_x, _y, this->w, this->l, this->r, this->color);
		//button foreground, background
		lcd.setColor(WHITE, this->color);
		lcd.setFont(this->fs);
		lcd.gotoxy(this->middle.x, this->middle.y);
		lcd.print(this->message);
	}
};
