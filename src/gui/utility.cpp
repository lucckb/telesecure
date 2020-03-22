#include <gui/utility.hpp>
#include <curses.h>
#include <string>
#include <sstream>
#include <codecvt>

namespace gui {

namespace _internal {
    short curs_color(color_t fg)
    {
        switch (fg) {           /* RGB */
            case color_t::black:                     /* 000 */
                return (COLOR_BLACK);
            case color_t::blue:                     /* 001 */
                return (COLOR_BLUE);
            case color_t::green:                     /* 010 */
                return (COLOR_GREEN);
            case color_t::cyan:                     /* 011 */
                return (COLOR_CYAN);
            case color_t::red:                     /* 100 */
                return (COLOR_RED);
            case color_t::magenta:                     /* 101 */
                return (COLOR_MAGENTA);
            case color_t::yellow:                     /* 110 */
                return (COLOR_YELLOW);
            case color_t::white:                     /* 111 */
                return (COLOR_WHITE);
            }
        return COLOR_BLACK;
    }
    int colornum(color_t fg, color_t bg)
    {
        constexpr int B = 1 << 7;
        const int bbb = (7 & int(bg)) << 4;
        const int ffff = 7 & int(fg);
        return (B | bbb | ffff);
    }
}

namespace {
    bool is_bold(color_t fg)
    {
        static constexpr auto i = 1 << 3;
        return (i & int(fg)); 
    }
    bool is_leading_utf8_byte(char c) {
        auto first_bit_set = (c & 0x80) != 0;
        auto second_bit_set = (c & 0X40) != 0;
         return !first_bit_set || second_bit_set;
    
    }
}

//Set color
void setcolor(WINDOW *wnd, color_t fg, color_t bg, attrib_t attributes)
{
    /* set the color pair (colornum) and bold/bright (A_BOLD) */
    wattron(wnd,COLOR_PAIR(_internal::colornum(fg, bg)));
    if(attributes&attrib_t::bold) wattron(wnd,A_BOLD);
    if(attributes&attrib_t::underline) wattron(wnd,A_UNDERLINE);
}
//Unset color
void unsetcolor(WINDOW *wnd,color_t fg, color_t bg)
{
/* unset the color pair (colornum) and
       bold/bright (A_BOLD) */
    wattroff(wnd,COLOR_PAIR(_internal::colornum(fg, bg)));
    if (is_bold(fg)) {
        wattroff(wnd,A_BOLD);
    }
}

//Unset attributes
void unsetattributes(WINDOW *wnd)
{
    wattroff(wnd, A_BOLD);
    wattroff(wnd,A_UNDERLINE);
}

int colorpair(color_t fg, color_t bg)
{
    return COLOR_PAIR(_internal::colornum(fg,bg));
}



void pop_utf8(std::string& x) {
    if(x.empty()) return;
    while (!is_leading_utf8_byte(x.back()))
        x.pop_back();
    x.pop_back();
}

std::vector<std::string> split_string(const std::string& str, std::size_t max) 
{
        std::vector<std::string> output;
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> convert;
        const auto wstr = convert.from_bytes(str);
        std::wistringstream iss(wstr);
	    std::wstring word;
        while((iss >> word)) {
            if(word.size()<= max ) {
                // Check if the last element can still hold another word (+ space)
                if (output.size() > 0 && (convert.from_bytes(output[output.size() - 1]).size() + word.size() + 1) <= max)
                    output[output.size() - 1] += ' ' + convert.to_bytes(word);
                else        
                    output.push_back(convert.to_bytes(word));
            } else {
                for(auto i=0UL,left=word.size(); i<word.size(); i+=max) {
                    const auto subs = word.substr(i,max);
                    output.push_back(convert.to_bytes(subs));
                }
            }
	    }
        return output;
}


}