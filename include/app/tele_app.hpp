#pragma once

#include <memory>
#include <array>
#include <input/Console.hpp>
#include <input/input_manager.hpp>
#include <shared_mutex>
#include <gui/window_manager.hpp>

namespace gui {
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
        void on_new_message(std::int64_t id, std::int64_t msgid, std::string_view name, std::string_view msg, bool outgoing, std::time_t date);
        //New control message
        void new_control_message(std::string_view msg) {
            std::unique_lock _lck(m_mtx);
            control_message_nlock(msg);
        }
        auto is_app_running() const noexcept {
            return m_app_running;
        }
         //Restore last buffers
        void restore_opened_buffers();
        //On user start or stop typing
        void on_user_typing( std::int64_t id, bool typing );
        //Set online status 
        void set_online_status( std::int64_t id, bool online);
        // When char is deleted
        void on_add_char(std::string_view ch);
        //On delete char
        void on_delete_char();
        //On leave session
        void on_leave_session();
        //Callback when input data completed
        void on_line_completed( );
        //When buffer should be switched
        void on_switch_buffer(int window);
        //On forward to readline
        void on_forward_char_to_readline(char ch);
        //On switch right
        void on_switch_right();
        //On switch right
        void on_switch_left();
        //On page up 
        void on_page_up();
        //On page down
        void on_page_down();
        //On clear edit
        void on_clear_edit();
    private:
        //Initialize gui
        void init_gui();
        //Initialize input
        void init_input();
        //Register commands
        void register_commands();
        // On switch buffer
        void on_switch_buffer_nolock(int window);
        //! When readline parser complete commmand
        void on_readline_completed(int code);
        //! Open and create new chat
        int on_new_chat_create(const CppReadline::Console::Arguments& args);
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
        std::array<std::shared_ptr<std::string>,num_chats> m_edit_lines;
        int m_current_buffer {};
        std::unique_ptr<CppReadline::Console> m_console;
        input::input_manager m_inp;
        const std::unique_ptr<telegram_cli> m_tcli;
        gui::window_manager m_win;
        std::shared_mutex m_mtx;
        bool m_app_running {true};
    };
}
