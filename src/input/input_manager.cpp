#include <input/input_manager.hpp>
#include <ncurses.h>

namespace {
    static constexpr auto CTRL(int ch) {
        return ch ;
    }
}


namespace input 
{

//Handle input loop
void input_manager::loop()
{
    for(auto ch=getch();;ch=getch())
    {
        switch(ch) {
        case 27: 
            return;
        case KEY_F(1)...KEY_F(10):
           m_switch_window_cb(ch-KEY_F(1));
           break;
        case KEY_BACKSPACE:
        case KEY_DC:
        case 127:
            m_delete_char_cb();
            break;
        case 10:
            m_line_completed_cb();
            break;
        default:
            if(isprint(ch)) m_add_char_cb(ch);
            break;
        }
    }
}

}