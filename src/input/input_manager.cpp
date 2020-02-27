#include <locale>
#include <input/input_manager.hpp>
#include <curses.h>
#include <locale>
#include <codecvt>


namespace {
    static constexpr auto CTRL(int ch) {
        return ch & 037;
    }
    struct key {
    enum key_ {
        backspace = 127,
        backspace2 = 263,
        enter = 10
    };};
}

namespace input 
{

//Handle input loop
void input_manager::loop()
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> convert;
    for(wint_t ch;;)
    {
        const auto ret = get_wch(&ch);
        switch(ch) {
        // Leave mode
        case CTRL('q'): 
            return;
        //Leave temporary
        case CTRL('p'):
            m_leave_cb(); 
            break;  
        // Switch data buffer
        case KEY_F(1)...KEY_F(10):
           m_switch_window_cb(ch-KEY_F(1));
           break;
        // Delete character
        case key::backspace:
            m_delete_char_cb();
            break;
        case key::backspace2:
            if(ret==KEY_CODE_YES) {
                m_delete_char_cb();
            } else {
                m_add_char_cb(convert.to_bytes(ch));
            }
            break;
        //Line completed
        case key::enter:
            m_line_completed_cb();
            break;
        //Forward to the input box
        default: 
            if(std::iswprint(ch) && ret!=KEY_CODE_YES) {
                m_add_char_cb(convert.to_bytes(ch));
            }
            break;
        }
    }
}

}