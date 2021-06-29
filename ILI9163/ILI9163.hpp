// ==========================================================================
//
// Author    : Mohammad Hawari
// File      : ILI9163.hpp
// Part of   : ILI9163 library for controlling a ILI9163 LCD display
// Copyright : Mohammad Hawari 2021.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// ==========================================================================

#ifndef ILI9163_HPP
#define ILI9163_HPP

#include "hwlib.hpp"

///@file

/// \brief
/// ILI9163 LCD color display
/// \details
/// This class implements an interface to an 130 x 130 pixel
/// color LCD display.
/// The interface is SPI 4 pins (SCL, SDA, CS, A0).
/// The driver chip is an ILI9163 a-Si TFT LCD .
///
/// The interface is direct: all writes
/// will be directly written to the display.
///
/// connect pin VCC to 5v and led to 3.3 v.
///
/// This type of display is reasonably priced
/// and available from lots of sources.

/// ILI9163 chip commands
enum class ILI9163_commands : uint8_t {
    nop                                   = 0x00,
    soft_reset                            = 0x01,
    get_red_channel                       = 0x06,
    get_green_channel                     = 0x07,
    get_blue_channel                      = 0x08,
    get_pixel_format                      = 0x0c,
    get_power_mode                        = 0x0a,
    get_address_mode                      = 0x0b,
    get_display_mode                      = 0x0d,
    get_signal_mode                       = 0x0e,
    get_diagnostic_result                 = 0x0f,
    enter_sleep_mode                      = 0x10,
    exit_sleep_mode                       = 0x11,
    enter_partial_mode                    = 0x12,
    enter_normal_mode                     = 0x13,
    exit_invert_mode                      = 0x20,
    enter_invert_mode                     = 0x21,
    set_gamma_curve                       = 0x26,
    set_display_off                       = 0x28,
    set_display_on                        = 0x29,
    set_column_address                    = 0x2a,
    set_page_address                      = 0x2b,
    write_memory_start                    = 0x2c,
    write_LUT                             = 0x2d,
    read_memory_start                     = 0x2e,
    set_partial_area                      = 0x30,
    set_scroll_area                       = 0x33,
    set_tear_off                          = 0x34,
    set_tear_on                           = 0x35,
    set_address_mode                      = 0x36,
    set_scroll_start                      = 0x37,
    exit_idle_mode                        = 0x38,
    enter_idle_mode                       = 0x39,
    set_pixel_format                      = 0x3a,
    write_memory_continue                 = 0x3c,
    read_memory_continue                  = 0x3e,
    set_tear_scanline                     = 0x44,
    get_scanline                          = 0x45,
    Read_ID1                              = 0xda,
    Read_ID2                              = 0xdb,
    Read_ID3                              = 0xdc,
    GAM_R_SEL                             = 0xf2,
    NEGATIVE_GAMMA_CORRECT                = 0xE1,
    POSITIVE_GAMMA_CORRECT                = 0xE0,
    POWER_CONTROL1                        = 0xC0,
    POWER_CONTROL2                        = 0xC1,
    POWER_CONTROL3                        = 0xC2,
    POWER_CONTROL4                        = 0xC3,
    POWER_CONTROL5                        = 0xC4,
    VCOM_CONTROL1                         = 0xC5,
    VCOM_CONTROL2                         = 0xC6,
    VCOM_OFFSET_CONTROL                   = 0xC7,
    FRAME_RATE_CONTROL1                   = 0xB1,
    FRAME_RATE_CONTROL2                   = 0xB2,
    FRAME_RATE_CONTROL3                   = 0xB3,
    DISPLAY_INVERSION                     = 0xB4,
    SOURCE_DRIVER_DIRECTION               = 0xB7,
    GATE_DRIVER_DIRECTION                 = 0xB8,
    WRITE_ID4_VALUE                       = 0xD3,
    NV_MEMORY_FUNCTION1                   = 0xD7,
    NV_MEMORY_FUNCTION2                   = 0xDE
};

// ==========================================================================
//
// ILI9163, accessed by spi
//
// ==========================================================================

/// abstract ILI9163 class
class ILI9163_spi_res_wrx_cs {
protected:

    // the spi bus & pins
    hwlib::spi_bus & bus;
    hwlib::target::pin_out & res;
    hwlib::target::pin_out & wrx;
    hwlib::target::pin_out & cs;

    // current cursor location in the controller
    hwlib::xy cursor;

public:

    ILI9163_spi_res_wrx_cs(hwlib::spi_bus & bus, hwlib::target::pin_out & res, hwlib::target::pin_out & wrx, hwlib::target::pin_out & cs);
    void command( ILI9163_commands c );
    void parameter( uint8_t p );
    void data(uint8_t d);
    void data16(uint16_t d);
    void setAddress(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);
    void pixels_byte_write(hwlib::xy location, uint16_t col);
    void drawRectFilled(uint16_t x,uint16_t y,uint16_t w,uint16_t h,uint16_t colour);
    void drawPixel(hwlib::xy location, uint8_t size, uint16_t colour);
    void ILI9163_clear(uint16_t col);

};

// ==========================================================================
//
// ILI9163, accessed by spi and a subclass of hwlib::window
//
// ==========================================================================

/// direct ILI9163 window
class ILI9163_spi_128x128_direct_res_wrx_cs : public ILI9163_spi_res_wrx_cs, public hwlib::window{

private:

    // display buffer
    static auto constexpr wsize = hwlib::xy(130, 129);

    void write_implementation(hwlib::xy pos, hwlib::color col) override;
    void clear_implementation( hwlib::color col ) override;

public:

    ILI9163_spi_128x128_direct_res_wrx_cs(hwlib::spi_bus & bus, hwlib::target::pin_out & res,
                                          hwlib::target::pin_out & wrx, hwlib::target::pin_out & cs);

    /// flush does nothing
    void flush() override {}
};


/// buffered ILI9163 window
class ILI9163_spi_128x128_buffered_res_wrx_cs : public ILI9163_spi_res_wrx_cs, public hwlib::window{


private:

    static auto constexpr wsize = hwlib::xy(130, 129);
    static auto constexpr buffsize = ((uint16_t) wsize.x * (uint16_t) wsize.y);
    uint16_t buffer[buffsize];
    void write_implementation(hwlib::xy pos, hwlib::color col) override;
    void clear_implementation( hwlib::color col ) override;

public:

    ILI9163_spi_128x128_buffered_res_wrx_cs(hwlib::spi_bus & bus, hwlib::target::pin_out & res,
                                            hwlib::target::pin_out & wrx,hwlib::target::pin_out & cs);
    void flush() override;
};

using ILI9163_display = ILI9163_spi_128x128_direct_res_wrx_cs;

#endif //ILI9163_HPP