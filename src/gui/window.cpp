#include <ncurses.h>
#include <gui/window.hpp>
#include <gui/utility.hpp>

namespace gui {

namespace detail {
    // Window private driver context
     struct window_driver_context {
         static auto window(int a,int b,int c, int d, bool border)
         {
             return border?newwin(a,b,c,d):newwin(a,b,c,d);
         }
         static auto mwindow(int a,int b,int c, int d, bool border, WINDOW *wnd)
         {
             auto dw = derwin(wnd,a-2,b-2,1,1);
             return border?dw:nullptr;
         }
         window_driver_context( window_driver_context& ) = delete;
         window_driver_context& operator=(window_driver_context& ) = delete;
         window_driver_context(const detail::curses_coord& c, bool border)
            : mwin(window(c.nlines,c.ncols,c.y0,c.x0,border),&delwin)
            , xwin(mwindow(c.nlines,c.ncols,c.y0,c.x0,border,mwin.get()),&delwin)
        {
        }
        auto win() noexcept {
            return xwin?xwin.get():mwin.get();
        }
        auto winm() noexcept {
            return mwin.get();
        }
    private:
        std::unique_ptr<WINDOW,decltype(&delwin)> mwin;
        std::unique_ptr<WINDOW,decltype(&delwin)> xwin;
    };
 }

//Constructor
window::window( float rx, float ry, float crx, float cry, color_t bg, color_t fg, bool border )
    : m_rx(rx), m_ry(ry), m_crx(crx), m_cry(cry), m_fg(fg), m_bg(bg), m_border(border),
      m_ctx( std::make_unique<detail::window_driver_context>(ncoord(),border))
{
}

//! Destructor
window::~window()
{
}

 //! Draw empty window
void window::paint()
{
   auto win = m_ctx->win();
   setcolor(win, m_fg,m_bg);
   if(m_border) box(m_ctx->winm(),0,0);
   wbkgd(win, colorpair(m_fg,m_bg));
   do_draw_screen();
   unsetcolor(win, m_fg,m_bg);
   if(m_border) wnoutrefresh(m_ctx->winm());
   else wnoutrefresh(m_ctx->win());
}

// To ncurses coordinates
auto window::ncoord() const noexcept -> detail::curses_coord
{
    int row, col;
    detail::curses_coord ret;
    getmaxyx(stdscr,row,col);
    ret.x0 = float(col) * m_rx;
    ret.y0 = float(row) * m_ry;
    ret.nlines = float(row) * m_cry;
    if(ret.nlines==0) ret.nlines++;
    ret.ncols = float(col) * m_crx;
    if(ret.ncols==0) ret.ncols++;
    return ret;
} 


// Resize window according to signal
void window::resize()
{
    const auto c = ncoord();
    if(m_border) {
        mvwin(m_ctx->winm(),c.y0,c.x0);
        wresize(m_ctx->winm(),c.nlines,c.ncols);
        wclear(m_ctx->winm());  
        box(m_ctx->winm(),0,0);
  
        mvwin(m_ctx->win(),c.y0+1,c.y0+1);
        wresize(m_ctx->win(),c.nlines-2,c.ncols-2);
        wclear(m_ctx->win());
      
        
    } else {
        mvwin(m_ctx->win(),c.y0,c.x0);
        wresize(m_ctx->win(),c.nlines,c.ncols);
        wclear(m_ctx->win());
    }
    do_draw_screen();
    if(m_border) {
        wnoutrefresh(m_ctx->winm());
    }
    else wnoutrefresh(m_ctx->win());
}


}