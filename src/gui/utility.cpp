#include <gui/utility.hpp>
#include <curses.h>
#include <string>

namespace gui {

namespace _internal {
    short curs_color(color_t fg)
    {
        switch (fg) {           /* RGB */
            case color_t::black:                     /* 000 */
                return (COLOR_BLACK);
            case color_t::blue:                     /* 001 */
                return (COLOR_BLUE);
            case color_t::green:                     /* 010 */
                return (COLOR_GREEN);
            case color_t::cyan:                     /* 011 */
                return (COLOR_CYAN);
            case color_t::red:                     /* 100 */
                return (COLOR_RED);
            case color_t::magenta:                     /* 101 */
                return (COLOR_MAGENTA);
            case color_t::yellow:                     /* 110 */
                return (COLOR_YELLOW);
            case color_t::white:                     /* 111 */
                return (COLOR_WHITE);
            }
        return COLOR_BLACK;
    }
    int colornum(color_t fg, color_t bg)
    {
        constexpr int B = 1 << 7;
        const int bbb = (7 & int(bg)) << 4;
        const int ffff = 7 & int(fg);
        return (B | bbb | ffff);
    }
}

namespace {
    bool is_bold(color_t fg)
    {
        static constexpr auto i = 1 << 3;
        return (i & int(fg)); 
    }
    bool is_leading_utf8_byte(char c) {
        auto first_bit_set = (c & 0x80) != 0;
        auto second_bit_set = (c & 0X40) != 0;
         return !first_bit_set || second_bit_set;
    
    }
}

//Set color
void setcolor(WINDOW *wnd, color_t fg, color_t bg, attrib_t attributes)
{
    /* set the color pair (colornum) and bold/bright (A_BOLD) */
    wattron(wnd,COLOR_PAIR(_internal::colornum(fg, bg)));
    if(attributes&attrib_t::bold) wattron(wnd,A_BOLD);
    if(attributes&attrib_t::underline) wattron(wnd,A_UNDERLINE);
}
//Unset color
void unsetcolor(WINDOW *wnd,color_t fg, color_t bg)
{
/* unset the color pair (colornum) and
       bold/bright (A_BOLD) */
    wattroff(wnd,COLOR_PAIR(_internal::colornum(fg, bg)));
    if (is_bold(fg)) {
        wattroff(wnd,A_BOLD);
    }
}

//Unset attributes
void unsetattributes(WINDOW *wnd)
{
    wattroff(wnd, A_BOLD);
    wattroff(wnd,A_UNDERLINE);
}

int colorpair(color_t fg, color_t bg)
{
    return COLOR_PAIR(_internal::colornum(fg,bg));
}



void pop_utf8(std::string& x) {
    while (!is_leading_utf8_byte(x.back()))
        x.pop_back();
    x.pop_back();
}

std::size_t utf8_strlen(std::string_view str)
{
    int c,i,ix,q;
    for (q=0, i=0, ix=str.length(); i < ix; i++, q++)
    {
        c = (unsigned char) str[i];
        if      (c>=0   && c<=127) i+=0;
        else if ((c & 0xE0) == 0xC0) i+=1;
        else if ((c & 0xF0) == 0xE0) i+=2;
        else if ((c & 0xF8) == 0xF0) i+=3;
        else if ((c & 0xFC) == 0xF8) i+=4; 
        else if ((c & 0xFE) == 0xFC) i+=5;
        else return 0;//invalid utf8
    }
    return q;
}

std::string text_wrap(std::string str, std::size_t location) 
{
    // your other code
    int n = str.rfind(' ', location);
    if (n != std::string::npos) {
        str.at(n) = '\n';
    }
    // your other code
    return str;
}

}