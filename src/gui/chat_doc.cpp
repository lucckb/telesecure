#include <gui/chat_doc.hpp>

namespace gui
{

//Add single line
void chat_doc::add_line(std::string_view line, bool outgoing)
{      
      m_items.emplace_back(line,outgoing);
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
      if( std::distance(m_curr_line,m_items.end()) < nlines ) {
        return;
      }
      for(;m_curr_line!=m_items.end()&&nlines>0;++m_curr_line,--nlines) {}
    } else if(nlines<0) {
      nlines = -nlines;
      // Not enough
      if( std::distance(m_items.begin(),m_curr_line) < nlines ) {
        return;
      }
      for(;m_curr_line!=m_items.begin()&&nlines>0;--m_curr_line,--nlines) {}
    }
    m_changed = true;
}

}