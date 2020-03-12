#include <gui/chat_view.hpp>
#include <gui/chat_doc.hpp>
#include <gui/window_driver_context.hpp>
#include <ctime>

namespace gui {

namespace {
    std::string time2str(std::time_t tim)
    {
        char buf[32] {};
        auto tm = std::localtime(&tim);
        std::snprintf(buf,sizeof buf,"%02i:%02i",tm->tm_hour,tm->tm_min);
        return buf;
    }
    std::size_t linecount(std::string_view str, std::size_t maxx, std::size_t extra=0)
    {
        const int slen = utf8_strlen(str) + extra;
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

void chat_view::do_draw_screen( detail::window_driver_context& ctx )
{
    constexpr auto c_date_siz = 5;
    constexpr auto c_hdr_siz = 2;
    if( m_view ) {
        auto win = ctx.win();
        int maxx,maxy;
        const auto items = m_view->items();
        getmaxyx(win,maxy,maxx);
        //Calculate lines from the end
        const int hdrsiz = utf8_strlen(m_view->who()) + c_date_siz + c_hdr_siz;
        auto i = items.end();
        for (int nlines=0;i!=items.begin(); --i) { 
            nlines += linecount(i->line,maxx,hdrsiz);
            if(nlines>=maxy) break;
        }
        scrollok(win, TRUE);
        int ln=0;
        for (;i!=items.end(); ++i) {
            std::string who;
            if(!i->outgoing) {   //Is Sender
                setcolor(win,fgcolor(),bgcolor());
                who = m_view->who();
            } else {    //Im not sender
                setcolor(win,color_t::blue,bgcolor());
                who = "Me";
            }
            mvwprintw(win,ln,0,"%s@%s> %s\n",who.c_str(),time2str(i->time).c_str(),i->line.c_str());
            ln += linecount(i->line,maxx,hdrsiz);
        }
        scrollok(win, FALSE);
    }
}


//Assign view to the container
void chat_view::set_view(std::shared_ptr<chat_doc> view) 
{
    wclear(ctx().win());
    m_view = view;
}

/*
//On create
void chat_view::on_create(detail::window_driver_context& ctx)
{
}
*/

}