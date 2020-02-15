#include <gui/utility.hpp>
#include <curses.h>

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
}



void setcolor(WINDOW *wnd, color_t fg, color_t bg)
{
    /* set the color pair (colornum) and bold/bright (A_BOLD) */
    wattron(wnd,COLOR_PAIR(_internal::colornum(fg, bg)));
    if (is_bold(fg)) {
        wattron(wnd,A_BOLD);
    }
}

void unsetcolor(WINDOW *wnd,color_t fg, color_t bg)
{
/* unset the color pair (colornum) and
       bold/bright (A_BOLD) */
    wattroff(wnd,COLOR_PAIR(_internal::colornum(fg, bg)));
    if (is_bold(fg)) {
        wattroff(wnd,A_BOLD);
    }
}

int colorpair(color_t fg, color_t bg)
{
    return COLOR_PAIR(_internal::colornum(fg,bg));
}

}