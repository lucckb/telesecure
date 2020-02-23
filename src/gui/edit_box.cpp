#include <gui/edit_box.hpp>
#include <gui/window_driver_context.hpp>
#include <functional>

namespace gui {


//! Construtor
edit_box::edit_box(color_t bg, color_t fg)
    : window(2,bg,fg,false)
{

}
//! Destructor
edit_box::~edit_box()
{

}

//! Draw screen
void edit_box::do_draw_screen( detail::window_driver_context& ctx )
{
    if(m_changed) {      
        auto win = ctx.win();
        if(m_delchar) {
            m_delchar = false;
            wclear(win);
            wprintw(win,m_line.c_str());
        } else {
            waddch(win,m_char);
        }
        m_changed = false;
    }
}

// When window is created
void edit_box::on_create(detail::window_driver_context& ctx)
{
    scrollok(ctx.win(),true);
}

//Add new character to the string
void edit_box::add_new_char( int ch )
{
    m_char = ch;
    m_changed = true;
    m_line += ch;
}

 //Delete char at cursor
void edit_box::del_char()
{
    m_changed = true;
    m_delchar = true;
    m_line.pop_back();
}

// Clear buffer 
void edit_box::clear()
{
    m_changed = true;
    m_line.clear();
}

}
 