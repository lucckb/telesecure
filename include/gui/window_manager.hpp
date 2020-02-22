/** Window manager base class for ncurses
 */
#pragma once

#include <memory>
#include <vector>

namespace gui {

    class window;
    struct rect;

class window_manager {
public:
    static window_manager& get() {
        static window_manager wnd;
        return wnd;
    } 
    //! Noncopyable 1
    window_manager(window_manager&) = delete;
    //! Noncopyable 2 
    window_manager& operator=(window_manager&) = delete;
    //! Destructor
    ~window_manager();
    //! Add window
    void add_window( std::shared_ptr<window> win );
    //! Delete windows
    void delete_windows();
    //! Repaint all windowss
    void repaint();
    //! Resize all according to the screen size
    void resize_all();
    //! Create all windows according to the layout
    void create_all();
private:
     //! Constructor
    window_manager( );
    // Initialize ncurses library
    void curses_init();
    // Destroy ncurses library 
    void curses_destroy();
    //Init color pairs for terminal
    void init_colorpairs();
    //Initialize signals
    void init_signals();
    // Calculate window pos
    void calc_window_coord(rect& r, int& avg_y) const;
private:
    //All Window lists (overlapping doesnt work )
    std::vector<std::shared_ptr<window>> m_winlist;
};


}
