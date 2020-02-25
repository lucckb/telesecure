#include <input/input_manager.hpp>
#include <ncurses.h>

namespace {
    static constexpr auto CTRL(int ch) {
        return ch & 037;
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
        // Leave mode
        case CTRL('q'): 
            return;
        //Leave temporary
        case CTRL('p'):
            m_leave_cb();
        // Switch data buffer
        case KEY_F(1)...KEY_F(10):
           m_switch_window_cb(ch-KEY_F(1));
           break;
        // Delete character
        case KEY_BACKSPACE:
        case KEY_DC:
        case 127:
            m_delete_char_cb();
            break;
        //Line completed
        case 10:
            m_line_completed_cb();
            break;
        //Forward to the input box
        default:
            if(isprint(ch)) m_add_char_cb(ch);
            break;
        }
    }
}

}