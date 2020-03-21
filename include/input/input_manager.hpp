#pragma once
#include <memory>
#include <functional>
#include <string>

namespace app {
    class tele_app;
}

namespace input {

    class input_manager {
    public:
        input_manager(app::tele_app& app);
        input_manager(input_manager&) = delete;
        input_manager& operator=(input_manager&) = delete;
        //Handle input loop
        void loop();
        // Forward to readline
        void forward_to_readline(bool enable) noexcept;
    private:
        //Readline mode handle
        bool readline_mode();
        //Normal mode handle
        bool normal_mode();
        void back_multichars(const char* buf, const size_t n);
        int read_multichars(char* buf, const size_t n);
        
    private:
        bool m_forward_readline {};
        app::tele_app& m_app;
    };
}
