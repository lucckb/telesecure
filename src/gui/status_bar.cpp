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
            if(m_active==it.first) setcolor(win,fgcolor(),color_t::red, attrib_t::underline);
            else setcolor(win,fgcolor(),bgcolor());
            wprintw(win,"%s%c ", bar_name(i.username).c_str(),i.newmsg?'*':(i.online?'+':' '));
            if(m_active==it.first) unsetattributes(win);
        }
        curs_set(2);
    }
    return ret;
}

//Add new user to status bar
void status_bar::add_user( id_t id, std::string_view name )
{
    m_users.emplace(id,item(name,false,false));
    changed(true);
}

//Delete new user form status 
void status_bar::del_user( id_t id )
{
    const auto it = m_users.find(id);
    if( it != m_users.end() )
        m_users.erase(it);
        changed(true);
}

//Set message online
void status_bar::set_online( id_t id, bool online )
{
    const auto it = m_users.find(id);
    if( it != m_users.end() ) {
        it->second.online = online;
       changed(true);
    }
}

//Set new message
void status_bar::set_newmsg( id_t id, bool newmsg )
{
    const auto it = m_users.find(id);
    if( it != m_users.end() ) {
        it->second.newmsg = newmsg;
       changed(true);
    }
}

//Set status active
void status_bar::set_active( id_t id )
{
    m_active = id;
    changed(true);
}

}