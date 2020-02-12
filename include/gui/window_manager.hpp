/** Window manager base class for ncurses
 */

namespace gui {

class window_manager {
public:
    //! Constructor
    window_manager( );
    //! Noncopyable 1
    window_manager( window_manager& ) = delete;
    //! Noncopyable 2 
    window_manager& operator=(window_manager&) = delete;
    //! Destructor
    ~window_manager();
private:
    // Initialize ncurses library
    void curses_init();
    // Destroy ncurses library 
    void curses_destroy();
    //Init color pairs for terminal
    void init_colorpairs();
    //Translate color to enum 
};


}
