#pragma once

class point {
public:
    size_t x;
    size_t y;
    
    point() {}
    point(size_t _x, size_t _y) : x(_x), y(_y) {  }
    
    void operator=(const point& p) {
        this->x = p.x; 
        this->y = p.y;
    }
};