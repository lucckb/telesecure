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
        static constexpr auto num_chats = 13;
        static constexpr auto code_failure = -1;
        enum {
            win_status, //Status window show chats in the top bar
            win_view,   //Chat view window is the main chat window
            win_edit    //Window edit for pass message to sender
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
        void on_new_message(std::int64_t id, std::int64_t msgid, std::string_view name, std::string_view msg, bool outgoing);
        //New control message
        void new_control_message(std::string_view msg) {
            std::unique_lock _lck(m_mtx);
            control_message_nlock(msg);
        }
         //Restore last buffers
        void restore_opened_buffers();
        //On user start or stop typing
        void on_user_typing( std::int64_t id, bool typing );
        //Set online status 
        void set_online_status( std::int64_t id, bool online);
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
        void on_switch_buffer_nolock(int window);
        void on_switch_buffer(int window);
        //! When readline parser complete commmand
        void on_readline_completed(int code);
        //! Open and create new chat
        int on_new_chat_create(const CppReadline::Console::Arguments& args);
        //On leave session
        void on_leave_session();
        //! When chat found
        std::pair<std::shared_ptr<gui::chat_doc>,int> find_chat(id_t id) noexcept;
        //! Find first free chat indentifier
        int find_free_chat_slot() noexcept;
        //! Find existing chat
        int find_existing_chat(id_t id) noexcept;
        //When delete chat handler
        int on_new_chat_delete(const CppReadline::Console::Arguments& args);
        //Control message pass without mutex protection
        void control_message_nlock(std::string_view msg);
        //Save buffer state
        void save_opened_buffers();
        //Read configuration
        std::vector<std::pair<int,long>> read_config();
    private:
        std::array<std::shared_ptr<gui::chat_doc>,num_chats> m_chats;
        int m_current_buffer {};
        std::unique_ptr<CppReadline::Console> m_console;
        const std::unique_ptr<telegram_cli> m_tcli;
        std::shared_mutex m_mtx;
    };
}
