#include <gui/chat_doc.hpp>

namespace gui
{

//Add single line
void chat_doc::add_line(std::string_view who, std::string_view line)
{      
      m_items.emplace_back(who,line);
      while(m_items.size() > maxnlines) {
        m_items.pop_front();
      }
      m_curr_line = m_items.end();
}

}