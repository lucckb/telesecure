#include <gui/chat_doc.hpp>


namespace gui
{

namespace {
    std::string string_time()
    {
        char buf[32] {};
        auto tim = std::time(nullptr);
        auto tm = std::localtime(&tim);
        std::snprintf(buf,sizeof buf,"%02i:%02i",tm->tm_hour,tm->tm_min);
        return buf;
    }
}

//Add single line
void chat_doc::add_line(std::string_view line, bool outgoing)
{ 
    using namespace std::string_literals;
    auto who = outgoing?"Me"s:m_who;
    auto fmtline = who + "@"s + string_time() + "> "s + std::string(line);
    m_items.emplace_back(std::move(fmtline),outgoing);
    while(m_items.size() > maxnlines) {
    m_items.pop_front();
    }
    m_curr_line = m_items.end();
    m_changed = true;
}

//Paging up or down
void chat_doc::rewind( int nlines )
{
    if(nlines>0) {
      //Not enough
      const auto diff = std::distance(m_curr_line,m_items.end());
      if( diff < nlines ) {
        return;
      }
      for(;m_curr_line!=m_items.end()&&nlines>0;++m_curr_line,--nlines) {}
    } else if(nlines<0) {
      nlines = -nlines;
      // Not enough
      const auto diff = std::distance(m_items.begin(),m_curr_line);
      if( diff < nlines ) {
        return;
      }
      for(;m_curr_line!=m_items.begin()&&nlines>0;--m_curr_line,--nlines) {}
    }
    m_changed = true;
}

}