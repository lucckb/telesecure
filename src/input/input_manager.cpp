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
        escape = 27,
        right_arrow = 360,
        right_arrow2 = 560,
        left_arrow = 262,
        left_arrow2 = 545,
        page_up = 339,
        page_dn = 338
    };};

    constexpr auto keytab_sz = 12;
    constexpr char keytab[keytab_sz][4] = {
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

    constexpr auto arrow_sz = 2;
    constexpr char arrow_right[arrow_sz][5] = {
        {91,70}, // OSX kbd
        { '[','1',';','5','C'}   //Unix
    };
    constexpr char arrow_left[arrow_sz][5] = {
        {91,72},  // OSX kbd
        {'[','1',';','5','D'}   //Unix
    };

    constexpr auto pagekey_sz = 2;
    constexpr char page_up[pagekey_sz][5] = {
        { '[', '5', '~' }, // OSX kbd
        { '[', '5', '~' }   //Unix
    };
    constexpr char page_down[pagekey_sz][5] = {
        { '[', '6', '~' }, // OSX kbd
        { '[', '6', '~' }   //Unix
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
    if(ch==ERR) return false;
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
           //Switch buffer handle
           for(int i=0;i<keytab_sz;i++) {
               if(!memcmp(keytab[i],buf,n)) {
                    m_switch_window_cb(i);
                    return false;
               }
           }
           //Arrow right handle
           for(int i=0;i<arrow_sz;i++) {
               if(!memcmp(arrow_right[i],buf,n)) {
                    m_switch_right_cb();
                    return false;
               }
           }
           //Arrow left handle
           for(int i=0;i<arrow_sz;i++) {
               if(!memcmp(arrow_left[i],buf,n)) {
                    m_switch_left_cb();
                    return false;
               }
           }
           //Page up handle
           for(int i=0;i<pagekey_sz;i++) {
               if(!memcmp(page_up[i],buf,n)) {
                    m_pageup_cb();
                    return false;
               }
           }
          // Page down handle
          for(int i=0;i<pagekey_sz;i++) {
               if(!memcmp(page_down[i],buf,n)) {
                    m_pagedn_cb();
                    return false;
               }
          }
         // Other keys
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
    if(ch==ERR) return false;
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
        //Right arrow
        case key::right_arrow:
        case key::right_arrow2:
            m_switch_right_cb();
            break;
        //Left arrow
        case key::left_arrow:
        case key::left_arrow2:
            m_switch_left_cb();
            break;
        //Page up
        case key::page_up:
            m_pageup_cb();
            break;
        //Page down
        case key::page_dn:
            m_pagedn_cb();
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
void input_manager::forward_to_readline(bool enable) noexcept 
{
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