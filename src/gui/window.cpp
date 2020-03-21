#include <ncurses.h>
#include <gui/window.hpp>
#include <gui/utility.hpp>
#include <gui/window_driver_context.hpp>


namespace gui {


//Constructor
window::window(int recommended_size, color_t bg, color_t fg)
    : m_recommended_size(recommended_size), m_fg(fg), m_bg(bg)
{
}

//! Destructor
window::~window()
{
}

 //! Draw empty window
bool window::paint()
{
   bool ret;
   auto win = m_ctx->win();
   //setcolor(win, m_fg,m_bg);
   wbkgd(win, colorpair(m_fg,m_bg));
   ret = do_draw_screen(*m_ctx);
   //unsetcolor(win, m_fg,m_bg);
   if(ret) {
        wnoutrefresh(m_ctx->win());
   }
   return ret;
}

void window::create(const rect& rect)
{
   if(!m_ctx) {
       m_ctx = std::make_unique<detail::window_driver_context>(rect);
       on_create(*m_ctx);
   }
}

// Resize window according to signal
void window::resize(const rect& rect)
{
    mvwin(m_ctx->win(),rect.y,rect.x);
    wresize(m_ctx->win(),rect.cy,rect.cx);
    do_draw_screen(*m_ctx);
    wnoutrefresh(m_ctx->win());
}

//Cursor final position on the last window
void window::cursor_set() noexcept
{
    curs_set(0);
}

}