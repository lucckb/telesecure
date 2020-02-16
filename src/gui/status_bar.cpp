#include <gui/status_bar.hpp>

namespace gui {

//Default constructor
status_bar::status_bar(color_t bg, color_t fg)
     : window(0,0,1,0,bg,fg,false)
{

}
//Destructor
status_bar::~status_bar()
{

}

// Do draw screen function
void status_bar::do_draw_screen()
{

}

//Add new user to status bar
void status_bar::add_user( id_t id, std::string_view name )
{
    m_users.emplace(id,item(name,false,false));
}
//Delete new user form status 
void status_bar::del_user( id_t id )
{
    const auto it = m_users.find(id);
    if( it != m_users.end() )
        m_users.erase(it);
}
//Set message online
void status_bar::set_online( id_t id, bool online )
{
    const auto it = m_users.find(id);
    if( it != m_users.end() ) {
        it->second.online = online;
    }
}
//Set new message
void status_bar::set_newmsg( id_t id, bool newmsg )
{
    const auto it = m_users.find(id);
    if( it != m_users.end() ) {
        it->second.newmsg = newmsg;
    }
}

}