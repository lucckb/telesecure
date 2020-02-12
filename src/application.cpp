#include <iostream>
#include <chrono>
#include <thread>
#include <gui/window_manager.hpp>
#include <td/telegram/Client.h>
#include <td/telegram/td_api.h>
#include <td/telegram/td_api.hpp>

#include <ncurses.h>
#include <gui/utility.hpp>

int main() {
    gui::window_manager wm;
    mvaddstr(0, 35, "COLOR DEMO");
    mvaddstr(2, 0, "low intensity text colors (0-7)");
    mvaddstr(12, 0, "high intensity text colors (8-15)");
    using namespace gui;
    for (int bg = 0; bg <= 7; bg++) {
        for (int fg = 0; fg <= 7; fg++) {
            setcolor(color_t(fg), color_t(bg));
            mvaddstr(fg + 3, bg * 10, "...test...");
            unsetcolor(color_t(fg), color_t(bg));
        }

        for (int fg = 8; fg <= 15; fg++) {
            setcolor(color_t(fg), color_t(bg));
            mvaddstr(fg + 5, bg * 10, "...test...");
            unsetcolor(color_t(fg), color_t(bg));
        }
    }

    mvaddstr(LINES - 1, 0, "press any key to quit");

    refresh();
    getch();
    endwin();
}