#pragma once
#include <memory>
#include <ncurses.h>
#include "curses_coord.h"


namespace gui {
namespace detail {

     struct curses_coord;
     // Window private driver context
     struct window_driver_context {
         static auto window(int a,int b,int c, int d, bool border) -> WINDOW*;
         static auto mwindow(int a,int b,int c, int d, bool border, WINDOW *wnd) -> WINDOW*;
         window_driver_context( window_driver_context& ) = delete;
         window_driver_context& operator=(window_driver_context& ) = delete;
         window_driver_context(const detail::curses_coord& c, bool border);
        auto win() noexcept -> WINDOW*;
        auto winm() noexcept -> WINDOW*;
    private:
        std::unique_ptr<WINDOW,decltype(&delwin)> mwin;
        std::unique_ptr<WINDOW,decltype(&delwin)> xwin;
    };

}}