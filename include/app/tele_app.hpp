#pragma once

#include <memory>
#include <array>
#include <input/Console.hpp>
#include <shared_mutex>

namespace gui {
    class window_manager;
    class chat_doc;
}
namespace input {
    class input_manager;

}

namespace app {
    class telegram_cli;
    // Core class application
    class tele_app {
        static constexpr auto num_chats = 12;
        enum {
            win_status,
            win_view,
            win_edit
        };
    public:
        //Constructor
        tele_app(tele_app&) = delete;
        tele_app& operator=(tele_app&) = delete;
        tele_app();
        ~tele_app();
        //Main thread
        void run();
        //When new message from chat
        void on_new_message(uint64_t id, std::string_view name, std::string_view msg);
        //New control message
        void new_control_message(std::string_view msg);
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
        //When chat found
        std::pair<std::shared_ptr<gui::chat_doc>,int> find_chat(id_t id) noexcept;
    private:
        std::array<std::shared_ptr<gui::chat_doc>,num_chats> m_chats;
        int m_current_buffer {};
        std::unique_ptr<CppReadline::Console> m_console;
        const std::unique_ptr<telegram_cli> m_tcli;
        std::shared_mutex m_mtx;
    };
}
