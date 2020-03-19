#include <gui/status_bar.hpp>
#include <gui/window_driver_context.hpp>
#include <gui/utility.hpp>

namespace gui {

namespace {
    std::string bar_name(const std::string& input ) {
        const auto np = input.find(' ');
        return input.substr(0,1) + input.substr(np+1,1);
    }
}

//Default constructor
status_bar::status_bar(color_t bg, color_t fg)
     : window(1,bg,fg,false)
{
}

//Destructor
status_bar::~status_bar()
{
}

// Do draw screen function
bool status_bar::do_draw_screen(detail::window_driver_context& ctx)
{
    auto win = ctx.win();
    auto ret {changed()};
    if(changed())
    {
        curs_set(0);
        wclear(win);
        for( const auto& it : m_users ) {
            const auto& i = it.second;
            const auto underline = m_active==it.first?attrib_t::underline:attrib_t::none;
            if(i.typing) {
                setcolor(win,fgcolor(),color_t::blue,underline);
            } else if(i.online) {
                setcolor(win,fgcolor(),color_t::yellow,underline);
            } else {
                setcolor(win,fgcolor(),bgcolor(),underline);
            }
            wprintw(win,"%s%c ", bar_name(i.username).c_str(),i.newmsg?'*':' ');
            if(m_active==it.first) unsetattributes(win);
        }
        curs_set(2);
    }
    return ret;
}

//Add new user to status bar
void status_bar::add_user( int id, std::string_view name )
{
    m_users.emplace(id,item(name,false,false));
    changed(true);
}

//Delete new user form status 
void status_bar::del_user( int id )
{
    const auto it = m_users.find(id);
    if( it != m_users.end() )
        m_users.erase(it);
        changed(true);
}

//Set message online
void status_bar::set_online( int id, bool online )
{
    const auto it = m_users.find(id);
    if( it != m_users.end() ) {
        it->second.online = online;
       changed(true);
    }
}

//Set new message
void status_bar::set_newmsg( int id, bool newmsg )
{
    const auto it = m_users.find(id);
    if( it != m_users.end() ) {
        it->second.newmsg = newmsg;
       changed(true);
    }
}

//Set message online
void status_bar::set_typing( int id, bool typing )
{
    const auto it = m_users.find(id);
    if( it != m_users.end() ) {
        it->second.typing = typing;
       changed(true);
    }
}

//Set status active
void status_bar::set_active( int id )
{
    m_active = id;
    changed(true);
}

}