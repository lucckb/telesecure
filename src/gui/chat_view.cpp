#include <gui/chat_view.hpp>
#include <gui/window_driver_context.hpp>

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
     auto win = ctx.win();
     int maxx,maxy;
     getmaxyx(win,maxy,maxx); maxy--;
     for (auto i = m_items.rbegin(); i != m_items.rend(); ++i,--maxy) {
       mvwprintw(win,maxy,0,"%s@%s> %s\n",i->who.c_str(),time2str(i->time).c_str(),i->line.c_str());
       if(maxy<=0 ) break;
     }
}

  //Add single line
void chat_view::add_line(std::string_view who, std::string_view line)
{      
      m_items.emplace_back(who,line);
      while(m_items.size() > maxnlines) {
        m_items.pop_front();
      }
      m_curr_line = m_items.end();
}

}