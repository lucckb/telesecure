#include <gui/window_manager.hpp>
#include <ncurses.h>
#include <stdexcept>
#include <gui/utility.hpp>
#include <gui/window.hpp>
#include <signal.h>
#include <sys/ioctl.h>
#include <system_error>
#include <future>
#include <locale.h>
#include <cstring>

namespace gui {

namespace {
    window_manager* m_mgr;
}


//! Constructor
window_manager::window_manager()
{
   //!Initialize ncurses
    if(m_mgr) {
        throw std::logic_error("Window manager already created");
    }
    curses_init();
    init_signals();
    m_mgr = this;
}

//! Destructor
window_manager::~window_manager() 
{   
    m_mgr = nullptr;
    curses_destroy();
}

//! Initialize ncurses library 
void window_manager::curses_init() 
{
      // Set locale attributes (including encoding) from the environment
    if (!setlocale(LC_ALL, "")) {
        throw std::logic_error("Failed to set locale attributes from environment");
    }
    initscr();			    /* Start curses mode 		*/
	raw();				    /* Line buffering disabled	*/
	keypad(stdscr, TRUE);	/* We get F1, F2 etc..		*/
    meta(stdscr, TRUE);     /* Enable meta */
	noecho();			    /* Don't echo() while we do getch */
    //curs_set(0);            /* Disable cursor */
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

  //! Add window
void window_manager::add_window( std::shared_ptr<window> win )
{
    m_winlist.push_back(std::move(win));
}

 //! Repaint all windowss
void window_manager::repaint( )
{
    wnoutrefresh(stdscr);
    for( auto wnd : m_winlist ) {
        if(!wnd) std::logic_error("Null Window exception");
        wnd->paint();
        wnd->changed(false);
    }
    doupdate();
}


 //Initialize signals
void window_manager::init_signals()
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = [](int) {
        auto a1 = std::async(std::launch::deferred, 
        []() {
            struct winsize w;
            if(ioctl(0, TIOCGWINSZ, &w)) {
                 throw std::system_error(errno,std::generic_category(),
                "unable to get terminal size"
            );
            }
            resize_term(w.ws_row,w.ws_col);
            if(m_mgr) m_mgr->resize_all();
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
    wclear(stdscr);
    wnoutrefresh(stdscr);
    rect r; int avg_y;
    calc_window_coord(r,avg_y);
    for( auto wnd : m_winlist ) {
        if(wnd->recommended_size()) {
            r.cy = wnd->recommended_size();
        } else {
            r.cy = avg_y;
        }
        wnd->changed(true);
        wnd->resize(r);
        wnd->changed(false);
        r.y += r.cy;
    }
    doupdate();
}

//! Create all windows according to the layout
void window_manager::create_all()
{
    rect r; int avg_y;
    calc_window_coord(r,avg_y);
    for( auto wnd : m_winlist ) {
        if(wnd->recommended_size()) {
            r.cy = wnd->recommended_size();
        } else {
            r.cy = avg_y;
        }
        wnd->create(r);
        r.y += r.cy;
    }
}

// Calculate window pos
void window_manager::calc_window_coord(rect& r, int& avg_y) const
{
   //Calculate full screen windows avg size
    int ws {};
    int nr {};
    for( auto wnd : m_winlist ) {
        if(wnd->recommended_size()) {
            ws += wnd->recommended_size();
        }
        else nr++;
    }
    int x,y;
    getmaxyx(stdscr,y,x);
    avg_y = (y - ws)/nr;
    r.x = 0; r.cx = x; r.y = 0;
}

}