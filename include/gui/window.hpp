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
        void paint();
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
    protected:
        // Draw window but without refresh
        virtual void do_draw_screen(detail::window_driver_context& ctx) = 0;
        // When window is created
        virtual void on_create(detail::window_driver_context& ctx) {
        
        }
    private:
        color_t m_fg {};    //! Foreground color
        color_t m_bg {};    //! Background color
        bool m_border {};   //! Draw border
        int m_recommended_size {};  //! Recommended size
        std::unique_ptr<detail::window_driver_context> m_ctx;
    };
}