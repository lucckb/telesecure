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
    for(;;)
    {
        if(m_forward_readline) readline_mode();
        else normal_mode();
    }
}

//Readline mode handle
void input_manager::readline_mode()
{
    const auto ch = getch();
    switch(ch) {
        // Leave mode
        case CTRL('c'): 
            return;
        //Leave temporary
        case CTRL('p'):
            m_leave_cb(); 
            break;  
        // Switch data buffer
        case KEY_F(1)...KEY_F(12):
           m_switch_window_cb(ch-KEY_F(1));
           break;
        default:
            // Forward to readline
            m_readline_cb(ch);
    } 
}
//Normal mode handle
void input_manager::normal_mode()
{
    wint_t ch;
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> convert;
    const auto ret = get_wch(&ch);
    switch(ch) 
    {
        // Leave mode
        case CTRL('c'): 
            return;
        //Leave temporary
        case CTRL('p'):
            m_leave_cb(); 
            break;  
        // Switch data buffer
        case KEY_F(1)...KEY_F(12):
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


 //Enable forwarding to the readline
void input_manager::forward_to_readline(bool enable) noexcept {
    m_forward_readline = enable;
    keypad(stdscr, m_forward_readline?FALSE:TRUE);	/* We get F1, F2 etc..		*/
    meta(stdscr, m_forward_readline?FALSE:TRUE);     /* Enable meta */
}

}