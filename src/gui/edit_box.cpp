#include <gui/edit_box.hpp>
#include <gui/window_driver_context.hpp>
#include <functional>
#include <wchar.h>
#include <wctype.h>
#include <cstring>
#include <readline/readline.h>

namespace gui {

namespace {
    // Makes a guess for malformed strings.
    static size_t strnwidth(const char *s, size_t n, size_t offset)
    {
        mbstate_t shift_state;
        wchar_t wc;
        size_t wc_len;
        size_t width = 0;

        // Start in the initial shift state
        memset(&shift_state, '\0', sizeof shift_state);

        for (size_t i = 0; i < n; i += wc_len) {
            // Extract the next multibyte character
            wc_len = mbrtowc(&wc, s + i, MB_CUR_MAX, &shift_state);
            switch (wc_len) {
            case 0:
                // Reached the end of the string
                goto done;

            case (size_t)-1: case (size_t)-2:
                // Failed to extract character. Guess that each character is one
                // byte/column wide each starting from the invalid character to
                // keep things simple.
                width += strnlen(s + i, n - i);
                goto done;
            }

            if (wc == '\t')
                width = ((width + offset + 8) & ~7) - offset;
            else
                // TODO: readline also outputs ~<letter> and the like for some
                // non-printable characters
                width += iswcntrl(wc) ? 2 : std::max(0, wcwidth(wc));
        }

    done:
        return width;
    }
    size_t strwidth(const char *s, size_t offset)
    {
        return strnwidth(s, SIZE_MAX, offset);
    }

}

//! Construtor
edit_box::edit_box(color_t bg, color_t fg)
    : window(2,bg,fg)
{
}

//! Destructor
edit_box::~edit_box()
{
}

//! Draw screen
bool edit_box::do_draw_screen( detail::window_driver_context& ctx )
{
    if(m_readline_mode) {
        on_readline_handle(rl_display_prompt, rl_line_buffer,rl_point);
        return true;
    } else {
        return draw_screen(ctx);
    }
}
//! Draw screen 2
bool edit_box::draw_screen(detail::window_driver_context& ctx)
{
    if(!m_line) return false;
    if(changed()) {      
        auto win = ctx.win();
        if(m_addchar) {
            waddstr(win,m_char.c_str());
        } else if(m_delchar) {
            int y,x;
            getyx(win,y,x);
            const auto maxx = getmaxx(win);
            if(x>0) { --x; }
            else { if(y>0) { --y; x=maxx-1; }}
            wmove(win,y,x);
            wdelch(win);
            if(y==0&&x==0) {
                waddstr(win,m_line->c_str());
            }
        }
        else {
            wclear(win);
            if(m_line->empty()) wmove(win,0,0); 
            else waddstr(win,m_line->c_str());
        } 
        m_addchar = m_delchar = false;
    }
    return changed();
}

// When window is created
void edit_box::on_create(detail::window_driver_context& ctx)
{
    scrollok(ctx.win(),true);
}

//Add new character to the string
void edit_box::add_new_char( std::string_view ch )
{
    if(!m_line) {
        throw std::logic_error("Null pointer exception");
    } 
    m_char = ch;
    changed(true);
    *m_line += ch;
    m_addchar = true;
}

 //Delete char at cursor
void edit_box::del_char()
{
    if(!m_line) {
        throw std::logic_error("Null pointer exception");
    } 
    changed(true);
    m_delchar = true;
    pop_utf8(*m_line);
}

// Clear buffer 
void edit_box::clear()
{
    if(!m_line) {
        throw std::logic_error("Null pointer exception");
    } 
    changed(true);
    m_line->clear();
}

 //Cursor set vinal position
void edit_box::cursor_set() noexcept
{
    int y,x;
    auto win = ctx().win();
    getyx(win,y,x);
    wmove(win,y,x);
    curs_set(2);
    wnoutrefresh(win);
}

// Readline handle outside the readline
void edit_box::on_readline_handle(const char* prompt, const char* linebuf, int point)
{
    size_t prompt_width = strwidth(prompt, 0);
    size_t cursor_col = prompt_width +
        strnwidth(linebuf, point, prompt_width);
    auto cmd_win = ctx().win();
    werase(cmd_win);
    mvwprintw(cmd_win, 0, 0, "%s%s", prompt, linebuf);
    if (cursor_col >= COLS)
        // Hide the cursor if it lies outside the window. Otherwise it'll
        // appear on the very right.
        curs_set(0);
    else {
        wmove(cmd_win, 0, cursor_col);
        curs_set(2);
    }
    // We batch window updates when resizing
    wrefresh(cmd_win);
    changed(true);
}

}   //GUI namespace END
 