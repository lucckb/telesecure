#include <ncurses.h>
#include <gui/window.hpp>
#include <gui/utility.hpp>

namespace gui {

namespace detail {
    // Window private driver context
     struct window_driver_context {
         window_driver_context( window_driver_context& ) = delete;
         window_driver_context& operator=(window_driver_context& ) = delete;
         window_driver_context(const detail::curses_coord& c)
            : win(newwin(c.nlines,c.ncols,c.y0,c.x0),&delwin)
        {}
        std::unique_ptr<WINDOW,decltype(&delwin)> win;
    };
 }

//Constructor
window::window( float rx, float ry, float crx, float cry, color_t bg, color_t fg, bool border )
    : m_rx(rx), m_ry(ry), m_crx(crx), m_cry(cry), m_fg(fg), m_bg(bg), 
      m_ctx( std::make_unique<detail::window_driver_context>(ncoord()))
{
 
}
//! Destructor
window::~window()
{
}



 //! Draw empty window
void window::paint()
{
   auto win = m_ctx->win.get();
   
   init_pair(1, COLOR_YELLOW, COLOR_BLUE);
   wattron(win,COLOR_PAIR(1));
   box(win,0,0);
   wprintw(win,"Some text");                  
   wattroff(win,COLOR_PAIR(1));
   wrefresh(win);
   
}

// To ncurses coordinates
auto window::ncoord() const noexcept -> detail::curses_coord
{
    int row, col;
    getmaxyx(stdscr,row,col);
    detail::curses_coord ret;
    ret.x0 = col * m_rx + 0.5f;
    ret.y0 = row * m_ry + 0.5f;
    ret.nlines = row * m_cry + 0.5f;
    ret.ncols = col * m_crx + 0.5f;
    return ret;
}

}