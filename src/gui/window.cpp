#include <ncurses.h>
#include <gui/window.hpp>
#include <gui/utility.hpp>
#include <gui/window_driver_context.hpp>


namespace gui {


//Constructor
window::window(int recommended_size, color_t bg, color_t fg, bool border)
    : m_recommended_size(recommended_size), m_fg(fg), m_bg(bg), m_border(border)
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
   do_draw_screen(*m_ctx);
   unsetcolor(win, m_fg,m_bg);
   if(m_border) {
     wnoutrefresh(m_ctx->winm());
    // wnoutrefresh(m_ctx->win());
   }
   else wnoutrefresh(m_ctx->win());
}

void window::create(const rect& rect)
{
   if(!m_ctx) {
       m_ctx = std::make_unique<detail::window_driver_context>(rect,m_border);
       on_create(*m_ctx);
   }
}

// Resize window according to signal
void window::resize(const rect& rect)
{
    if(m_border) {
        mvwin(m_ctx->winm(),rect.y,rect.x);
        wresize(m_ctx->winm(),rect.cy,rect.cx);
        wclear(m_ctx->winm());  
        box(m_ctx->winm(),0,0);
  
        mvwin(m_ctx->win(),rect.y+1,rect.y+1);
        wresize(m_ctx->win(),rect.cy-2,rect.cx-2);
        wclear(m_ctx->win());
    } else {
        mvwin(m_ctx->win(),rect.y,rect.x);
        wresize(m_ctx->win(),rect.cy,rect.cx);
        wclear(m_ctx->win());
    }
    do_draw_screen(*m_ctx);
    if(m_border) {
        wnoutrefresh(m_ctx->winm());
    }
    else wnoutrefresh(m_ctx->win());
}


}