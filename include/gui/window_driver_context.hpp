#pragma once

#include <memory>
#include <curses.h>


namespace gui {
    struct rect;
namespace detail {

     // Window private driver context
     struct window_driver_context {
         window_driver_context( window_driver_context& ) = delete;
         window_driver_context& operator=(window_driver_context& ) = delete;
         window_driver_context(const rect& c);
        auto win() noexcept -> WINDOW*;
    private:
        std::unique_ptr<WINDOW,decltype(&delwin)> mwin;
    };

}}