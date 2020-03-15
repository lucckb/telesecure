#pragma once
#include "window.hpp"
#include <string>
#include <list>
#include <input/Console.hpp>

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
        //Create new object instance
        static auto clone(color_t bg, color_t fg)
        {
            return std::make_shared<status_bar>(bg,fg);
        }
        //Default constructor
        status_bar(color_t bg, color_t fg);
        //Destructor
        virtual ~status_bar();
        //Add newuser to watch list
        void add_user( id_t id, std::string_view name );
        //Del user from watch list
        void del_user( id_t id );
        //Set user online
        void set_online( id_t id, bool online );
        //New msg received
        void set_newmsg( id_t id, bool newmsg );
        //Set status active
        void set_active( id_t id );
    protected:
        // Do draw screen function
        bool do_draw_screen( detail::window_driver_context& ctx ) override;
    private:
        std::vector<std::pair<id_t,item>> m_users;
        id_t m_active {};
    };
}