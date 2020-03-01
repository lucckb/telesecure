#pragma once 
#include "window.hpp"
#include <string>

namespace gui {
    class edit_box final : public window {
    public:
        //! Construtor
        edit_box( color_t bg, color_t fb );
        //! Destructor
        virtual ~edit_box(); 
        //Create new object instance
        static auto clone(color_t bg, color_t fg) {
            return std::make_shared<edit_box>(bg,fg);
        }
        // Do draw screen function
        void do_draw_screen(detail::window_driver_context& ctx) override;
        // When window is created
        void on_create(detail::window_driver_context& ctx) override;
        //Add new character to the string
        void add_new_char( std::string_view ch );
        //Delete char at cursor
        void del_char();
        // Clear buffer 
        void clear();
        //Special function in readline mode
        void on_readline_handle(const char* prompt, const char* linebuf, int point);
    private:
        std::string m_char {};
        bool m_changed {};
        bool m_delchar {};
        std::string m_line;
    };
}