#pragma once

#include <memory>
#include <array>

namespace gui {
    class window_manager;
    class chat_doc;
}
namespace input {
    class input_manager;

}

namespace app {

    // Core class application
    class tele_app {
        static constexpr auto num_chats = 12;
        enum {
            win_status,
            win_view,
            win_edit
        };
    public:
        tele_app(tele_app&) = delete;
        tele_app& operator=(tele_app&) = delete;
        tele_app();
        void run();
    private:
        void init_gui();
        void init_input();
        //Callback when input data completed
        void on_line_completed( );
    private:
        std::array<std::shared_ptr<gui::chat_doc>,num_chats> m_chats;
    };
}
