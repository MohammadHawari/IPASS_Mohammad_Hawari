#ifndef SNAKE_HPP
#define SNAKE_HPP

#include "hwlib.hpp"
#include "ILI9163.hpp"
#include <array>
#include <cmath>


#define ARRAY_SIZE 100          // max snake length

////////////////////////////////////////////////////////////////////////

// abstract class object
class object {
protected:

    hwlib::window & w;
    hwlib::xy location;
    hwlib::xy size;

public:

    object( hwlib::window & w, const hwlib::xy & location, const hwlib::xy & size):
            w( w ),
            location( location ),
            size( size )
    {}

    virtual void draw() = 0;
    virtual void update(){}
    bool overlaps( const object & other );
    virtual void interact( object & other ){}
    bool operator==(const object & rhs);
}; // class object

////////////////////////////////////////////////////////////////////////

// class line
class line : public object {
protected:

    hwlib::xy end;
    hwlib::color col;

public:

    line( hwlib::window & w, const hwlib::xy & location, const hwlib::xy & end, hwlib::color col):
            object( w, location, end - location),
            end( end ),
            col(col)
    {}

    void draw() override {
        hwlib::line x( location, end, col);
        x.draw( w );
    }

    line & operator=(const line & rhs){
        location.x = rhs.location.x;
        location.y = rhs.location.y;
        end.x = rhs.end.x;
        end.y = rhs.end.y;
        return *this;
    }
}; // class line

///////////////////////////////////////////////////////////////////////

// class snake
class snake : public object{

private:
    int length;
    std::array<hwlib::xy, ARRAY_SIZE> tail;
    hwlib::xy speed;
    bool left;
    bool up;
    bool down;
    bool right;
    bool drawn;

public:

    snake(hwlib::window & w,
          const hwlib::xy & location = hwlib::xy(50, 63),
          const hwlib::xy & end = hwlib::xy(0, 0)):
        object(w, location, end)
    {
        speed = hwlib::xy(1, 0);
        length = 0;
        left = true;
        up = false;
        down = false;
        right = false;
        drawn = false;
    }
    void directions(const int d);
    void update() override;
    void interact( object & other ) override;
    void draw() override;
    bool win();
    bool death();

}; // class snake

//////////////////////////////////////////////////////////////////////

// class circle
class circle : public object {
protected:

    int radius;

public:

    circle( hwlib::window & w, const hwlib::xy & midpoint, int radius):
            object( w,
                      midpoint - hwlib::xy( radius, radius ),
                      hwlib::xy( radius, radius ) * 2),
            radius( radius )
    {}

    void draw() override {
        hwlib::circle c( location + hwlib::xy( radius, radius ), radius);
        c.draw( w );
    }
}; // class circle

/////////////////////////////////////////////////////////////////////

// class food
class food : public circle{
    bool drwan;
public:
    food(hwlib::window & w, const hwlib::xy & midpoint):
        circle(w, midpoint, 3)
    {
        drwan = false;
    }
    void draw() override;
    void interact(object & other) override;
}; // class food

/////////////////////////////////////////////////////////////////////

// class wall
class wall : public object{

private:
    line left, right, top, bottom;
    bool ij;
    bool filled;
    bool drawn;

public:
    wall( hwlib::window & w, const hwlib::xy location, const hwlib::xy size, bool ij = true, bool filled = true);
    void draw() override;
}; // class wall

////////////////////////////////////////////////////////////////////

#endif //SNAKE_HPP
