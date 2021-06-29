// ==========================================================================
//
// Author    : Mohammad Hawari
// File      : ILI9163.cpp
// Part of   : ILI9163 library for controlling a ILI9163 LCD display
// Copyright : Mohammad Hawari 2021.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// ==========================================================================

#include "ILI9163.hpp"

///@file

/// ILI9163_spi_res_wrx_cs constructor
///
/// construct by providing the spi bus and the res, wrx and cs pins
ILI9163_spi_res_wrx_cs::ILI9163_spi_res_wrx_cs(hwlib::spi_bus & bus,
                                               hwlib::target::pin_out & res,
                                               hwlib::target::pin_out & wrx,
                                               hwlib::target::pin_out & cs):
    bus( bus ),
    res( res ),
    wrx( wrx ),
    cs( cs ),
    cursor(255, 255)
    {
        res.write( 0 );
        hwlib::wait_ms( 1 );
        res.write( 1 );

        hwlib::wait_ms(20);
    }

/// send a command without data
void ILI9163_spi_res_wrx_cs::command( ILI9163_commands c ){
    wrx.write( 0 );
    wrx.flush();
    auto t = bus.transaction( cs );
    t.write( static_cast< uint8_t >( c ) );
}

/// send 8 bit parameter
void ILI9163_spi_res_wrx_cs::parameter( uint8_t p ){
    wrx.write(1);
    wrx.flush();
    auto t = bus.transaction( cs );
    t.write( p );
}

/// send 8 bit data
void ILI9163_spi_res_wrx_cs::data(uint8_t d){
    wrx.write(1);
    wrx.flush();
    auto t = bus.transaction( cs );
    t.write( d );
}

/// send 16 bit data
void ILI9163_spi_res_wrx_cs::data16(uint16_t d){
    wrx.write(1);
    wrx.flush();
    auto t = bus.transaction( cs );
    t.write( (d >> 8) & 0xff );
    t.write(d & 0xff );
}

/// set colom and page address then start a write transaction
void ILI9163_spi_res_wrx_cs::setAddress(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2) {
    command( ILI9163_commands::set_column_address);
    data16(x1);
    data16(x2);

    command( ILI9163_commands::set_page_address);
    data16(y1);
    data16(y2);
    // memory write
    command(ILI9163_commands::write_memory_start);
}

/// write the pixel byte d at column x page y with the color col
void ILI9163_spi_res_wrx_cs::pixels_byte_write(
        hwlib::xy location,
        uint16_t col
){

    if(location != cursor){
        setAddress(location.x, location.y, 0x81, 0x80);
        data16(col);
        cursor = location;
        cursor.x++;
    } else{
        data16(col);
        cursor.x++;
    }

    if(cursor.x >= 130){
        cursor.x = 0;
        cursor.y++;
    }
}

/// draw a filled rectangle
void ILI9163_spi_res_wrx_cs::drawRectFilled(uint16_t x,uint16_t y,uint16_t w,uint16_t h,uint16_t colour) {
    if ((x >= 128) || (y >= 128)) return;
    if ((x + w - 1) >= 128) w = 128 - x;
    if ((y + h - 1) >= 128) h = 128 - y;
    setAddress(x, y, x + w - 1, y + h - 1);
    for (y = h; y >= 0; y--)
        for (x = w; x >= 0; x--)
            data16(colour);
}

/// draw a biger pixel
void ILI9163_spi_res_wrx_cs::drawPixel(hwlib::xy location, uint8_t size, uint16_t colour) {
    if (size == 1) // default size or big size
        pixels_byte_write(location, colour);
    else
        drawRectFilled(location.x, location.y, size, size, colour);
}

/// clears the display with color col
void ILI9163_spi_res_wrx_cs::ILI9163_clear(uint16_t col) {

    for (int i = 0; i <= 128; i++) {
        for (int j = 0; j <= 129; j++) {
            pixels_byte_write(hwlib::xy(j, i), col);
        }
    }
}

//========================================================================================================


void ILI9163_spi_128x128_direct_res_wrx_cs::write_implementation(hwlib::xy pos, hwlib::color col){

    uint16_t d =
            ( ( static_cast< uint_fast16_t >(col.blue)   & 0x00f8 ) << 8)
            + ( ( static_cast< uint_fast16_t >(col.green) & 0x00fc ) << 3 )
            + ( ( static_cast< uint_fast16_t >(col.red)  & 0x00f8 ) >> 3 );
    pixels_byte_write(pos, d);

}

void ILI9163_spi_128x128_direct_res_wrx_cs::clear_implementation( hwlib::color col ){

    // convert hwlib color into uint16
    uint16_t d =
            ( ( static_cast< uint_fast16_t >(col.blue)   & 0x00f8 ) << 8)
            + ( ( static_cast< uint_fast16_t >(col.green) & 0x00fc ) << 3 )
            + ( ( static_cast< uint_fast16_t >(col.red)  & 0x00f8 ) >> 3 );

    ILI9163_clear(d);
}

/// ILI9163_spi_128x128_direct_res_wrx_cs constructor
///
/// construct by providing the spi channel and initialize the display
ILI9163_spi_128x128_direct_res_wrx_cs::ILI9163_spi_128x128_direct_res_wrx_cs(hwlib::spi_bus & bus,
                                                                             hwlib::target::pin_out & res,
                                                                             hwlib::target::pin_out & wrx,
                                                                             hwlib::target::pin_out & cs):

    ILI9163_spi_res_wrx_cs(bus, res, wrx, cs),
    window( wsize, hwlib::black, hwlib::white )
{
    // exit sleep mode
    command(ILI9163_commands::exit_sleep_mode);
    hwlib::wait_ms(5);

    // set pixel format to 16 bit 5,6,5 RGB
    command(ILI9163_commands::set_pixel_format);
    parameter(0x05);

    command(ILI9163_commands::set_gamma_curve);
    parameter(0x04);

    command(ILI9163_commands::GAM_R_SEL);
    parameter(0x01);

    command(ILI9163_commands::POSITIVE_GAMMA_CORRECT);
    parameter(0x3f); parameter(0x25); parameter(0x1c);
    parameter(0x1e); parameter(0x20); parameter(0x12);
    parameter(0x2a); parameter(0x90); parameter(0x24);
    parameter(0x11); parameter(0x00); parameter(0x00);
    parameter(0x00); parameter(0x00); parameter(0x00);

    command(ILI9163_commands::NEGATIVE_GAMMA_CORRECT);
    parameter(0x20); parameter(0x20); parameter(0x20);
    parameter(0x20); parameter(0x05); parameter(0x00);
    parameter(0x15); parameter(0xa7); parameter(0x3d);
    parameter(0x18); parameter(0x25); parameter(0x2a);
    parameter(0x2b); parameter(0x2b); parameter(0x3a);

    command(ILI9163_commands::FRAME_RATE_CONTROL1);
    parameter(0x08); parameter(0x08); // DIVA = 8 // VPA = 8

    command(ILI9163_commands::DISPLAY_INVERSION);
    parameter(0x07); // NLA = 1, NLB = 1, NLC = 1 (all on Frame Inversion)

    command(ILI9163_commands::POWER_CONTROL1);
    parameter(0x0a); // VRH = 10:  GVDD = 4.30
    parameter(0x02); // VC = 2: VCI1 = 2.65

    command(ILI9163_commands::POWER_CONTROL2);
    parameter(0x02); // BT = 2: AVDD = 2xVCI1, VCL = -1xVCI1, VGH = 5xVCI1, VGL = -2xVCI1

    command(ILI9163_commands::VCOM_CONTROL1);
    parameter(0x50); // VMH = 80: VCOMH voltage = 4.5
    parameter(0x5b); // VML = 91: VCOML voltage = -0.225

    command(ILI9163_commands::VCOM_OFFSET_CONTROL);
    parameter(0x40); // nVM = 0, VMF = 64: VCOMH output = VMH, VCOML output = VML

    command(ILI9163_commands::set_column_address);
    parameter(0x00); // XSH
    parameter(0x00); // XSL
    parameter(0x00); // XEH
    parameter(0x7f); // XEL (128 pixels x)

    command(ILI9163_commands::set_page_address);
    parameter(0x00);
    parameter(0x00);
    parameter(0x00);
    parameter(0x7f); // 128 pixels y

    // Select display orientation
    command(ILI9163_commands::set_address_mode);
    parameter(0x00);

    // Set the display to on
    command(ILI9163_commands::set_display_on);
    command(ILI9163_commands::write_memory_start);

}

//========================================================================================================

void ILI9163_spi_128x128_buffered_res_wrx_cs::write_implementation(hwlib::xy pos, hwlib::color col){

    int a = pos.x + wsize.x * pos.y;

    buffer[a] =
    ( ( static_cast< uint_fast16_t >(col.blue)   & 0x00f8 ) << 8)
    + ( ( static_cast< uint_fast16_t >(col.green) & 0x00fc ) << 3 )
    + ( ( static_cast< uint_fast16_t >(col.red)  & 0x00f8 ) >> 3 );
}

void ILI9163_spi_128x128_buffered_res_wrx_cs::clear_implementation( hwlib::color col ){

    uint16_t d =
            ( ( static_cast< uint_fast16_t >(col.blue)   & 0x00f8 ) << 8)
            + ( ( static_cast< uint_fast16_t >(col.green) & 0x00fc ) << 3 )
            + ( ( static_cast< uint_fast16_t >(col.red)  & 0x00f8 ) >> 3 );

    for (int i = 0; i <= 128; i++) {
        for (int j = 0; j <= 129; j++) {
            int a = j + wsize.x * i;
            buffer[a] = d;
        }
    }
}

/// ILI9163_spi_128x128_buffered_res_wrx_cs constructor
///
/// construct by providing the spi channel and initialize the display
ILI9163_spi_128x128_buffered_res_wrx_cs::ILI9163_spi_128x128_buffered_res_wrx_cs(hwlib::spi_bus & bus, hwlib::target::pin_out & res,
                                        hwlib::target::pin_out & wrx, hwlib::target::pin_out & cs):

    ILI9163_spi_res_wrx_cs(bus, res, wrx, cs),
    window( wsize, hwlib::black, hwlib::white )
{
    command(ILI9163_commands::exit_sleep_mode);
    hwlib::wait_ms(5);

    command(ILI9163_commands::set_pixel_format);
    parameter(0x05);

    command(ILI9163_commands::set_gamma_curve);
    parameter(0x04);

    command(ILI9163_commands::GAM_R_SEL);
    parameter(0x01);

    command(ILI9163_commands::POSITIVE_GAMMA_CORRECT);
    parameter(0x3f); parameter(0x25); parameter(0x1c);
    parameter(0x1e); parameter(0x20); parameter(0x12);
    parameter(0x2a); parameter(0x90); parameter(0x24);
    parameter(0x11); parameter(0x00); parameter(0x00);
    parameter(0x00); parameter(0x00); parameter(0x00);

    command(ILI9163_commands::NEGATIVE_GAMMA_CORRECT);
    parameter(0x20); parameter(0x20); parameter(0x20);
    parameter(0x20); parameter(0x05); parameter(0x00);
    parameter(0x15); parameter(0xa7); parameter(0x3d);
    parameter(0x18); parameter(0x25); parameter(0x2a);
    parameter(0x2b); parameter(0x2b); parameter(0x3a);

    command(ILI9163_commands::FRAME_RATE_CONTROL1);
    parameter(0x08); parameter(0x08); // DIVA = 8 // VPA = 8

    command(ILI9163_commands::DISPLAY_INVERSION);
    parameter(0x07); // NLA = 1, NLB = 1, NLC = 1 (all on Frame Inversion)

    command(ILI9163_commands::POWER_CONTROL1);
    parameter(0x0a); // VRH = 10:  GVDD = 4.30
    parameter(0x02); // VC = 2: VCI1 = 2.65

    command(ILI9163_commands::POWER_CONTROL2);
    parameter(0x02); // BT = 2: AVDD = 2xVCI1, VCL = -1xVCI1, VGH = 5xVCI1, VGL = -2xVCI1

    command(ILI9163_commands::VCOM_CONTROL1);
    parameter(0x50); // VMH = 80: VCOMH voltage = 4.5
    parameter(0x5b); // VML = 91: VCOML voltage = -0.225

    command(ILI9163_commands::VCOM_OFFSET_CONTROL);
    parameter(0x40); // nVM = 0, VMF = 64: VCOMH output = VMH, VCOML output = VML

    command(ILI9163_commands::set_column_address);
    parameter(0x00); // XSH
    parameter(0x00); // XSL
    parameter(0x00); // XEH
    parameter(0x7f); // XEL (128 pixels x)

    command(ILI9163_commands::set_page_address);
    parameter(0x00);
    parameter(0x00);
    parameter(0x00);
    parameter(0x7f); // 128 pixels y

    // Select display orientation
    command(ILI9163_commands::set_address_mode);
    parameter(0x00);

    // Set the display to on
    command(ILI9163_commands::set_display_on);
    command(ILI9163_commands::write_memory_start);

}

/// write the buffer to the display
void ILI9163_spi_128x128_buffered_res_wrx_cs::flush(){

    setAddress(0, 0, 0x81, 0x80);
    for (int i = 0; i <= 128; i++) {
        for (int j = 0; j <= 129; j++) {
            int a = j + wsize.x * i;
            data16(buffer[a]);
        }
    }
}

//========================================================================================================


















