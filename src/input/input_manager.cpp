#include <locale>
#include <input/input_manager.hpp>
#include <curses.h>
#include <locale>
#include <codecvt>
#include <cstring>


namespace {
    constexpr auto CTRL(int ch) {
        return ch & 037;
    }
    struct key {
    enum key_ {
        backspace = 127,
        backspace2 = 263,
        enter = 10,
        escape = 27
    };};

    constexpr auto keytab_sz = 12;
    constexpr char keytab[12][4] = {
        {0x4f, 0x50, 0, 0}, //1
        {0x4f, 0x51, 0, 0}, //2
        {0x4f, 0x52, 0, 0}, //3
        {0x4f, 0x53, 0, 0}, //4
        {0x5b, 0x31, 0x35, 0x7e}, //5
        {0x5b, 0x31, 0x37, 0x7e}, //6
        {0x5b, 0x31, 0x38, 0x7e}, //7
        {0x5b, 0x31, 0x39, 0x7e}, //8
        {0x5b, 0x32, 0x30, 0x7e}, //9
        {0x5b, 0x32, 0x31, 0x7e}, //10
        {0x5b, 0x32, 0x32, 0x7e}, //11
        {0x5b, 0x32, 0x34, 0x7e} //12
    };

}

namespace input
{

//Handle input loop
void input_manager::loop()
{
    for(bool exitcode;;)
    {
        if(m_forward_readline) exitcode = readline_mode();
        else exitcode = normal_mode();
        if(exitcode) break;
    }
}

//Readline mode handle
bool input_manager::readline_mode()
{
    auto ch = getch();
    switch(ch) {
        // Leave mode
        case CTRL('c'): 
            return true;
        //Leave temporary
        case CTRL('p'):
            m_leave_cb(); 
            break;  
        // Switch data buffer
        case KEY_F(1)...KEY_F(12):
           m_switch_window_cb(ch-KEY_F(1));
           break;
        //Handle fn keypad manually
        case key::escape: {
           char buf[16];
           const auto n = read_multichars(buf,sizeof buf);
           for(int i=0;i<keytab_sz;i++) {
               if(!memcmp(keytab[i],buf,n)) {
                    m_switch_window_cb(i);
                    return false;
               }
           }
          m_readline_cb(key::escape);
          back_multichars(buf,n);
        }
        break;
        default:
            // Forward to readline
            m_readline_cb(ch);
    }
    return false;
}
//Normal mode handle
bool input_manager::normal_mode()
{
    wint_t ch;
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> convert;
    const auto ret = get_wch(&ch);
    switch(ch) 
    {
        // Leave mode
        case CTRL('c'): 
            return true;
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
    return false;
}


 //Enable forwarding to the readline
void input_manager::forward_to_readline(bool enable) noexcept {
    m_forward_readline = enable;
    //Keypad enabled only on nonreadline mode
    keypad(stdscr, m_forward_readline?FALSE:TRUE);	/* We get F1, F2 etc..		*/
}


int input_manager::read_multichars(char* buf, const size_t n)
{
    nodelay(stdscr,TRUE);
    int i;
    for(i=0;i<n;++i) {
        auto ch = getch();
        if(ch==ERR) {
            nodelay(stdscr,FALSE);
            return i;
        }
        buf[i] = ch;
    }
    nodelay(stdscr,FALSE);
    return i;
}

void input_manager::back_multichars(const char* buf, const size_t n)
{
    for(int i=0;i<n;++i) {
        m_readline_cb(buf[i]);
    }
}

}