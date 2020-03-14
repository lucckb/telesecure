#pragma once
#include <curses.h>
#include <stdexcept>
#include <string>

namespace gui 
{
    // Color base enumeration
    enum class color_t  {
        black, blue, green, cyan,
        red, magenta, yellow, white
    };
    enum class attrib_t {
        none,
        bold = 1U << 0U,
        underline = 1U << 1U
    };

    constexpr inline int operator&(  attrib_t a1, attrib_t a2 )
    {
        return int(a1) & int(a2);
    }
    constexpr inline attrib_t operator|(  attrib_t a1, attrib_t a2 )
    {
        return attrib_t(int(a1) | int(a2));
    }
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
    void setcolor(WINDOW* wnd, color_t fg, color_t bg, attrib_t attributes=attrib_t::none);
    //! Unset font from specific color
    void unsetcolor(WINDOW* wnd, color_t fg, color_t bg);
    //! Get the color pair ID
    int colorpair(color_t fg, color_t bg);
    //Unset attributes
    void unsetattributes(WINDOW *wnd);
    //! Pop UTF8 from string
    void pop_utf8(std::string& x);
    //! Utf8 strlen
    std::size_t utf8_strlen(std::string_view str);
    //! Text wrap
    std::string text_wrap(std::string str, std::size_t location);

}
namespace util {
 //! Get home directory
    std::string home_dir();
}

