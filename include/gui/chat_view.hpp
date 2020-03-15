#pragma once 
#include "window.hpp"
#include <memory>

namespace gui {
    class chat_doc;

    class chat_view final : public window {
    public:
        //! Construtor
        chat_view( color_t bg, color_t fb );
        //! Destructor
        virtual ~chat_view(); 
        //Create new object instance
        static auto clone(color_t bg, color_t fg) {
            return std::make_shared<chat_view>(bg,fg);
        }
        //Assign view to the container
        void set_view( std::shared_ptr<chat_doc> view);
    protected:
         // Do draw screen function
        bool do_draw_screen(detail::window_driver_context& ctx) override;
        //On create
        //void on_create(detail::window_driver_context& ctx) override;
    private:
        std::shared_ptr<chat_doc> m_view;
    };
}

