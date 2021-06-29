#include "snake.hpp"


////////////////////////////////////////////////////////////////////////////

int constrain(const int & x, const int & a, const int & b) {
    if(x < a) {
        return a;
    }
    else if(b < x) {
        return b;
    }
    else
        return x;
}

int power(const int & base, const int pow){
    int result = 0;
    for(int i = 0; i < pow; i++){
        result = base * base;
    }
    return result;
}

int dist(const hwlib::xy & pos1, const hwlib::xy & pos2){
    int tmp =  power((pos2.x - pos1.x), 2) + power((pos2.y - pos1.y), 2);
    return sqrt(tmp);
}

bool within( int x, int a, int b ){
    return ( x >= a ) && ( x <= b );
}

// some useful functions

///////////////////////////////////////////////////////////////////////////

bool object::operator==(const object & rhs){
    return(location.x == rhs.location.x && location.y == rhs.location.y &&
           size.x == rhs.size.x && size.y == rhs.size.y);
}


bool object::overlaps( const object & other ) {

    bool x_overlap = within(
            location.x,
            other.location.x,
            other.location.x + other.size.x
    ) || within(
            other.location.x,
            location.x,
            location.x + size.x
    );

    bool y_overlap = within(
            location.y,
            other.location.y,
            other.location.y + other.size.y
    ) || within(
            other.location.y,
            location.y,
            location.y + size.y
    );

    return x_overlap && y_overlap;
}

// class object fuctions
////////////////////////////////////////////////////////////////////////////

wall::wall( hwlib::window & w, hwlib::xy location, hwlib::xy size, bool ij, bool filled):
        object(w, location, size),
        left(   w, location, hwlib::xy(location.x, size.y), hwlib::black),
        right(  w, hwlib::xy(size.x, location.y), size, hwlib::black),
        top(w, location, hwlib::xy(size.x , location.y), hwlib::black),
        bottom( w, hwlib::xy(location.x, size.y), size, hwlib::black),
        ij(ij),
        filled(filled)
{
    drawn = false;
}

void wall::draw(){

    if(!drawn) {
        if (filled) {
            hwlib::color ink = hwlib::black;
            if (ij) {
                for (int i = location.x; i <= size.x; i++) {
                    for (int j = location.y; j <= size.y; j++) {
                        w.write(hwlib::xy(i, j), ink);
                    }
                }
            } else {
                for (int i = location.y; i <= size.y; i++) {
                    for (int j = location.x; j <= size.x; j++) {
                        w.write(hwlib::xy(j, i), ink);
                    }
                }
            }
            drawn = true;

        } else {
            top.draw();
            left.draw();
            right.draw();
            bottom.draw();
            drawn = true;
        }
    }
}

// class wall functions
///////////////////////////////////////////////////////////////////////////

void snake::update() {

    if(length <= 0){
        hwlib::rectangle x0(location, hwlib::xy(location.x +4, location.y + 4), hwlib::white);
        x0.draw(w);
    }else{
        hwlib::rectangle x3(tail[0], hwlib::xy(tail[0].x +4, tail[0].y + 4), hwlib::white);
        x3.draw(w);

        for(int i = 0; i < length; i++){
            tail[i] = tail[i+1];
        }

        tail[length-1] = location;
    }

    hwlib::rectangle x3(tail[0], hwlib::xy(tail[0].x +4, tail[0].y + 4), hwlib::black);
    x3.draw(w);

    location.x = location.x + (speed.x * 4);
    location.y = location.y + (speed.y * 4);

    location.x = constrain(location.x, 8 + 4, 121 -4);
    location.y = constrain(location.y, 8 + 4, 121 -4);

    hwlib::rectangle x1(location, hwlib::xy(location.x +4, location.y + 4), hwlib::black);
    x1.draw(w);
}

void snake::interact(object &other) {
    if( this != & other){
        if( overlaps( other )){
                length++;
            }
        }
}

void snake::draw() {
    if(!drawn) {
        for(int i = 0; i < length; i++){
            hwlib::rectangle x(tail[i], hwlib::xy(tail[i].x +4, tail[i].y + 4), hwlib::black);
            x.draw(w);
        }
        drawn = true;
    }
}

void snake::directions(const int d){
    if(d == 0 && !left){
        right = true;
        left = false;
        up = false;
        down = false;
        speed.x = -1;
        speed.y = 0;
    }else if(d == 1 && !right){
        right = false;
        left = true;
        up = false;
        down = false;
        speed.x = 1;
        speed.y = 0;
    }else if(d == 2 && !down){
        right = false;
        left = false;
        up = true;
        down = false;
        speed.x = 0;
        speed.y = -1;
    }else if(d == 3 && !up){
        right = false;
        left = false;
        up = false;
        down = true;
        speed.x = 0;
        speed.y = 1;
    }
}


bool snake::win() {
    if(length == ARRAY_SIZE){
        return true;
    }else{
        return false;
    }
}

bool snake::death() {
    for (int i = 0; i < length; i++) {
        hwlib::xy x = tail[i];
        int d = dist(location, x);

        if (d == 0) {
            length = 0;
            return true;
        }
    }

    if(location.x <= 12 || location.x >= 117 || location.y <= 12 || location.y >= 117){
        return true;
    }

    return false;
}

// class snake functions
//////////////////////////////////////////////////////////////////////////

void food::draw() {
    if(!drwan) {
        hwlib::circle c(location + hwlib::xy(radius, radius), radius, hwlib::red);
        c.draw(w);
        drwan = true;
    }
}

void food::interact(object &other) {
    if( this != & other) {
        if (overlaps(other)) {
            hwlib::circle c( location + hwlib::xy( radius, radius ), radius, hwlib::white);
            c.draw( w );
            int x;
            int y;
            x = hwlib::rand() % 121;
            y = hwlib::rand() % 121;
            x = constrain(x, 8 + 10, 121 - 10);
            y = constrain(y, 8 + 10, 121 - 10);
            location = hwlib::xy(x, y);
            hwlib::circle c1(location + hwlib::xy(radius, radius), radius, hwlib::red);
            c1.draw(w);

        }
    }
}

// class food functions
/////////////////////////////////////////////////////////////////////////