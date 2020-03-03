#pragma once

#include <memory>
#include <array>
#include <input/Console.hpp>

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
        //Initialize GUI
        void init_gui();
        //Initialize input
        void init_input();
        //Register commands
        void register_commands();
        //Callback when input data completed
        void on_line_completed( );
        // On switch buffer
        void on_switch_buffer(int window);
        //! When readline parser complete commmand
        void on_readline_completed(int code);
    private:
        std::array<std::shared_ptr<gui::chat_doc>,num_chats> m_chats;
        int m_current_buffer {};
        std::unique_ptr<CppReadline::Console> m_console;
    };
}
