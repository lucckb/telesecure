#include <gui/window_manager.hpp>
#include <ncurses.h>
#include <stdexcept>
#include <gui/utility.hpp>
#include <gui/window.hpp>
#include <signal.h>
#include <sys/ioctl.h>
#include <system_error>
#include <future>

namespace gui {


//! Constructor
window_manager::window_manager()
{
    //!Initialize ncurses
   curses_init();
   init_signals();
  
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
	NC_CHECK(raw());				    /* Line buffering disabled	*/
	NC_CHECK(keypad(stdscr, TRUE));	/* We get F1, F2 etc..		*/
	NC_CHECK(noecho());			    /* Don't echo() while we do getch */
    if (has_colors() == FALSE) {
        endwin();
        throw std::logic_error("Your terminal does not support color");
    }
    NC_CHECK(start_color());
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
            NC_CHECK(init_pair(colorpair, _internal::curs_color(color_t(fg)), _internal::curs_color(color_t(bg))));
        }
    }
}

//! Destroy ncurses library
void window_manager::curses_destroy()
{   
    endwin();			/* End curses mode	*/
} 

  //! Add window
void window_manager::add_window( std::shared_ptr<window> win )
{
    m_winlist.push_back(std::move(win));
}

 //! Repaint all windowss
void window_manager::repaint( )
{
    NC_CHECK(wnoutrefresh(stdscr));
    for( auto wnd : m_winlist ) {
        wnd->paint();
    }
    NC_CHECK(doupdate());
}


 //Initialize signals
void window_manager::init_signals()
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = [](int) {
        auto a1 = std::async(std::launch::deferred, 
        []() {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));  
            struct winsize w;
            ioctl(0, TIOCGWINSZ, &w);
            (clearok(curscr,TRUE));
            (resize_term(w.ws_row,w.ws_col));
            get().resize_all();
        } );
        a1.wait();
    };
    if(sigaction(SIGWINCH, &sa, nullptr)) {
        throw std::system_error(errno,std::generic_category(),
             "unable to register signal for SIGWINCH"
            );
    }
}

//Resize all windows according to the screen size
 void window_manager::resize_all()
 {
    NC_CHECK(wresize(stdscr, LINES, COLS));
    NC_CHECK(wnoutrefresh(stdscr));
    NC_CHECK(clear());
    for( auto wnd : m_winlist ) {
        wnd->resize();
    }
    NC_CHECK(doupdate());
 }
 
}