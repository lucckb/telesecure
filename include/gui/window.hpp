/** Window curses abstraction base class 
 * github vdksoft /signals  
*/

#pragma once

#include "utility.hpp"
#include <memory>



namespace gui {
    namespace detail {
        struct curses_coord;
        class window_driver_context;
    }
    //! Base class window
    class window {
    public:
        //! Public constructor
        window(int recommended_size, color_t bg, color_t fg, bool border);
        //! Destructor
        virtual ~window();
        //! Noncopyable 1
        window(window&) = delete;
        //! Noncopyable 2 
        window& operator=(window&) = delete; 
        // Paint window
        bool paint();
        // Resize window according to signal
        virtual void resize(const rect& rect);
        // Get border 
        auto has_border() const {
            return m_border;
        }
        auto recommended_size() const {
            return m_recommended_size;
        }
        // Real windows create
        void create(const rect& width);
         //Changed state
        void changed(bool val) noexcept {
            m_changed = val;
        }
        //Cursor final position on the last window
        virtual void cursor_set() noexcept;
    protected:
        // Draw window but without refresh
        virtual bool do_draw_screen(detail::window_driver_context& ctx) = 0;
        // When window is created
        virtual void on_create(detail::window_driver_context& ctx) {
        
        }
        //Get foreground color
        auto fgcolor() const noexcept {
            return m_fg;
        }
        //Get foreground color
        auto bgcolor() const noexcept {
            return m_bg;
        }
        //Get context
        auto& ctx() {
            return *m_ctx;
        } 
        //Changed state
        auto changed() const noexcept {
            return m_changed;
        }
    private:
        color_t m_fg {};    //! Foreground color
        color_t m_bg {};    //! Background color
        bool m_border {};   //! Draw border
        int m_recommended_size {};  //! Recommended size
        std::unique_ptr<detail::window_driver_context> m_ctx;
        bool m_changed {true};
    };
}