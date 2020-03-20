#include <gui/chat_view.hpp>
#include <gui/chat_doc.hpp>
#include <gui/window_driver_context.hpp>
#include <ctime>

namespace gui {

namespace {
   
    std::size_t linecount(std::string_view str, std::size_t maxx)
    {
        const int slen = utf8_strlen(str);
        return slen/maxx + !!(slen%maxx);
    }
}

//! Construtor
chat_view::chat_view(color_t bg, color_t fg)
       : window(0,bg,fg,false)
{
}

//! Destructor
chat_view::~chat_view()
{
}

//When screen should be redrawed
bool chat_view::do_draw_screen( detail::window_driver_context& ctx )
{
    if(m_view) {
        changed( changed()| m_view->changed() );
        m_view->displayed();
    }
    bool ret = changed();
    if( m_view && (changed()||m_view->changed()) ) {
        auto win = ctx.win();
        int maxx,maxy;
        getmaxyx(win,maxy,maxx);
        curs_set(0);
        //Calculate lines from the end
        const auto begin = m_view->begin();
        const auto end = m_view->end();
        auto i = end; 
        if(i!=begin) --i;
        for (int nlines=0;i!=begin; --i) { 
            nlines += linecount(i->first,maxx);
            if(nlines>=maxy) break;
        }
        wclear(win);
        scrollok(win, TRUE);
        for (;i!=end; ++i) {
            if(!i->second) {   //Is Sender
                setcolor(win,fgcolor(),bgcolor());
            } else {    //Im not sender
                setcolor(win,color_t::blue,bgcolor());
            }
            waddstr(win,i->first.c_str());
            waddch(win,'\n');
        }
        scrollok(win, FALSE);
        curs_set(2);
    }
    return ret;
}


//Assign view to the container
void chat_view::set_view(std::shared_ptr<chat_doc> view) 
{
    wclear(ctx().win());
    m_view = view;
    changed(true);
}

//Scroll up
void chat_view::scrolling(scroll_mode mode)
{
    auto win = ctx().win();
    int maxx,maxy;
    getmaxyx(win,maxy,maxx);
    maxy = maxy-1;
    m_view->rewind(mode==scroll_mode::up?-maxy:maxy);
    changed(true);
}


}