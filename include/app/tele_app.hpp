#pragma once

#include <memory>

namespace gui {
    class window_manager;
    class status_bar;
    class edit_box;
    class chat_view;
}
namespace input {
    class input_manager;

}

namespace app {

    // Core class application
    class tele_app {
    public:
        tele_app(tele_app&) = delete;
        tele_app& operator=(tele_app&) = delete;
        tele_app();
        void run();
    private:
        void init_gui();
    private:
        std::shared_ptr<gui::status_bar> m_bar;
        std::shared_ptr<gui::edit_box> m_edit;
        std::shared_ptr<gui::chat_view> m_view;
    };
}
