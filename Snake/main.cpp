#include "hwlib.hpp"
#include "snake.hpp"

int main( void ) {
    namespace target = hwlib::target;

    auto scl = target::pin_out(target::pins::scl);
    auto sda = target::pin_out(target::pins::sda);
    auto cs = target::pin_out(target::pins::d8);
    auto wrx = target::pin_out(target::pins::d2);
    auto res = target::pin_out(target::pins::d9);
    auto spi_bus = hwlib::spi_bus_bit_banged_sclk_mosi_miso(scl, sda, hwlib::pin_in_dummy);
    auto ILI9163 = ILI9163_display(spi_bus, res, wrx, cs);
    auto knop_right = target::pin_in_out(target::pins::d22);
    auto knop_left = target::pin_in_out(target::pins::d24);
    auto knop_up = target::pin_in_out(target::pins::d28);
    auto knop_down = target::pin_in_out(target::pins::d26);
    knop_right.direction_set_input();
    knop_down.direction_set_input();
    knop_left.direction_set_input();
    knop_up.direction_set_input();

    ILI9163.clear(hwlib::white);

    wall bottom( ILI9163, hwlib::xy(   0,  0 ), hwlib::xy( 129,  8));
    wall top( ILI9163, hwlib::xy( 121,  0 ), hwlib::xy( 129, 129 ),false);
    wall left( ILI9163, hwlib::xy(  0, 121 ), hwlib::xy( 129, 129 ));
    wall right( ILI9163, hwlib::xy( 0,  0 ), hwlib::xy( 8, 129 ), false);
    snake s(ILI9163);
    food f(ILI9163, hwlib::xy(100, 63));

    std::array< object *, 2> objects = {&s, &f};

    left.draw();
    right.draw();
    top.draw();
    bottom.draw();
    s.draw();
    f.draw();

    bool result;

    for(;;) {

        hwlib::wait_ms(100);

        if(!knop_right.read()){
            s.directions(0);
        } if(!knop_left.read()){
            s.directions(1);
        } if(!knop_up.read()){
            s.directions(2);
        }if(!knop_down.read()){
            s.directions(3);
        }

        for (auto &p : objects) {
            p->update();
        }

        for (auto &p : objects) {
            for (auto &other : objects) {
                p->interact(*other);
            }
        }

        if(s.win()){
            result = true;
            break;
        }

        if(s.death()){
            result = false;
            break;

        }
    }

    if(result){
        ILI9163.clear(hwlib::blue);
    }else{
        ILI9163.clear(hwlib::red);
    }
}
