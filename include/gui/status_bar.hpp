#pragma once
#include "window.hpp"
#include <string>
#include <map>
#include <input/Console.hpp>

namespace gui {
    class status_bar final: public window 
    {
    public:
        using id_t = int;
    private:
        struct item {
            item(std::string_view _username, bool _online, bool _newmsg )
                : username(_username), online(_online), newmsg(_newmsg)
            {
            }
            std::string username;   //Telegram user name
            bool online {};            //Is online
            bool newmsg {};            //Got new msg
            bool typing {};            //Is typing
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
        void add_user( int id, std::string_view name );
        //Del user from watch list
        void del_user( int id );
        //Set user online
        void set_online( int id, bool online );
        //New msg received
        void set_newmsg( int id, bool newmsg );
        //Set is typing
        void set_typing( int id, bool typing );
        //Set status active
        void set_active( int id );
    protected:
        // Do draw screen function
        bool do_draw_screen( detail::window_driver_context& ctx ) override;
    private:
        std::map<int,item> m_users;
        int m_active {};
    };
}