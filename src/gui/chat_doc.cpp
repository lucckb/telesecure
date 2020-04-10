#include <gui/chat_doc.hpp>
#include <gui/utility.hpp>

namespace gui
{

namespace {
    std::string string_time(std::time_t tim)
    {
        char buf[32] {};
        if(tim<=0) tim = std::time(nullptr);
        auto tm = std::localtime(&tim);
        std::snprintf(buf,sizeof buf,"%02i:%02i",tm->tm_hour,tm->tm_min);
        return buf;
    }
}

//Add single line
void chat_doc::add_line(std::string_view line, bool outgoing, std::time_t date)
{ 
    using namespace std::string_literals;
    auto who = outgoing?"Me"s:m_who;
    auto fmtline = who + "@"s + string_time(date) + "> "s + std::string(line);
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
        //Now we need to check after split all needed
        int virt_lines {};
        int real_lines {};
        for( auto it=m_curr_line; it!=m_items.end(); ++it ) {
            virt_lines += split_string(it->first,m_maxx).size();
            ++real_lines;
            if(virt_lines>=nlines) break;
        }
        if( virt_lines < nlines ) {
          //return;
        }
        for(;m_curr_line!=m_items.end()&&real_lines>0;++m_curr_line,--real_lines) {}
    } else if(nlines<0) {
        nlines = -nlines;
         //Now we need to check after split all needed
        int virt_lines {};
        int real_lines {};
        for( auto it=m_curr_line; it!=m_items.begin();) {
            --it;
            virt_lines += split_string(it->first,m_maxx).size();
            ++real_lines;
            if(virt_lines>=nlines) break;
        }
        if( virt_lines < nlines ) {
            return;
        }
        for(;m_curr_line!=m_items.begin()&&real_lines>0; --real_lines) {
            --m_curr_line;
        }
        ++m_curr_line;
    }
    m_changed = true;
}

}