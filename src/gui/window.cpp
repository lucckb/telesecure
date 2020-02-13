#include <ncurses.h>
#include <gui/window.hpp>
#include <gui/utility.hpp>

namespace gui {

namespace detail {
    // Window private driver context
     struct window_driver_context {
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
    draw_empty();
}
//! Destructor

window::~window()
{
}

//! Resize window
void window::resize(int rows, int cols)
{
    m_row = rows;
    m_col = cols;
    draw_empty();
}

 //Draw empty window
void window::draw_empty()
{
   auto win = m_ctx->win.get();
   setcolor(m_fg, m_bg);
   wbkgd(win,COLOR_PAIR(1));
   unsetcolor(m_fg, m_bg);
}

// To ncurses coordinates
auto window::ncoord() const noexcept -> detail::curses_coord
{
    detail::curses_coord ret;
    ret.x0 = m_col * m_rx + 0.5f;
    ret.y0 = m_row * m_ry + 0.5f;
    ret.nlines = m_row * m_cry + 0.5f;
    ret.ncols = m_col * m_crx + 0.5f;
    return ret;
}

}