#pragma once
#include "window.hpp"
#include <string>
#include <map>

namespace gui {
    class status_bar final: public window 
    {
    public:
        using id_t = unsigned long;
    private:
        struct item {
            item(std::string_view _username, bool _online, bool _newmsg )
                : username(_username), online(_online), newmsg(_newmsg)
            {
            }
            std::string username;   //Telegram user name
            bool online;            //Is online
            bool newmsg;            //Got new msg
        };
    public: 
        static auto clone(color_t bg, color_t fg)
        {
            return std::make_shared<status_bar>(bg,fg);
        }
        void add_user( id_t id, std::string_view name );
        void del_user( id_t id );
        void set_online( id_t id, bool online );
        void set_newmsg( id_t id, bool newmsg );
        //Default constructor
        status_bar(color_t bg, color_t fg);
        //Destructor
        virtual ~status_bar();
    protected:
        // Do draw screen function
        virtual void do_draw_screen();
    private:
        std::map<id_t,item> m_users;
    };
}