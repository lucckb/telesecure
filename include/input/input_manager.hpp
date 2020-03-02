#pragma once
#include <memory>
#include <functional>
#include <string>

namespace input {

    class input_manager {
    public:
        input_manager() {
        }
        input_manager(input_manager&) = delete;
        input_manager& operator=(input_manager&) = delete;
        static input_manager& get() {
            static input_manager obj;
            return obj;
        } 
        //Register switch window callback
        void register_switch_window( std::function<void(int)> cb ) {
            m_switch_window_cb = cb;
        }
        //Register delete char callback
        void register_delete_char( std::function<void()> cb ) {
            m_delete_char_cb = cb;
        }
        //Register add char callback
        void register_add_char( std::function<void(std::string_view)> cb ) {
            m_add_char_cb = cb;
        }
        //Register line completed callback
        void register_line_completed( std::function<void()> cb ) {
            m_line_completed_cb = cb;
        }
        //Register line completed callback
        void register_leave_session( std::function<void()> cb ) {
            m_leave_cb = cb;
        }
        //Enable forwarding to the readline
        void forward_to_readline(bool enable ) noexcept;
        //Register readline callback
        void register_readline_callback( std::function<void(int)> cb ) {
            m_readline_cb = cb;
        }
        //Handle input loop
        void loop();
    private:
        //Readline mode handle
        void readline_mode();
        //Normal mode handle
        void normal_mode();
    private:
        std::function<void(int)> m_switch_window_cb;
        std::function<void()> m_delete_char_cb;
        std::function<void(std::string_view)> m_add_char_cb;
        std::function<void()> m_line_completed_cb;
        std::function<void()> m_leave_cb;
        std::function<void(int)> m_readline_cb;
        bool m_forward_readline {};
    };
}
