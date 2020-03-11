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

void chat_view::do_draw_screen( detail::window_driver_context& ctx )
{
     if( m_view ) {
        auto win = ctx.win();
        int maxx,maxy;
        const auto items = m_view->items();
        getmaxyx(win,maxy,maxx); maxy--;
        for (auto i = items.rbegin(); i != items.rend(); ++i,--maxy) {
           mvwprintw(win,maxy,0,"%s@%s> %s\n",m_view->who().c_str(),time2str(i->time).c_str(),i->line.c_str());
           if(maxy<=0 ) break;
      }
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