/** Window curses abstraction base class */

namespace gui {
    class window {
    public:
        // Public constructor
        window( );
        //! Noncopyable 1
        window(window&) = delete;
        //! Noncopyable 2 
        window& operator=(window&) = delete;
    };
}