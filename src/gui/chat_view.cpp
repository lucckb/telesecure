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

#if 0
void chat_view::do_draw_screen( detail::window_driver_context& ctx )
{
     if( m_view ) {
        auto win = ctx.win();
        int maxx,maxy;
        const auto items = m_view->items();
        getmaxyx(win,maxy,maxx); maxy--;
        for (auto i = items.rbegin(); i!=items.rend(); ++i) {
           const auto nlen = utf8_strlen(i->line);
           const auto nlines = nlen/maxx + !!(nlen%maxx);
           for(int j=0,k=nlines-1;j<nlen;j+=maxx,--k) {
               if(j==0) {
                    const auto s = i->line.substr(0,maxx).c_str();
                    mvwprintw(win,maxy+k,0,"%s@%s> %s\n",m_view->who().c_str(),time2str(i->time).c_str(),s);
               } else {
                   const auto s = i->line.substr(j,maxx).c_str();
                   mvwprintw(win,maxy+k,0,"%s",s);
               }
           }
           if(maxy<=0) break;
           maxy-=nlines;
      }
    }
}

#else
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
        int nlines=0;
        const int hdrsiz = utf8_strlen(m_view->who()) + c_date_siz + c_hdr_siz;
        auto i = items.rbegin();
        for (;i!=items.rend(); ++i) { 
            const int slen = utf8_strlen(i->line) + hdrsiz;
            nlines += slen/maxx + !!(slen/maxx);
            if(nlines>=maxy) break;
        }
        int j=0;
        scrollok(win, TRUE);
        for (std::list<std::string>::const_iterator k=(i+1).base(); k!=items.end(); ++k,++j) {
            mvwprintw(win,j,0,"%s@%s> %s\n",m_view->who().c_str(),time2str(i->time).c_str(),i->line.c_str());
        }
        scrollok(win, FALSE);
    }
}

#endif

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