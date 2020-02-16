/** Window curses abstraction base class 
 * github vdksoft /signals  
*/

#pragma once

#include "utility.hpp"
#include <memory>
#include "curses_coord.h"



namespace gui {
    namespace detail {
        struct curses_coord;
        class window_driver_context;
    }
    //! Base class window
    class window {
    public:
        //! Public constructor
        window(float rx, float ry, float crx, float cry, color_t bg, color_t fg, bool border);
        //! Destructor
        virtual ~window();
        //! Noncopyable 1
        window(window&) = delete;
        //! Noncopyable 2 
        window& operator=(window&) = delete;
        // Paint window
        void paint();
        // Resize window according to signal
        virtual void resize();
    protected:
        // To ncurses coordinate calculator
        auto ncoord() const noexcept -> detail::curses_coord;
        // Draw window but without refresh
        virtual void do_draw_screen(detail::window_driver_context& ctx) = 0;
    private:
        float m_rx {};      //! Relative x start
        float m_ry {};      //! Relative y start
        float m_crx {};     //! Relative size x
        float m_cry {};     //! Relative size y
        color_t m_fg {};    //! Foreground color
        color_t m_bg {};    //! Background color
        bool m_border {};   //! Draw border 
        std::unique_ptr<detail::window_driver_context> m_ctx;
    };
}