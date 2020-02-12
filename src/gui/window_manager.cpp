#include <gui/window_manager.hpp>
#include <ncurses.h>
#include <stdexcept>
#include <gui/utility.hpp>

namespace gui {

//! Constructor
window_manager::window_manager()
{
    //!Initialize ncurses
    curses_init();
}

//! Destructor
window_manager::~window_manager() 
{
  curses_destroy();
}

//! Initialize ncurses library 
void window_manager::curses_init() 
{
    initscr();			    /* Start curses mode 		*/
	raw();				    /* Line buffering disabled	*/
	keypad(stdscr, TRUE);	/* We get F1, F2 etc..		*/
	noecho();			    /* Don't echo() while we do getch */
    if (has_colors() == FALSE) {
        endwin();
        throw std::logic_error("Your terminal does not support color");
    }
    start_color();
    init_colorpairs();
}

//! Init colorpairs
void window_manager::init_colorpairs()
{
    int fg, bg;
    int colorpair;
    for (bg = 0; bg <= 7; bg++) {
        for (fg = 0; fg <= 7; fg++) {
            colorpair = _internal::colornum(color_t(fg), color_t(bg));
            init_pair(colorpair, _internal::curs_color(color_t(fg)), _internal::curs_color(color_t(bg)));
        }
    }
}

//! Destroy ncurses library
void window_manager::curses_destroy()
{   
      endwin();			/* End curses mode	*/
} 

}