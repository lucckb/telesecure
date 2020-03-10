#include <exception>
#include <cstdlib>
#include <iostream>
#include <curses.h>
#include <app/tele_app.hpp>


int main() {

    try {
        app::tele_app the_app;
        the_app.run();
    } catch( const std::exception& ex )
    {
        clear();
        printw("Software failure. Unhandled exception: %s\n", ex.what() );
        printw("Press any key to exit..");
        refresh();
        getch();
        return EXIT_FAILURE;   
    }
}