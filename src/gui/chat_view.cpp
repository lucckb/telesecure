#include <gui/chat_view.hpp>
#include <gui/chat_doc.hpp>
#include <gui/window_driver_context.hpp>
#include <ctime>

namespace gui {

namespace {
       std::string time2str(std::time_t tim)
       {
           auto tm = std::localtime(&tim);
           return std::to_string(tm->tm_hour) + ":" + std::to_string(tm->tm_min);
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
     if( m_view ) {
        auto win = ctx.win();
        int maxx,maxy;
        const auto items = m_view->items();
        getmaxyx(win,maxy,maxx); maxy--;
        for (auto i = items.rbegin(); i != items.rend(); ++i,--maxy) {
           mvwprintw(win,maxy,0,"%s@%s> %s\n",i->who.c_str(),time2str(i->time).c_str(),i->line.c_str());
           if(maxy<=0 ) break;
      }
    }
}

}