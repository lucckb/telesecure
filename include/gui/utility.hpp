#pragma once
#include <ncurses.h>
#include <stdexcept>

namespace gui 
{
    // Color base enumeration
    enum class color_t  {
        black, blue, green, cyan,
        red, magenta, yellow, white
    };

    struct rect {
        rect() {}
        int x {}, y {};
        int cx {}, cy {};
    };

    namespace _internal {
        //Color to curs color
       short curs_color(color_t fg);
         // Pair creation
         int colornum(color_t fg, color_t bg);
    }

    //! Set font to specific color
    void setcolor(WINDOW* wnd, color_t fg, color_t bg);
    //! Unset font from specific color
    void unsetcolor(WINDOW* wnd, color_t fg, color_t bg);
    //! Get the color pair ID
    int colorpair(color_t fg, color_t bg);
}


