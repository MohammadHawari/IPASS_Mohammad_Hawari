#include "hwlib.hpp"
#include "ILI9163.hpp"

int main( void ) {
    namespace target = hwlib::target;

    auto scl = target::pin_out(target::pins::scl);
    auto sda = target::pin_out(target::pins::sda);
    auto cs = target::pin_out(target::pins::d8);
    auto wrx = target::pin_out(target::pins::d2);
    auto res = target::pin_out(target::pins::d9);
    auto spi_bus = hwlib::spi_bus_bit_banged_sclk_mosi_miso(scl, sda, hwlib::pin_in_dummy);
    auto ILI9163 = ILI9163_display(spi_bus, res, wrx, cs);
    auto ILI9163_1 = ILI9163_spi_128x128_buffered_res_wrx_cs(spi_bus, res, wrx, cs);

    ILI9163.clear(hwlib::white);

    ILI9163_1.clear(hwlib::green);
    ILI9163_1.flush();

    hwlib::cout << "end" << hwlib::endl;
}