#pragma once 
#include "window.hpp"

namespace gui {
    class chat_view final : public window {
    public:
        //! Construtor
        chat_view( color_t bg, color_t fb );
        //! Destructor
        virtual ~chat_view(); 
         //Create new object instance
        static auto clone(color_t bg, color_t fg)
        {
            return std::make_shared<chat_view>(bg,fg);
        }
    protected:
         // Do draw screen function
        void do_draw_screen( detail::window_driver_context& ctx ) override;
    };
}

