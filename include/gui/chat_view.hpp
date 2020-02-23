#pragma once 
#include "window.hpp"
#include <list>
#include <string>
#include <ctime>

namespace gui {
    class chat_view final : public window {
        static const auto maxnlines = 1000;
    public:
        struct item {
            item( std::string_view _who, std::string_view _line)
                : time(std::time(nullptr)),who(_who),line(_line)
            {}
            std::time_t time;
            std::string who;
            std::string line;
        };
        //! Construtor
        chat_view( color_t bg, color_t fb );
        //! Destructor
        virtual ~chat_view(); 
        //Create new object instance
        static auto clone(color_t bg, color_t fg) {
            return std::make_shared<chat_view>(bg,fg);
        }
        //Add single line
        void add_line(std::string_view who, std::string_view line);
    protected:
         // Do draw screen function
        void do_draw_screen(detail::window_driver_context& ctx) override;
    private:
        std::list<item> m_items;
        std::list<item>::iterator m_curr_line { m_items.end() };
    };
}

