/** Window manager base class for ncurses
 */
#pragma once

#include <memory>
#include <vector>

namespace gui {

    class window;

class window_manager {
public:
    //! Constructor
    window_manager( );
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
    void repaint( );
private:
    // Initialize ncurses library
    void curses_init();
    // Destroy ncurses library 
    void curses_destroy();
    //Init color pairs for terminal
    void init_colorpairs();
    //Translate color to enum 
private:
    std::vector<std::shared_ptr<window>> m_winlist;
};


}
